#define GB_INTERNAL
#include <Core/gb.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned callbacks;
static GB_sample_t last;
static void callback(GB_gameboy_t *gb, GB_sample_t *sample)
{
    (void) gb;
    callbacks++;
    last = *sample;
}
static void next_sample(GB_gameboy_t *gb)
{
    unsigned initial = callbacks;
    while (callbacks == initial) {
        GB_advance_cycles(gb, 4);
        GB_apu_run(gb, true);
    }
}
int main(void)
{
    puts("path,sample,output_left,dac_fraction,band_input,band_output_fixed16");
    for (unsigned path = 0; path < 2; path++) {
        GB_gameboy_t *gb = GB_alloc();
        GB_init(gb, GB_MODEL_DMG_B);
        GB_set_sample_rate(gb, 48000);
        GB_set_highpass_filter_mode(gb, GB_HIGHPASS_OFF);
        GB_set_interference_volume(gb, 0);
        GB_apu_set_sample_callback(gb, callback);
        GB_apu_write(gb, GB_IO_NR52, 0);
        GB_apu_write(gb, GB_IO_NR52, 0x80);
        GB_apu_write(gb, GB_IO_NR50, 0x77);
        GB_apu_write(gb, GB_IO_NR51, 0);
        /* Keep CH2 enabled, inactive and unrouted so neither stimulus touches
           the all-DAC-off output-disconnect path or capacitor holding. */
        GB_apu_write(gb, GB_IO_NR22, 0x08);
        if (path == 0) GB_apu_write(gb, GB_IO_NR12, 0x08);
        else GB_apu_write(gb, GB_IO_NR51, 0x11);
        unsigned settle = GB_BAND_LIMITED_WIDTH + ceil(48000.0 / DAC_ATTACK_SPEED);
        for (unsigned i = 0; i < settle; i++) next_sample(gb);
        if (path == 0) GB_apu_write(gb, GB_IO_NR51, 0x11);
        else GB_apu_write(gb, GB_IO_NR12, 0x08);
        for (unsigned i = 0; i < settle; i++) {
            next_sample(gb);
            printf("%s,%u,%d,%.17g,%d,%d\n", path == 0 ? "NR51-routing" : "NR12-enable",
                   i, last.left, gb->apu_output.dac_discharge[GB_SQUARE_1],
                   gb->apu_output.band_limited[GB_SQUARE_1].input.left,
                   gb->apu_output.band_limited[GB_SQUARE_1].output.left);
        }
        GB_dealloc(gb);
    }
}
