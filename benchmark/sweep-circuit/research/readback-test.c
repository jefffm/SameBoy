/* Run a finite CPU probe and check its packed before/after active bits.
   The expected value comes from the physical acoustic NR52 readback. */
#define GB_INTERNAL
#include <Core/gb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool done;
static uint32_t pixels[160 * 144];

static uint32_t encode_rgb(GB_gameboy_t *gb, uint8_t r, uint8_t g, uint8_t b)
{
    (void)gb;
    return (uint32_t)r << 16 | (uint32_t)g << 8 | b;
}

static bool observe_write(GB_gameboy_t *gb, uint16_t address, uint8_t value)
{
    (void)gb;
    if (address == 0xff82 && value == 4) done = true;
    return true;
}

int main(int argc, char **argv)
{
    if (argc != 4) return 2;
    GB_gameboy_t *gb = GB_alloc();
    GB_init(gb, GB_MODEL_DMG_B);
    if (GB_load_rom(gb, argv[1]) || GB_load_boot_rom(gb, argv[2])) return 2;
    GB_set_pixels_output(gb, pixels);
    GB_set_rgb_encode_callback(gb, encode_rgb);
    GB_set_sample_rate(gb, 0);
    GB_set_write_memory_callback(gb, observe_write);
    while (!done) GB_run(gb);
    unsigned status = GB_read_memory(gb, 0xc004);
    unsigned expected = strtoul(argv[3], NULL, 0);
    unsigned active = status & 3;
    printf("%s: packed pre/post=%02x status=%u expected=%u\n",
           argv[1], status, active, expected);
    GB_dealloc(gb);
    return active != expected;
}
