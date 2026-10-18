/* A DMG reconnect must restore the DAC's held source before subtracting the
   coupling capacitor. A changed source must retain its genuine reconnect step.
   Drive real register writes and sample callbacks; never manufacture a capacitor. */
#define GB_INTERNAL
#include <Core/gb.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

enum { SAMPLE_RATE = 48000 }; /* Rate of the physical NOIZE JAMS recording. */
static unsigned callbacks, failures;
static GB_sample_t last_sample;

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

static unsigned settle_samples(void)
{
    /* Flush the entire synthesis kernel and finish the documented DAC fade. */
    return GB_BAND_LIMITED_WIDTH + ceil((double) SAMPLE_RATE / DAC_DECAY_SPEED);
}

static GB_gameboy_t *setup(GB_model_t model, unsigned channel, int other)
{
    GB_gameboy_t *gb = GB_alloc();
    GB_init(gb, model);
    GB_set_sample_rate(gb, SAMPLE_RATE);
    GB_set_highpass_filter_mode(gb, GB_HIGHPASS_ACCURATE);
    GB_set_interference_volume(gb, 0);
    GB_apu_set_sample_callback(gb, sample_callback);
    GB_apu_write(gb, GB_IO_NR52, 0);
    GB_apu_write(gb, GB_IO_NR52, 0x80);
    GB_apu_write(gb, GB_IO_NR50, 0x77);
    GB_apu_write(gb, GB_IO_NR51, 0x11 << channel);
    if (other >= 0) {
        /* Connect the unrouted control DAC first. Its zero mixer contribution
           makes this initial connection independent of the changed attack. */
        GB_apu_write(gb, dac_registers[other], enabled_value(other));
        CHECK(!gb->apu.is_active[other]);
    }
    GB_apu_write(gb, dac_registers[channel], enabled_value(channel));
    /* Untriggered DACs expose a constant digital-zero source. This keeps genuine
       oscillator/BLEP transitions out of the reconnect being tested. */
    CHECK(!gb->apu.is_active[channel]);
    for (unsigned i = 0; i < settle_samples(); i++) {
        next_sample(gb);
    }
    CHECK(gb->apu_output.dac_discharge[channel] == 1);
    CHECK(gb->apu_output.highpass_diff.left != 0);
    CHECK(gb->apu_output.highpass_diff.right != 0);
    return gb;
}

enum change { UNCHANGED, CHANGE_VOLUME, CHANGE_ROUTING };

static void check_reconnect(GB_model_t model, unsigned channel, int other,
                            enum change change)
{
    GB_gameboy_t *gb = setup(model, channel, other);
    bool disconnected = model == GB_MODEL_DMG_B && other < 0;
    GB_double_sample_t held = gb->apu_output.highpass_diff;
    GB_apu_write(gb, dac_registers[channel], 0);
    if (change == CHANGE_VOLUME) {
        GB_apu_write(gb, GB_IO_NR50, 0);
    }
    if (change == CHANGE_ROUTING) {
        GB_apu_write(gb, GB_IO_NR51, 0);
    }
    for (unsigned i = 0; i < settle_samples(); i++) {
        next_sample(gb);
        if (disconnected) {
            CHECK(last_sample.left == 0 && last_sample.right == 0);
            CHECK(gb->apu_output.highpass_diff.left == held.left);
            CHECK(gb->apu_output.highpass_diff.right == held.right);
        }
    }
    CHECK(gb->apu_output.dac_discharge[channel] == 0);
    GB_band_limited_t *source = &gb->apu_output.band_limited[channel];
    CHECK(source->output.left == source->input.left * GB_BAND_LIMITED_ONE);
    CHECK(source->output.right == source->input.right * GB_BAND_LIMITED_ONE);
    GB_sample_t raw = {
        .left = source->input.left * CH_STEP,
        .right = source->input.right * CH_STEP,
    };
    held = gb->apu_output.highpass_diff;
    GB_sample_t expected = {
        .left = raw.left - (int16_t) held.left,
        .right = raw.right - (int16_t) held.right,
    };
    /* An intentionally changed disconnected source must not be hidden by
       resetting the capacitor or muting the first reconnected samples. */
    if (disconnected && change != UNCHANGED) {
        CHECK(expected.left != 0 && expected.right != 0);
    }
    GB_apu_write(gb, dac_registers[channel], enabled_value(channel));
    CHECK(gb->apu_output.highpass_diff.left == held.left);
    CHECK(gb->apu_output.highpass_diff.right == held.right);
    next_sample(gb);
    printf("%s model=%x channel=%u other=%d change=%u raw=%d,%d cap=%a,%a "
           "output=%d,%d fraction=%a cap_after=%a,%a\n",
           disconnected ? "reconnect" : "control", model, channel, other, change,
           raw.left, raw.right, held.left, held.right, last_sample.left,
           last_sample.right, gb->apu_output.dac_discharge[channel],
           gb->apu_output.highpass_diff.left, gb->apu_output.highpass_diff.right);
    if (disconnected) {
        CHECK(last_sample.left == expected.left);
        CHECK(last_sample.right == expected.right);
        CHECK(gb->apu_output.dac_discharge[channel] == 1);
        CHECK(gb->apu_output.highpass_diff.left ==
              raw.left - (raw.left - held.left) * gb->apu_output.highpass_rate);
        CHECK(gb->apu_output.highpass_diff.right ==
              raw.right - (raw.right - held.right) * gb->apu_output.highpass_rate);
    }
    else {
        /* Existing fades must remain for CGB and when another DAC keeps the
           DMG output connected, even if that DAC is inactive and unrouted. */
        CHECK(gb->apu_output.dac_discharge[channel] ==
              (double) DAC_ATTACK_SPEED / SAMPLE_RATE);
    }
    GB_dealloc(gb);
}

int main(void)
{
    for (unsigned channel = 0; channel < GB_N_CHANNELS; channel++) {
        for (enum change change = UNCHANGED; change <= CHANGE_ROUTING; change++) {
            check_reconnect(GB_MODEL_DMG_B, channel, -1, change);
            check_reconnect(GB_MODEL_CGB_E, channel, -1, change);
            check_reconnect(GB_MODEL_DMG_B, channel,
                            (channel + 1) % GB_N_CHANNELS, change);
        }
    }
    printf("DAC reconnect: %u failed checks\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
