/* DUWO/DOME retain the duty bit across an active pulse retrigger. */
#define GB_INTERNAL
#include <Core/gb.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
       must keep that distinction; old states reconstruct their combinational bit. */
    size_t size = GB_get_save_state_size_no_bess(gb);
    uint8_t *state = malloc(size);
    assert(state);
    GB_save_state_to_buffer_no_bess(gb, state);
    gb->apu.square_duty_output[channel] = true;
    assert(GB_load_state_from_buffer(gb, state, size) == 0);
    assert(!gb->apu.square_duty_output[channel]);
    const size_t prefix = GB_SECTION_SIZE(header) +
        GB_SECTION_SIZE(core_state) + sizeof(uint32_t) +
        GB_SECTION_SIZE(dma) + sizeof(uint32_t) +
        GB_SECTION_SIZE(mbc) + sizeof(uint32_t) +
        GB_SECTION_SIZE(hram) + sizeof(uint32_t) +
        GB_SECTION_SIZE(timing) + sizeof(uint32_t);
    uint32_t saved_apu_size;
    memcpy(&saved_apu_size, state + prefix, sizeof(saved_apu_size));
    /* 89933a9d added clock gates, making its serialized APU section 120 bytes. */
    const uint32_t old_apu_size = 120;
    assert(offsetof(GB_apu_t, square_duty_output) == old_apu_size);
    size_t apu_start = prefix + sizeof(uint32_t);
    memset(state + apu_start + offsetof(GB_apu_t, square_clock_active) + 2,
           0xff, old_apu_size - offsetof(GB_apu_t, square_clock_active) - 2);
    memcpy(state + prefix, &old_apu_size, sizeof(old_apu_size));
    memmove(state + apu_start + old_apu_size, state + apu_start + saved_apu_size,
            size - apu_start - saved_apu_size);
    size -= saved_apu_size - old_apu_size;
    assert(GB_load_state_from_buffer(gb, state, size) == 0);
    assert(gb->apu.square_duty_output[channel]);
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
    puts("duty latch: new-state roundtrip, legacy120 reconstruction and zero-volume retention passed");
    GB_dealloc(gb);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    duty_latch(argv[1], 0);
    duty_latch(argv[1], 1);
    puts("DMG pulse duty output latch: passed");
}
