/* Local validation harness. Public CPU callbacks, pixels, and memory access.
   Optional injected NR52 bits test the ROM reporter, not physical hardware. */
#include "Core/gb.h"
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t ticks, reset_tick, trigger_tick, pre_tick, measure_start;
static unsigned shift, frequency, read_index, trials, pre, sram_reads, sram_writes;
static bool armed, measuring, done;
static uint8_t injected[11587 * 2];
static bool inject;
static FILE *trace;
static uint32_t pixels[160 * 144];

static uint32_t rgb(GB_gameboy_t *gb, uint8_t r, uint8_t g, uint8_t b)
{
    return (uint32_t)r << 16 | (uint32_t)g << 8 | b;
}

static bool write_memory(GB_gameboy_t *gb, uint16_t address, uint8_t value)
{
    if (address >= 0xa000 && address < 0xc000) sram_writes++;
    if (address == 0xff82) {
        measuring = value == 2;
        if (value == 2 && !measure_start) measure_start = ticks;
        if (value == 4) done = true;
    }
    if (!measuring) return true;
    if (address == 0xff04) reset_tick = ticks;
    if (address == 0xff26 && !value) armed = false;
    if (address == 0xff10) shift = value & 7;
    if (address == 0xff13) frequency = value;
    if (address == 0xff14 && (value & 0x80)) {
        frequency |= (value & 7) << 8;
        trigger_tick = ticks;
        read_index = 0;
        armed = true;
        assert(shift >= 1 && shift <= 7);
    }
    return true;
}

static uint8_t read_memory(GB_gameboy_t *gb, uint16_t address, uint8_t value)
{
    if (address >= 0xa000 && address < 0xc000) sram_reads++;
    if (address != 0xff26 || !measuring || !armed) return value;
    assert(trials < 11587 && read_index < 2);
    size_t size;
    uint8_t *io = GB_get_direct_access(gb, GB_DIRECT_ACCESS_IO, &size, NULL);
    assert(!(io[0x40] & 0x80));
    if (inject) value = (value & 0xfe) | injected[2 * trials + read_index];
    if (!read_index++) {
        pre = value & 1;
        pre_tick = ticks;
    }
    else {
        fprintf(trace, "%u,%u,%u,%u,%u,%" PRIu64 ",%" PRIu64 ",%" PRIu64 "\n",
                trials, shift, frequency, pre, value & 1,
                (trigger_tick - reset_tick) / 2, (pre_tick - trigger_tick) / 2,
                (ticks - pre_tick) / 2);
        trials++;
        armed = false;
    }
    return value;
}

static void run_for(GB_gameboy_t *gb, double seconds)
{
    uint64_t end = ticks + seconds * GB_get_clock_rate(gb) * 2;
    while (ticks < end) ticks += GB_run(gb);
}

static void press(GB_gameboy_t *gb, GB_key_t key)
{
    GB_set_key_state(gb, key, true);
    run_for(gb, .15);
    GB_set_key_state(gb, key, false);
    /* Scanning 2018 retained results can take about half a second. Do not
       issue another key or capture the old page before that redraw finishes. */
    run_for(gb, 1);
}

static void snapshot(GB_gameboy_t *gb, const char *directory, const char *name)
{
    char path[4096];
    snprintf(path, sizeof(path), "%s/%s.ppm", directory, name);
    FILE *file = fopen(path, "wb");
    assert(file);
    fputs("P6\n160 144\n255\n", file);
    for (unsigned i = 0; i < 160 * 144; i++) {
        uint8_t color[] = {pixels[i] >> 16, pixels[i] >> 8, pixels[i]};
        assert(fwrite(color, 1, 3, file) == 3);
    }
    assert(!fclose(file));
    snprintf(path, sizeof(path), "%s/%s.txt", directory, name);
    file = fopen(path, "w");
    assert(file);
    uint8_t *vram = GB_get_direct_access(gb, GB_DIRECT_ACCESS_VRAM, NULL, NULL);
    for (unsigned y = 0; y < 18; y++) {
        for (unsigned x = 0; x < 20; x++) fputc(vram[0x1800 + y * 32 + x] + 32, file);
        fputc('\n', file);
    }
    assert(!fclose(file));
}

int main(int argc, char **argv)
{
    assert(argc == 7); /* ROM, boot, output directory, model, injection or -, UI 0/1 */
    GB_random_seed(1);
    GB_gameboy_t *gb = GB_alloc();
    GB_init(gb, strtoul(argv[4], NULL, 0));
    GB_set_border_mode(gb, GB_BORDER_NEVER);
    GB_set_pixels_output(gb, pixels);
    GB_set_rgb_encode_callback(gb, rgb);
    GB_set_emulate_joypad_bouncing(gb, false);
    GB_set_sample_rate(gb, 0);
    assert(!GB_load_rom(gb, argv[1]));
    assert(!GB_load_boot_rom(gb, argv[2]));
    if (strcmp(argv[5], "-")) {
        FILE *file = fopen(argv[5], "rb");
        assert(file && fread(injected, 1, sizeof(injected), file) == sizeof(injected));
        assert(fgetc(file) == EOF);
        fclose(file);
        inject = true;
    }
    char path[4096];
    snprintf(path, sizeof(path), "%s/trials.csv", argv[3]);
    trace = fopen(path, "w");
    assert(trace);
    fputs("trial,shift,frequency,pre,post,div_to_trigger_T,trigger_to_pre_T,pre_to_post_T\n", trace);
    GB_set_write_memory_callback(gb, write_memory);
    GB_set_read_memory_callback(gb, read_memory);
    bool ui = atoi(argv[6]);
    if (ui) {
        run_for(gb, 6);
        snapshot(gb, argv[3], "intro");
        press(gb, GB_KEY_START);
    }
    uint64_t deadline = (uint64_t)GB_get_clock_rate(gb) * 2 * 180;
    while (!done && ticks < deadline) ticks += GB_run(gb);
    assert(done && trials == 11587 && !sram_reads && !sram_writes);
    assert(!fclose(trace));
    printf("model=%s trials=%u measurement_and_reporting_s=%.6f SRAM=%u/%u injected=%u\n",
           argv[4], trials, (double)(ticks - measure_start) / (GB_get_clock_rate(gb) * 2),
           sram_reads, sram_writes, inject);
    if (ui) {
        run_for(gb, .1);
        snapshot(gb, argv[3], "summary");
        snprintf(path, sizeof(path), "%s/ram.bin", argv[3]);
        FILE *file = fopen(path, "wb");
        assert(file);
        size_t size;
        uint8_t *ram = GB_get_direct_access(gb, GB_DIRECT_ACCESS_RAM, &size, NULL);
        assert(fwrite(ram, 1, size, file) == size);
        fclose(file);
        press(gb, GB_KEY_RIGHT);
        snapshot(gb, argv[3], "detail-first");
        press(gb, GB_KEY_UP);
        snapshot(gb, argv[3], "detail-last");
        press(gb, GB_KEY_DOWN);
        snapshot(gb, argv[3], "detail-wrap");
        press(gb, GB_KEY_DOWN);
        snapshot(gb, argv[3], "detail-next");
        press(gb, GB_KEY_RIGHT);
        snapshot(gb, argv[3], "detail-shift2");
        press(gb, GB_KEY_START);
        snapshot(gb, argv[3], "summary-return");
        press(gb, GB_KEY_LEFT);
        snapshot(gb, argv[3], "detail-shift7");
        press(gb, GB_KEY_RIGHT);
        snapshot(gb, argv[3], "summary-wrap");
    }
    GB_dealloc(gb);
}
