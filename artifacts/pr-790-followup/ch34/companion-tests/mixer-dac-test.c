/* NR50/NR51 rerender the existing DAC value. They must not put an invalidation
   sentinel in the retained four-bit sample when a DAC is disabled. */
#define GB_INTERNAL
#include <Core/gb.h>
#include <assert.h>
#include <stdio.h>

static void tick(GB_gameboy_t *gb)
{
    gb->apu.apu_cycles = 1;
    GB_apu_run(gb, true);
}

static void mixer_write(GB_gameboy_t *gb, unsigned reg, uint8_t value,
                        unsigned channel, uint8_t expected)
{
    GB_apu_write(gb, reg, value);
    printf("model=%x channel=%u reg=%02x value=%02x sample=%u expected=%u\n",
           gb->model, channel, reg, value, gb->apu.samples[channel], expected);
    fflush(stdout);
    assert(gb->apu.samples[channel] == expected);
}

static void check(GB_model_t model)
{
    GB_gameboy_t *gb = GB_alloc();
    GB_init(gb, model);
    GB_set_sample_rate(gb, 0);
    GB_apu_write(gb, GB_IO_NR52, 0);
    GB_apu_write(gb, GB_IO_NR52, 0x80);
    for (unsigned channel = 0; channel < GB_N_CHANNELS; channel++) {
        mixer_write(gb, GB_IO_NR50, 0x77, channel, 0);
        mixer_write(gb, GB_IO_NR51, 0xff, channel, 0);
    }
    const unsigned envelope[] = {GB_IO_NR12, GB_IO_NR22, GB_IO_NR30, GB_IO_NR42};
    for (unsigned channel = 0; channel < GB_N_CHANNELS; channel++) {
        GB_apu_write(gb, envelope[channel], channel == GB_WAVE ? 0x80 : 0xf0);
        mixer_write(gb, GB_IO_NR51, 0xff, channel, 0);
        assert(!gb->apu.is_active[channel]);
        if (channel <= GB_SQUARE_2) {
            GB_apu_write(gb, channel == GB_SQUARE_1 ? GB_IO_NR11 : GB_IO_NR21, 0x80);
            GB_apu_write(gb, channel == GB_SQUARE_1 ? GB_IO_NR13 : GB_IO_NR23, 0xff);
            GB_apu_write(gb, channel == GB_SQUARE_1 ? GB_IO_NR14 : GB_IO_NR24, 0x87);
        }
        else if (channel == GB_WAVE) {
            GB_apu_write(gb, GB_IO_NR30, 0);
            for (unsigned i = GB_IO_WAV_START; i <= GB_IO_WAV_END; i++) GB_apu_write(gb, i, 0xff);
            GB_apu_write(gb, GB_IO_NR32, 0x20);
            GB_apu_write(gb, GB_IO_NR33, 0xff);
            GB_apu_write(gb, GB_IO_NR30, 0x80);
            GB_apu_write(gb, GB_IO_NR34, 0x87);
        }
        else {
            GB_apu_write(gb, GB_IO_NR43, 0);
            GB_apu_write(gb, GB_IO_NR44, 0x80);
        }
        while (!gb->apu.samples[channel]) tick(gb);
        assert(gb->apu.samples[channel] == 15);
        mixer_write(gb, GB_IO_NR50, 0x77, channel, 15);
        mixer_write(gb, GB_IO_NR51, 0, channel, 15);
        GB_apu_write(gb, envelope[channel], 0);
        /* The AGB has digital mixing, so DAC-off updates its sample to zero. */
        unsigned expected = model > GB_MODEL_CGB_E ? 0 : 15;
        mixer_write(gb, GB_IO_NR50, 0, channel, expected);
        mixer_write(gb, GB_IO_NR51, 0xff, channel, expected);
    }
    GB_dealloc(gb);
}

int main(void)
{
    check(GB_MODEL_DMG_B);
    check(GB_MODEL_CGB_E);
    check(GB_MODEL_AGB);
    puts("Mixer writes preserve analog DAC samples and digital AGB behavior: passed");
}
