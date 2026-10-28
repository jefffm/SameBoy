/* A pulse's frequency gate survives length expiry on DMG-B. The recording's
   length/retrigger bursts expose the phase accumulated while NR52 is clear. */
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
    /* Same clock fixture as dac-timing-test; no private pulse state is used
       to set up or assert the model-scope cases. */
    gb->apu_output.sample_cycles += 2 * GB_get_sample_rate(gb);
    gb->apu.apu_cycles = 1;
    GB_apu_run(gb, true);
    CHECK(callbacks == before + 1);
}

static unsigned retrigger_edge(GB_model_t model, unsigned channel, unsigned history)
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

    /* Synchronize to the 12.5% pulse's falling edge through PCM. With other
       channels muted, zero/fifteen are +4080/-4080, or 0/-8160 on AGB due
       to its digital envelope bias. No duty index or timer is inspected. */
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
    GB_write_memory(gb, length, 0x3f);
    GB_write_memory(gb, high, 0x46);
    for (unsigned i = 0; i < 4 && (GB_read_memory(gb, 0xff26) & (1 << channel)); i++) {
        GB_apu_div_event(gb);
    }
    CHECK(!(GB_read_memory(gb, 0xff26) & (1 << channel)));

    /* History 0 retriggers immediately; 1 waits six duty steps after length
       expiry; 2 disables the DAC during that wait, then enables it without
       triggering and waits again. Enabling the DAC must not restart clocks. */
    if (history == 2) {
        GB_write_memory(gb, envelope, 0);
    }
    if (history) {
        unsigned audible = 0;
        for (t = 0; t < 6 * 2048; t += 2) {
            next_sample(gb);
            /* Inactive AGB channels lose their envelope bias, so digital
               zero becomes +4080 on every model after length expiry. */
            if (history == 1 && (last_sample.left != 4080 || last_sample.right != 4080)) {
                audible++;
            }
        }
        CHECK(audible == 0);
    }
    if (history == 2) {
        GB_write_memory(gb, envelope, 0xf0);
        for (t = 0; t < 6 * 2048; t += 2) {
            next_sample(gb);
        }
    }
    CHECK(!(GB_read_memory(gb, 0xff26) & (1 << channel)));
    GB_write_memory(gb, high, 0x86);
    CHECK(GB_read_memory(gb, 0xff26) & (1 << channel));
    unsigned edge = 0;
    for (t = 2; t <= 9 * 2048; t += 2) {
        next_sample(gb);
        if (last_sample.left == zero - 8160 && last_sample.right == zero - 8160) {
            edge = t;
            break;
        }
    }
    CHECK(edge != 0);
    GB_dealloc(gb);
    return edge;
}

static void clock_model(GB_model_t model, unsigned channel)
{
    unsigned immediate = retrigger_edge(model, channel, 0);
    unsigned background = retrigger_edge(model, channel, 1);
    unsigned dac_stopped = retrigger_edge(model, channel, 2);
    /* Six elapsed duty steps must advance the audible edge by six periods,
       but only on pre-CGB and only while the frequency gate remains open. */
    unsigned advance = model < GB_MODEL_CGB_0 ? 6 * 2048 : 0;
    printf("model=%x CH%u retrigger_edge_T immediate=%u background=%u DAC_stop=%u expected_advance=%u\n",
           model, channel + 1, immediate, background, dac_stopped, advance);
    CHECK(immediate == background + advance);
    CHECK(immediate == dac_stopped);
}

static void ticks(GB_gameboy_t *gb, unsigned t_cycles)
{
    for (unsigned t = 0; t < t_cycles; t += 2) {
        gb->apu.apu_cycles = 1;
        GB_apu_run(gb, true);
    }
}

static void length_expiry(const char *rom, unsigned channel)
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
    GB_apu_write(gb, length, 0x3f);
    GB_apu_write(gb, envelope, 0xf0);
    GB_apu_write(gb, low, 0);
    ticks(gb, 2048);
    assert(gb->apu.square_channels[channel].current_sample_index == 0);
    GB_apu_write(gb, high, 0xc6);
    ticks(gb, 2048);
    while (gb->apu.is_active[channel]) GB_apu_div_event(gb);
    unsigned phase = gb->apu.square_channels[channel].current_sample_index;
    ticks(gb, 2048);
    printf("CH%u after length expiry: phase %u -> %u, expected %u, NR52=%02x\n",
           channel + 1, phase, gb->apu.square_channels[channel].current_sample_index,
           (phase + 1) & 7, GB_apu_read(gb, GB_IO_NR52));
    fflush(stdout);
    assert(gb->apu.square_channels[channel].current_sample_index == ((phase + 1) & 7));
    assert(!gb->apu.is_active[channel]);
    assert(gb->apu.samples[channel] == 0);
    GB_apu_write(gb, envelope, 0);
    phase = gb->apu.square_channels[channel].current_sample_index;
    ticks(gb, 2048);
    assert(gb->apu.square_channels[channel].current_sample_index == phase);
    GB_apu_write(gb, envelope, 0xf0);
    ticks(gb, 2048);
    assert(gb->apu.square_channels[channel].current_sample_index == phase);
    GB_apu_write(gb, high, 0x86);
    ticks(gb, 2048);
    assert(gb->apu.is_active[channel]);
    size_t size = GB_get_save_state_size(gb);
    uint8_t *state = malloc(size);
    assert(state);
    while (gb->apu.is_active[channel]) {
        /* Re-enable one length count, then let it expire before serializing. */
        GB_apu_write(gb, length, 0x3f);
        GB_apu_write(gb, high, 0x46);
        GB_apu_div_event(gb);
    }
    GB_save_state_to_buffer(gb, state);
    ticks(gb, 2048);
    phase = gb->apu.square_channels[channel].current_sample_index;
    GB_apu_write(gb, envelope, 0);
    assert(GB_load_state_from_buffer(gb, state, size) == 0);
    ticks(gb, 2048);
    assert(gb->apu.square_channels[channel].current_sample_index == phase);
    free(state);
    GB_apu_write(gb, GB_IO_NR52, 0);
    ticks(gb, 2048);
    assert(gb->apu.square_channels[channel].current_sample_index == 0);
    GB_dealloc(gb);
}

#ifdef PULSE_CLOCK_STATE
static void legacy_state(const char *rom, bool active)
{
    GB_gameboy_t *gb = GB_alloc();
    GB_init(gb, GB_MODEL_DMG_B);
    assert(GB_load_rom(gb, rom) == 0);
    GB_apu_write(gb, GB_IO_NR52, 0);
    GB_apu_write(gb, GB_IO_NR52, 0x80);
    GB_apu_write(gb, GB_IO_NR22, 0xf0);
    GB_apu_write(gb, GB_IO_NR24, 0x86);
    /* Advance past most of the initial trigger delay before saving. */
    ticks(gb, 2048);
    if (!active) GB_apu_write(gb, GB_IO_NR22, 0);
    size_t size = GB_get_save_state_size_no_bess(gb);
    uint8_t *state = malloc(size);
    assert(state);
    GB_save_state_to_buffer_no_bess(gb, state);
    const size_t prefix = GB_SECTION_SIZE(header) +
        GB_SECTION_SIZE(core_state) + sizeof(uint32_t) +
        GB_SECTION_SIZE(dma) + sizeof(uint32_t) +
        GB_SECTION_SIZE(mbc) + sizeof(uint32_t) +
        GB_SECTION_SIZE(hram) + sizeof(uint32_t) +
        GB_SECTION_SIZE(timing) + sizeof(uint32_t);
    uint32_t saved_apu_size;
    memcpy(&saved_apu_size, state + prefix, sizeof(saved_apu_size));
    /* The released APU section was 104 bytes, including tail padding. */
    const uint32_t old_apu_size = 104;
    assert(saved_apu_size == GB_SECTION_SIZE(apu));
    size_t apu_start = prefix + sizeof(uint32_t);
    state[apu_start + 102] = state[apu_start + 103] = 0;
    memcpy(state + prefix, &old_apu_size, sizeof(old_apu_size));
    memmove(state + apu_start + old_apu_size, state + apu_start + saved_apu_size,
            size - apu_start - saved_apu_size);
    size -= saved_apu_size - old_apu_size;
    gb->apu.square_clock_active[0] = gb->apu.square_clock_active[1] = false;
    assert(GB_load_state_from_buffer(gb, state, size) == 0);
    assert(!gb->apu.square_clock_active[0]);
    assert(gb->apu.square_clock_active[1] == active);
    if (active) {
        GB_apu_write(gb, GB_IO_NR21, 0x3f);
        GB_apu_write(gb, GB_IO_NR24, 0x46);
        while (gb->apu.is_active[GB_SQUARE_2]) GB_apu_div_event(gb);
        unsigned phase = gb->apu.square_channels[GB_SQUARE_2].current_sample_index;
        ticks(gb, 2048);
        assert(gb->apu.square_channels[GB_SQUARE_2].current_sample_index == ((phase + 1) & 7));
    }
    printf("legacy104byte_APU active=%d: gate repaired and continuation checked\n", active);
    free(state);
    GB_dealloc(gb);
}

#endif

int main(int argc, char **argv)
{
    assert(argc == 2);
    length_expiry(argv[1], 0);
    length_expiry(argv[1], 1);
#ifdef PULSE_CLOCK_STATE
    legacy_state(argv[1], false);
    legacy_state(argv[1], true);
#endif
    puts("DMG pulse frequency gate: passed");
    for (unsigned m = 0; m < sizeof(pre_cgb_models) / sizeof(pre_cgb_models[0]); m++) {
        for (unsigned channel = 0; channel < 2; channel++) {
            clock_model(pre_cgb_models[m], channel);
        }
    }
    const GB_model_t controls[] = {
        GB_MODEL_CGB_0, GB_MODEL_CGB_A, GB_MODEL_CGB_B, GB_MODEL_CGB_C,
        GB_MODEL_CGB_D, GB_MODEL_CGB_E, GB_MODEL_AGB, GB_MODEL_GBP,
    };
    for (unsigned m = 0; m < sizeof(controls) / sizeof(controls[0]); m++) {
        for (unsigned channel = 0; channel < 2; channel++) {
            clock_model(controls[m], channel);
        }
    }
    printf("Pulse clock model scope: %u failed checks\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
