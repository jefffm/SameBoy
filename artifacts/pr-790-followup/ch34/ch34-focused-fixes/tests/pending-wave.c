/* Scratch boundary validation: manufacture a pending read, then verify the
   sample exposed by retrigger. This is compatibility evidence, not hardware. */
#define GB_INTERNAL
#include "Core/gb.h"
#include <assert.h>
#include <stdio.h>

static void tick(GB_gameboy_t *gb)
{
    gb->apu.apu_cycles = 1;
    GB_apu_run(gb, true);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    for (unsigned pending = 1; pending <= 2; pending++) {
        for (unsigned interrupted_by_trigger = 0; interrupted_by_trigger < 2; interrupted_by_trigger++) {
            GB_gameboy_t *gb = GB_alloc();
            GB_init(gb, GB_MODEL_DMG_B);
            assert(GB_load_rom(gb, argv[1]) == 0);
            GB_set_sample_rate(gb, 0);
            GB_apu_write(gb, GB_IO_NR52, 0);
            GB_apu_write(gb, GB_IO_NR52, 0x80);
            for (unsigned i = GB_IO_WAV_START; i <= GB_IO_WAV_END; i++) {
                GB_apu_write(gb, i, 0x55);
            }
            GB_apu_write(gb, GB_IO_NR32, 0x20);
            GB_apu_write(gb, GB_IO_NR30, 0x80);
            gb->apu.wave_channel.pulsed = true;
            gb->apu.wave_channel.current_sample_byte = 0x55;
            gb->apu.wave_channel.sample_countdown = pending - 1;
            tick(gb);
            assert(gb->apu.wave_channel.bugged_read_countdown == pending);
            if (interrupted_by_trigger) {
                GB_apu_write(gb, GB_IO_NR34, 0x80);
            }
            else {
                GB_apu_write(gb, GB_IO_NR30, 0);
            }
            for (unsigned i = 0; i < pending; i++) tick(gb);
            if (!interrupted_by_trigger) {
                GB_apu_write(gb, GB_IO_NR30, 0x80);
                GB_apu_write(gb, GB_IO_NR34, 0x80);
            }
            printf("pending=%u interrupted_by_trigger=%u exposed_sample=%u expected=5\n",
                   pending, interrupted_by_trigger, gb->apu.samples[GB_WAVE]);
            fflush(stdout);
            assert(gb->apu.samples[GB_WAVE] == 5);
            GB_dealloc(gb);
        }
    }
}
