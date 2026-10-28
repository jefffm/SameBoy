/* Compare equivalent constant-source DAC transitions with the unchanged CGB
   fade, then genuine NR50/NR51 source writes with the unchanged synthesis path.
   DAC control must share the DMG kernel's midpoint latency; source audio must
   not acquire another delay. No synthetic capacitor or mixer state is assigned. */
#define GB_INTERNAL
#include <Core/gb.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "pre-cgb-models.h"

enum { HARDWARE_RATE = 48000, PHASES = 5 };
static unsigned failures, callbacks;
static GB_sample_t last_sample;
static const unsigned dac_registers[] = {
    GB_IO_NR12, GB_IO_NR22, GB_IO_NR30, GB_IO_NR42,
};
static const char *phase_names[] = {
    "enable", "disable", "changed-source-enable", "NR50-source", "NR51-source",
};

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static void callback(GB_gameboy_t *gb, GB_sample_t *sample)
{
    (void) gb;
    callbacks++;
    last_sample = *sample;
}

static void next_sample(GB_gameboy_t *gb)
{
    unsigned initial = callbacks;
    while (callbacks == initial) {
        /* One APU tick is two DMG T-cycles. Advance the sample clock as the
           timing layer does, including at its one-sample-per-APU-tick rate. */
        gb->apu_output.sample_cycles += 2 * gb->apu_output.sample_rate;
        gb->apu.apu_cycles = 1;
        GB_apu_run(gb, true);
    }
    CHECK(callbacks == initial + 1);
}

static uint8_t enabled_value(unsigned channel)
{
    return channel == GB_WAVE ? 0x80 : 0x08;
}

static GB_gameboy_t *setup(GB_model_t model, unsigned channel, bool unfiltered)
{
    GB_gameboy_t *gb = GB_alloc();
    GB_init(gb, model);
    if (gb->sgb) gb->sgb->intro_animation = GB_SGB_INTRO_ANIMATION_LENGTH;
    /* Clock/2 is the core's unfiltered one-sample-per-APU-tick path. */
    GB_set_sample_rate(gb, unfiltered ? GB_get_clock_rate(gb) / 2 : HARDWARE_RATE);
    CHECK((gb->apu_output.max_cycles_per_sample == 1) == unfiltered);
    GB_set_highpass_filter_mode(gb, GB_HIGHPASS_OFF);
    GB_set_interference_volume(gb, 0);
    GB_apu_set_sample_callback(gb, callback);
    GB_apu_write(gb, GB_IO_NR52, 0);
    GB_apu_write(gb, GB_IO_NR52, 0x80);
    GB_apu_write(gb, GB_IO_NR50, 0x77);
    GB_apu_write(gb, GB_IO_NR51, 0x11 << channel);
    /* Keep another inactive, unrouted DAC enabled, excluding all-DAC-off
       capacitor holding and the special first-DAC reconnect behavior. */
    unsigned other = (channel + 1) % GB_N_CHANNELS;
    GB_apu_write(gb, dac_registers[other], enabled_value(other));
    CHECK(!gb->apu.is_active[other]);
    CHECK(!gb->apu.is_active[channel]);
    return gb;
}

static void record(GB_gameboy_t *gb, GB_sample_t *samples, unsigned count)
{
    for (unsigned i = 0; i < count; i++) {
        next_sample(gb);
        if (samples) samples[i] = last_sample;
    }
}

static void stimulus(GB_gameboy_t *gb, unsigned channel, unsigned phase,
                     unsigned settle)
{
    if (phase == 0 || phase == 2) {
        if (phase == 2) {
            /* The source really changes while its DAC is disabled. Allow the
               existing kernel to settle, then require the new level on return. */
            GB_apu_write(gb, GB_IO_NR50, 0);
            record(gb, NULL, settle);
        }
        GB_apu_write(gb, dac_registers[channel], enabled_value(channel));
    }
    else if (phase == 1) {
        GB_apu_write(gb, dac_registers[channel], 0);
    }
    else if (phase == 3) {
        GB_apu_write(gb, GB_IO_NR50, 0x77);
    }
    else {
        GB_apu_write(gb, GB_IO_NR51, 0);
    }
}

static void check(GB_model_t model, unsigned channel, bool unfiltered)
{
    GB_gameboy_t *dmg = setup(model, channel, unfiltered);
    GB_gameboy_t *cgb = setup(GB_MODEL_CGB_E, channel, unfiltered);
    /* SGB1 has a different oscillator. Match the reference's clock as well as
       its output rate before comparing the two synthesis phases exactly. */
    GB_set_clock_multiplier(cgb, (double) GB_get_clock_rate(dmg) / GB_get_clock_rate(cgb));
    GB_set_sample_rate(cgb, GB_get_sample_rate(dmg));
    unsigned rate = GB_get_sample_rate(dmg);
    /* Cover the whole existing kernel and DAC fade; no fitted timing window. */
    unsigned settle = GB_BAND_LIMITED_WIDTH + ceil((double) rate / DAC_ATTACK_SPEED);
    GB_sample_t *dmg_samples = calloc(settle, sizeof(*dmg_samples));
    GB_sample_t *cgb_samples = calloc(settle, sizeof(*cgb_samples));
    if (!dmg_samples || !cgb_samples) abort();
    record(dmg, NULL, settle);
    record(cgb, NULL, settle);
    GB_sample_t previous = last_sample;
    CHECK(previous.packed == 0);
    for (unsigned phase = 0; phase < PHASES; phase++) {
        stimulus(dmg, channel, phase, settle);
        record(dmg, dmg_samples, settle);
        stimulus(cgb, channel, phase, settle);
        record(cgb, cgb_samples, settle);
        unsigned latency = !unfiltered && phase < 3 ? GB_BAND_LIMITED_WIDTH / 2 : 0;
        unsigned mismatches = 0;
        for (unsigned i = 0; i < settle; i++) {
            GB_sample_t expected = i < latency ? previous : cgb_samples[i - latency];
            if (dmg_samples[i].packed != expected.packed) mismatches++;
        }
        printf("model=%x rate=%u channel=%u phase=%s expected_latency=%u mismatches=%u\n",
               model, rate, channel, phase_names[phase], latency, mismatches);
        CHECK(mismatches == 0);
        previous = cgb_samples[settle - 1];
        /* The high-volume and changed-volume levels come directly from NR50's
           eight gain steps and the inactive DAC's digital-zero source. */
        int expected_final = phase == 1 || phase == 4 ? 0 :
            0xf * CH_STEP * (phase == 2 ? 1 : 8);
        CHECK(previous.left == expected_final && previous.right == expected_final);
        CHECK(dmg_samples[settle - 1].packed == previous.packed);
        /* Preserve complete CGB PCM, rather than a tolerance or selected points,
           for an independent byte comparison against the pre-fix executable. */
        printf("CGB rate=%u channel=%u phase=%s PCM=", rate, channel, phase_names[phase]);
        for (unsigned i = 0; i < settle; i++) {
            printf("%s%d:%d", i ? "," : "", cgb_samples[i].left, cgb_samples[i].right);
        }
        puts("");
    }
    free(dmg_samples);
    free(cgb_samples);
    GB_dealloc(dmg);
    GB_dealloc(cgb);
}

static void check_dc_target(GB_model_t model, unsigned channel)
{
    GB_gameboy_t *gb = setup(model, channel, false);
    GB_gameboy_t *connected_control = setup(model, channel, false);
    GB_set_highpass_filter_mode(gb, GB_HIGHPASS_REMOVE_DC_OFFSET);
    GB_set_highpass_filter_mode(connected_control, GB_HIGHPASS_REMOVE_DC_OFFSET);
    GB_apu_write(gb, dac_registers[channel], enabled_value(channel));
    GB_apu_write(connected_control, dac_registers[channel], enabled_value(channel));
    unsigned settle = GB_BAND_LIMITED_WIDTH + ceil((double) HARDWARE_RATE / DAC_ATTACK_SPEED);
    record(gb, NULL, settle);
    record(connected_control, NULL, settle);
    CHECK(gb->apu_output.highpass_diff.left == connected_control->apu_output.highpass_diff.left);
    CHECK(gb->apu_output.highpass_diff.right == connected_control->apu_output.highpass_diff.right);
    GB_apu_write(gb, dac_registers[channel], 0);
    unsigned mismatches = 0;
    /* Before the write reaches the synthesis midpoint, neither its multiplier
       nor the DC-removal target may anticipate it. The continuously enabled
       twin supplies the expected samples and capacitor evolution directly. */
    for (unsigned i = 0; i < GB_BAND_LIMITED_WIDTH / 2; i++) {
        next_sample(gb);
        GB_sample_t delayed_sample = last_sample;
        next_sample(connected_control);
        if (delayed_sample.packed != last_sample.packed ||
            gb->apu_output.highpass_diff.left != connected_control->apu_output.highpass_diff.left ||
            gb->apu_output.highpass_diff.right != connected_control->apu_output.highpass_diff.right) {
            mismatches++;
        }
    }
    printf("DC target channel=%u premature_sample_or_capacitor_changes=%u\n", channel, mismatches);
    CHECK(mismatches == 0);
    GB_dealloc(gb);
    GB_dealloc(connected_control);
}

int main(void)
{
    for (unsigned m = 0; m < sizeof(pre_cgb_models) / sizeof(pre_cgb_models[0]); m++) {
        for (unsigned channel = 0; channel < GB_N_CHANNELS; channel++) {
            check(pre_cgb_models[m], channel, false);
            check(pre_cgb_models[m], channel, true);
            check_dc_target(pre_cgb_models[m], channel);
        }
    }
    printf("DAC synthesis timing: %u failed checks\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
