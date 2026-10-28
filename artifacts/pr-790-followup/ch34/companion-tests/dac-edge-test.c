/* The DMG all-DAC-off switch shares the source synthesis latency, without an
   additional DAC fade. Exercise writes and real sample callbacks; do not call
   the static mixer or manufacture a high-pass capacitor value. */
#define GB_INTERNAL
#include <Core/gb.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "pre-cgb-models.h"

enum { SAMPLE_RATE = 48000 }; /* Rate of the physical NOIZE JAMS capture. */
static unsigned callbacks;
static GB_sample_t last_sample;
static unsigned failures;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static void sample_callback(GB_gameboy_t *gb, GB_sample_t *sample)
{
    (void) gb;
    callbacks++;
    last_sample = *sample;
}

static void next_sample(GB_gameboy_t *gb)
{
    unsigned before = callbacks;
    while (callbacks == before) {
        /* One CPU M-cycle advances the ordinary sample clock and APU timing. */
        GB_advance_cycles(gb, 4);
        GB_apu_run(gb, true);
    }
    CHECK(callbacks == before + 1);
}

static const unsigned dac_registers[] = {
    GB_IO_NR12, GB_IO_NR22, GB_IO_NR30, GB_IO_NR42,
};

static uint8_t enabled_value(unsigned channel)
{
    return channel == GB_WAVE ? 0x80 : 0xf0;
}

static GB_gameboy_t *setup(GB_model_t model, unsigned channel, int other)
{
    GB_gameboy_t *gb = GB_alloc();
    GB_init(gb, model);
    if (gb->sgb) gb->sgb->intro_animation = GB_SGB_INTRO_ANIMATION_LENGTH;
    GB_set_sample_rate(gb, SAMPLE_RATE);
    GB_set_highpass_filter_mode(gb, GB_HIGHPASS_ACCURATE);
    GB_set_interference_volume(gb, 0);
    GB_apu_set_sample_callback(gb, sample_callback);
    GB_apu_write(gb, GB_IO_NR52, 0);
    GB_apu_write(gb, GB_IO_NR52, 0x80);
    GB_apu_write(gb, GB_IO_NR50, 0x77);

    if (channel == GB_WAVE) {
        for (unsigned reg = GB_IO_WAV_START; reg <= GB_IO_WAV_END; reg++) {
            GB_apu_write(gb, reg, 0xff);
        }
        GB_apu_write(gb, GB_IO_NR32, 0x20);
    }
    else if (channel == GB_NOISE) {
        /* Shift 15 halts the LFSR, giving a constant sample for this test. */
        GB_apu_write(gb, GB_IO_NR43, 0xf0);
    }
    GB_apu_write(gb, dac_registers[channel], enabled_value(channel));
    const unsigned trigger[] = {GB_IO_NR14, GB_IO_NR24, GB_IO_NR34, GB_IO_NR44};
    GB_apu_write(gb, trigger[channel], 0x80);
    if (other >= 0) {
        /* An enabled, untriggered DAC remains connected even without NR51. */
        GB_apu_write(gb, dac_registers[other], enabled_value(other));
        CHECK(!gb->apu.is_active[other]);
    }
    GB_apu_write(gb, GB_IO_NR51, 0x11 << channel);

    /* Fill the existing synthesis kernel and finish its documented DAC attack;
       this is a derived settling interval, not a tolerance for the tested edge. */
    unsigned settle = GB_BAND_LIMITED_WIDTH + ceil((double) SAMPLE_RATE / DAC_ATTACK_SPEED);
    for (unsigned i = 0; i < settle; i++) {
        next_sample(gb);
    }
    CHECK(gb->apu_output.dac_discharge[channel] == 1);
    CHECK(gb->apu_output.highpass_diff.left != 0);
    CHECK(gb->apu_output.highpass_diff.right != 0);
    return gb;
}

static void check_edge(GB_model_t model, unsigned channel, int other)
{
    GB_gameboy_t *gb = setup(model, channel, other);
    bool disconnected = model < GB_MODEL_CGB_0 && other < 0;
    GB_gameboy_t *connected_control = disconnected ? setup(model, channel, other) : NULL;
    GB_double_sample_t capacitor = gb->apu_output.highpass_diff;
    GB_apu_write(gb, dac_registers[channel], 0);
    CHECK(!GB_apu_is_DAC_enabled(gb, channel));
    unsigned latency = model < GB_MODEL_CGB_0 && gb->apu_output.max_cycles_per_sample != 1 ?
        GB_BAND_LIMITED_WIDTH / 2 : 0;
    /* Already queued source samples still precede the register transition.
       Require their actual output and capacitor evolution to match a twin
       whose DAC remains enabled, rather than accepting an early mute. */
    for (unsigned i = 0; i < latency; i++) {
        next_sample(gb);
        GB_sample_t queued_sample = last_sample;
        if (connected_control) {
            next_sample(connected_control);
            CHECK(queued_sample.packed == last_sample.packed);
            CHECK(gb->apu_output.highpass_diff.left == connected_control->apu_output.highpass_diff.left);
            CHECK(gb->apu_output.highpass_diff.right == connected_control->apu_output.highpass_diff.right);
        }
    }
    capacitor = gb->apu_output.highpass_diff;
    printf("model=%x last_channel=%u other_enabled_unrouted=%d", model, channel, other);
    /* Cover the entire old 50 us fade: ceil(48000 / 20000) = 3 renders. */
    unsigned decay_samples = ceil((double) SAMPLE_RATE / DAC_DECAY_SPEED);
    for (unsigned i = 0; i < decay_samples; i++) {
        next_sample(gb);
        printf(" sample%u=%d,%d capacitor%u=%a,%a", i, last_sample.left,
               last_sample.right, i, gb->apu_output.highpass_diff.left,
               gb->apu_output.highpass_diff.right);
        if (disconnected) {
            CHECK(last_sample.left == 0 && last_sample.right == 0);
            CHECK(gb->apu_output.highpass_diff.left == capacitor.left);
            CHECK(gb->apu_output.highpass_diff.right == capacitor.right);
        }
        else {
            /* CGB retains its existing analog path; another enabled DMG DAC
               must also keep filtering, regardless of activity or routing. */
            CHECK(last_sample.left != 0 || last_sample.right != 0);
            CHECK(gb->apu_output.highpass_diff.left != capacitor.left);
            CHECK(gb->apu_output.highpass_diff.right != capacitor.right);
        }
    }
    puts("");
    if (connected_control) GB_dealloc(connected_control);
    GB_dealloc(gb);
}

int main(void)
{
    for (unsigned channel = 0; channel < GB_N_CHANNELS; channel++) {
        check_edge(GB_MODEL_CGB_E, channel, -1);
    }
    for (unsigned m = 0; m < sizeof(pre_cgb_models) / sizeof(pre_cgb_models[0]); m++) {
        for (unsigned channel = 0; channel < GB_N_CHANNELS; channel++) {
            check_edge(pre_cgb_models[m], channel, -1);
            /* Each inactive, unrouted DAC must still keep output connected. */
            check_edge(pre_cgb_models[m], (channel + 1) % GB_N_CHANNELS, channel);
        }
    }
    printf("DAC shutdown edge: %u failed checks\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
