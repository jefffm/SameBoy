#include "Core/gb.h"
#include <stdio.h>
#include <stdlib.h>

/* Run the real ROM instruction stream, reading its result only after it has
   finished the stimulus. WRAM addresses are supplied from the RGBDS symbols. */
static bool done;
static uint16_t result_size, result_data;
static uint32_t rgb(GB_gameboy_t *gb, uint8_t r, uint8_t g, uint8_t b)
{
    (void)gb;
    return (uint32_t)r << 16 | (uint32_t)g << 8 | b;
}
static bool write_callback(GB_gameboy_t *gb, uint16_t address, uint8_t value)
{
    if (address == 0xff82 && value == 3) {
        unsigned count = GB_read_memory(gb, result_size);
        putchar('[');
        for (unsigned i = 0; i < count; i++) {
            printf("%s%u", i ? "," : "", GB_read_memory(gb, result_data + i));
        }
        puts("]");
        done = true;
    }
    return true;
}

int main(int argc, char **argv)
{
    if (argc != 5) return 2;
    result_size = strtoul(argv[3], NULL, 16);
    result_data = strtoul(argv[4], NULL, 16);
    GB_random_seed(1);
    GB_gameboy_t *gb = GB_alloc();
    if (!gb) return 2;
    GB_init(gb, GB_MODEL_DMG_B);
    uint32_t pixels[160 * 144];
    GB_set_pixels_output(gb, pixels);
    GB_set_rgb_encode_callback(gb, rgb);
    GB_set_border_mode(gb, GB_BORDER_NEVER);
    if (GB_load_boot_rom(gb, argv[2]) || GB_load_rom(gb, argv[1])) return 2;
    GB_set_turbo_mode(gb, true, true);
    GB_set_turbo_cap(gb, 0);
    GB_set_write_memory_callback(gb, write_callback);
    while (!done) GB_run(gb);
    GB_free(gb);
    free(gb);
    return 0;
}
