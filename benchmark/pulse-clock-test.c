/* A pulse's frequency gate survives length expiry on DMG-B. The recording's
   length/retrigger bursts expose the phase accumulated while NR52 is clear. */
#define GB_INTERNAL
#include <Core/gb.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    /* The prior 300a1209 APU section was 112 bytes, including tail padding. */
    const uint32_t old_apu_size = 112;
    assert(saved_apu_size == GB_SECTION_SIZE(apu));
    assert(offsetof(GB_apu_t, square_clock_active) == old_apu_size);
    size_t apu_start = prefix + sizeof(uint32_t);
    memset(state + apu_start + offsetof(GB_apu_t, noise_feedback_latch) + 1,
           0xff, old_apu_size - offsetof(GB_apu_t, noise_feedback_latch) - 1);
    memcpy(state + prefix, &old_apu_size, sizeof(old_apu_size));
    memmove(state + apu_start + old_apu_size, state + apu_start + saved_apu_size,
            size - apu_start - saved_apu_size);
    size -= saved_apu_size - old_apu_size;
    gb->apu.square_clock_active[0] = gb->apu.square_clock_active[1] = !active;
    assert(GB_load_state_from_buffer(gb, state, size) == 0);
    assert(!gb->apu.square_clock_active[0]);
    assert(gb->apu.square_clock_active[1] == active);
    printf("legacy112byte_APU active=%d: gate inferred from saved channel state\n", active);
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
}
