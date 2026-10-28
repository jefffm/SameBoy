/* DUWO/DOME retain the duty bit across an active pulse retrigger. */
#define GB_INTERNAL
#include <Core/gb.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pre-cgb-models.h"

static unsigned failures, callbacks;
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
    /* Use the same one-sample-per-APU-tick fixture as dac-timing-test. Only
       PCM callbacks and register reads are assertions in the model test. */
    gb->apu_output.sample_cycles += 2 * GB_get_sample_rate(gb);
    gb->apu.apu_cycles = 1;
    GB_apu_run(gb, true);
    CHECK(callbacks == before + 1);
}

static void duty_model(GB_model_t model, unsigned channel)
{
    GB_gameboy_t *gb = GB_alloc();
    GB_init(gb, model);
    if (gb->sgb) {
        gb->sgb->intro_animation = GB_SGB_INTRO_ANIMATION_LENGTH;
    }
    GB_set_sample_rate(gb, GB_get_clock_rate(gb) / 2);
    GB_set_highpass_filter_mode(gb, GB_HIGHPASS_OFF);
    GB_set_interference_volume(gb, 0);
    GB_apu_set_sample_callback(gb, sample_callback);
    for (unsigned i = 0; i < GB_N_CHANNELS; i++) {
        GB_set_channel_muted(gb, i, i != channel);
    }
    unsigned length = channel ? 0xff16 : 0xff11;
    unsigned envelope = channel ? 0xff17 : 0xff12;
    unsigned low = channel ? 0xff18 : 0xff13;
    unsigned high = channel ? 0xff19 : 0xff14;
    GB_write_memory(gb, 0xff26, 0);
    GB_write_memory(gb, 0xff26, 0x80);
    GB_write_memory(gb, 0xff24, 0x77);
    GB_write_memory(gb, 0xff25, 0x11 << channel);
    GB_write_memory(gb, length, 0);
    GB_write_memory(gb, envelope, 0xf0);
    GB_write_memory(gb, low, 0);
    GB_write_memory(gb, high, 0x86);

    /* The 12.5% pulse has one high step followed by seven low steps. Find
       its falling edge through PCM, not the private duty index/latch. At
       full NR50 gain, digital zero is +4080 and fifteen is -4080. AGB's
       digital envelope bias subtracts a further 4080 from both levels. */
    signed zero = model > GB_MODEL_CGB_E ? 0 : 4080;
    unsigned t;
    for (t = 0; t < 9 * 2048; t += 2) {
        next_sample(gb);
        if (last_sample.left == zero - 8160) break;
    }
    CHECK(t < 9 * 2048);
    for (t = 0; t < 2048 + 4; t += 2) {
        next_sample(gb);
        if (last_sample.left == zero) break;
    }
    CHECK(t < 2048 + 4);
    CHECK(GB_read_memory(gb, 0xff26) & (1 << channel));
    GB_write_memory(gb, length, 0x40);
    GB_write_memory(gb, high, 0x86);
    /* Duty 25% is high at this phase, but pre-CGB retains the previous low
       bit until the next frequency clock. CGB/AGB retain their old immediate
       recomputation. The entire 512T window precedes the 2048T clock. */
    signed expected = model < GB_MODEL_CGB_0 ? zero : zero - 8160;
    unsigned mismatches = 0;
    for (t = 0; t < 512; t += 2) {
        next_sample(gb);
        if (last_sample.left != expected || last_sample.right != expected) {
            mismatches++;
        }
    }
    printf("model=%x CH%u duty0->1 retrigger PCM=%d:%d expected=%d mismatches=%u\n",
           model, channel + 1, last_sample.left, last_sample.right, expected, mismatches);
    CHECK(mismatches == 0);
    GB_dealloc(gb);
}

static void tick(GB_gameboy_t *gb)
{
    gb->apu.apu_cycles = 1;
    GB_apu_run(gb, true);
}

static void duty_latch(const char *rom, unsigned channel)
{
    GB_gameboy_t *gb = GB_alloc();
    GB_init(gb, GB_MODEL_DMG_B);
    assert(GB_load_rom(gb, rom) == 0);
    GB_set_sample_rate(gb, 0);
    GB_apu_write(gb, GB_IO_NR52, 0);
    GB_apu_write(gb, GB_IO_NR52, 0x80);
    unsigned length = channel ? GB_IO_NR21 : GB_IO_NR11;
    unsigned envelope = channel ? GB_IO_NR22 : GB_IO_NR12;
    unsigned low = channel ? GB_IO_NR23 : GB_IO_NR13;
    unsigned high = channel ? GB_IO_NR24 : GB_IO_NR14;
    GB_apu_write(gb, length, 0);
    GB_apu_write(gb, envelope, 0xf0);
    GB_apu_write(gb, low, 0);
    GB_apu_write(gb, high, 0x86);
    while (gb->apu.square_channels[channel].current_sample_index != 7) tick(gb);
    while (gb->apu.square_channels[channel].current_sample_index != 0) tick(gb);
    assert(gb->apu.samples[channel] == 0);
    GB_apu_write(gb, length, 0x40);
    GB_apu_write(gb, high, 0x86);
    printf("CH%u duty0->1 retrigger: output=%d, expected retained 0\n", channel+1, gb->apu.samples[channel]);
    fflush(stdout);
    assert(gb->apu.samples[channel] == 0);
#ifdef PULSE_DUTY_STATE
    /* The live duty register now differs from the latched output. New states
       must keep that distinction; exact legacy reconstruction is retired. */
    size_t size = GB_get_save_state_size_no_bess(gb);
    uint8_t *state = malloc(size);
    assert(state);
    GB_save_state_to_buffer_no_bess(gb, state);
    gb->apu.square_duty_output[channel] = true;
    assert(GB_load_state_from_buffer(gb, state, size) == 0);
    assert(!gb->apu.square_duty_output[channel]);
    free(state);
#endif
    GB_apu_write(gb, length, 0x80);
    /* Same phase, but now the next timer clock latches duty 2's high bit. */
    while (gb->apu.square_channels[channel].current_sample_index != 7) tick(gb);
    assert(gb->apu.samples[channel] == 15);
    while (gb->apu.square_channels[channel].current_sample_index != 0) tick(gb);
    GB_apu_write(gb, length, 0xc0);
    GB_apu_write(gb, high, 0x86);
    assert(gb->apu.samples[channel] == 15);
    while (gb->apu.square_channels[channel].current_sample_index != 1) tick(gb);
    assert(gb->apu.samples[channel] == 15);
    /* An output bit of one must survive volume zero: the latch cannot be
       reconstructed from whether the current numeric sample is nonzero. */
    GB_apu_write(gb, envelope, 0x08);
    GB_apu_write(gb, high, 0x86);
    assert(gb->apu.square_channels[channel].current_volume == 0);
    assert(gb->apu.samples[channel] == 0);
    GB_apu_write(gb, length, 0);
    GB_apu_write(gb, envelope, 0xf0);
    GB_apu_write(gb, high, 0x86);
    assert(gb->apu.samples[channel] == 15);
    puts("duty latch: new-state roundtrip and zero-volume retention passed");
    GB_dealloc(gb);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    duty_latch(argv[1], 0);
    duty_latch(argv[1], 1);
    puts("DMG pulse duty output latch: passed");
    for (unsigned m = 0; m < sizeof(pre_cgb_models) / sizeof(pre_cgb_models[0]); m++) {
        for (unsigned channel = 0; channel < 2; channel++) {
            duty_model(pre_cgb_models[m], channel);
        }
    }
    const GB_model_t controls[] = {
        GB_MODEL_CGB_0, GB_MODEL_CGB_A, GB_MODEL_CGB_B, GB_MODEL_CGB_C,
        GB_MODEL_CGB_D, GB_MODEL_CGB_E, GB_MODEL_AGB, GB_MODEL_GBP,
    };
    for (unsigned m = 0; m < sizeof(controls) / sizeof(controls[0]); m++) {
        for (unsigned channel = 0; channel < 2; channel++) {
            duty_model(controls[m], channel);
        }
    }
    printf("Pulse duty model scope: %u failed checks\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
