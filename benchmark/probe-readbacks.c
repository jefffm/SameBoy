/* Execute the exact physical probe ROM and read its completed CPU results.
   FF80/FF81 identify cases; FF82=3 exposes the readback, and FF82=4 ends the ROM. */
#include "Core/gb.h"
#include <stdio.h>
#include <stdlib.h>

static bool done;
static uint16_t result_size, result_data;

static void log_message(GB_gameboy_t *gb, const char *message, GB_log_attributes_t attributes)
{
    (void)gb;
    (void)attributes;
    fputs(message, stderr);
}

static uint32_t rgb(GB_gameboy_t *gb, uint8_t r, uint8_t g, uint8_t b)
{
    (void)gb;
    return (uint32_t)r << 16 | (uint32_t)g << 8 | b;
}

static bool observe_write(GB_gameboy_t *gb, uint16_t address, uint8_t value)
{
    if (address != 0xff82) return true;
    if (value == 4) done = true;
    if (value == 3) {
        unsigned id = GB_read_memory(gb, 0xff80) | GB_read_memory(gb, 0xff81) << 8;
        unsigned count = GB_read_memory(gb, result_size);
        printf("{\"id\":%u,\"payload\":[", id);
        for (unsigned i = 0; i < count; i++) {
            printf("%s%u", i ? "," : "", GB_read_memory(gb, result_data + i));
        }
        puts("]}");
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
    GB_set_log_callback(gb, log_message);
    uint32_t pixels[160 * 144];
    GB_set_pixels_output(gb, pixels);
    GB_set_rgb_encode_callback(gb, rgb);
    GB_set_border_mode(gb, GB_BORDER_NEVER);
    GB_set_sample_rate(gb, 0);
    GB_set_turbo_mode(gb, true, true);
    GB_set_turbo_cap(gb, 0);
    if (GB_load_boot_rom(gb, argv[2]) || GB_load_rom(gb, argv[1])) return 2;
    GB_set_write_memory_callback(gb, observe_write);
    while (!done) GB_run(gb);
    GB_free(gb);
    free(gb);
    return 0;
}
