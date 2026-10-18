#define _POSIX_C_SOURCE 200809L
#define GB_INTERNAL
#include "Core/gb.h"
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Only public Core APIs are used. GB_run returns 8 MHz ticks. Input and trace
   timestamps refer to GB_run boundaries, not exact sub-instruction write cycles. */
typedef struct {
    uint64_t tick;
    GB_key_t key;
    bool pressed;
} button_event_t;

typedef struct {
    uint64_t ticks, samples, writes, marker_events;
    uint32_t clock_rate;
    uint16_t case_id;
    bool suite_done;
    FILE *trace, *markers;
} capture_t;

static void usage(const char *program)
{
    fprintf(stderr,
            "Usage: %s --rom FILE [--sav FILE] --boot FILE (--seconds N | --until-done) --seed N\n"
            "       --wav FILE --screenshot FILE [--buttons FILE] [--apu-log FILE]\n"
            "       [--markers-log FILE] [--highpass accurate|off|dc]\n"
            "Model: DMG-B. Audio: stereo 48000 Hz, recorded from boot.\n"
            "Buttons: sorted lines 'seconds key down|up', with optional # comments.\n"
            "Keys: right left up down a b select start. Times are from boot.\n"
            "Input changes apply at the first instruction boundary at/after time.\n"
            "Markers: FF80/FF81 hold the low/high case ID; FF82 writes emit events.\n"
            "Events: 1 marker start, 2 stimulus start, 3 stimulus end, 4 suite done.\n"
            "Marker timestamps, like APU logs, are GB_run boundaries, not exact bus cycles.\n"
            "--until-done stops after event 4, with no time limit.\n"
            "Highpass defaults to accurate; emulated interference is disabled.\n"
            "SAV is read only: no battery-save operation is performed.\n",
            program);
}

static bool parse_seconds(const char *text, double *value)
{
    char *end;
    errno = 0;
    *value = strtod(text, &end);
    return !errno && end != text && !*end && isfinite(*value) && *value >= 0;
}

static bool seconds_to_ticks(double seconds, uint32_t clock_rate, uint64_t *ticks)
{
    long double value = ceill((long double)seconds * clock_rate * 2);
    if (value >= (long double)UINT64_MAX) return false;
    *ticks = (uint64_t)value;
    return true;
}

static bool read_buttons(const char *path, uint32_t clock_rate,
                         button_event_t **events, size_t *count)
{
    static const char *names[] = {"right", "left", "up", "down", "a", "b", "select", "start"};
    FILE *file = fopen(path, "r");
    if (!file) {
        perror(path);
        return false;
    }
    char *line = NULL;
    size_t capacity = 0, line_number = 0;
    bool success = true;
    while (getline(&line, &capacity, file) >= 0) {
        line_number++;
        char *comment = strchr(line, '#');
        if (comment) *comment = 0;
        char *save;
        char *time = strtok_r(line, " \t\r\n", &save);
        if (!time) continue;
        char *key = strtok_r(NULL, " \t\r\n", &save);
        char *state = strtok_r(NULL, " \t\r\n", &save);
        double seconds;
        button_event_t event;
        if (!key || !state || strtok_r(NULL, " \t\r\n", &save) ||
            !parse_seconds(time, &seconds) || !seconds_to_ticks(seconds, clock_rate, &event.tick) ||
            (*count && event.tick < (*events)[*count - 1].tick)) {
            success = false;
            break;
        }
        unsigned index;
        for (index = 0; index < GB_KEY_MAX; index++) {
            if (!strcmp(key, names[index])) break;
        }
        if (index == GB_KEY_MAX || (strcmp(state, "down") && strcmp(state, "up"))) {
            success = false;
            break;
        }
        event.key = index;
        event.pressed = !strcmp(state, "down");
        if (*count == SIZE_MAX / sizeof(**events)) {
            errno = ENOMEM;
            success = false;
            break;
        }
        button_event_t *resized = realloc(*events, (*count + 1) * sizeof(**events));
        if (!resized) {
            success = false;
            break;
        }
        *events = resized;
        (*events)[(*count)++] = event;
    }
    if (ferror(file)) success = false;
    if (!success) fprintf(stderr, "%s:%zu: invalid or unreadable button event\n", path, line_number);
    free(line);
    if (fclose(file)) success = false;
    return success;
}

static uint32_t encode_rgb(GB_gameboy_t *gb, uint8_t red, uint8_t green, uint8_t blue)
{
    (void)gb;
    return (uint32_t)red << 16 | (uint32_t)green << 8 | blue;
}

static void count_sample(GB_gameboy_t *gb, GB_sample_t *sample)
{
    (void)sample;
    capture_t *capture = GB_get_user_data(gb);
    capture->samples++;
    double t = (double) capture->ticks / (2.0 * capture->clock_rate);
    if (t > 3.1413 && t < 3.1426) {
        fprintf(stderr, "%.9f,%" PRIu64 ",%d,%d,%.17g,%.17g,%d,%d,%d,%d,%d,%u,%u,%u\n", t, capture->samples, sample->left, sample->right,
            gb->apu_output.dac_discharge[GB_NOISE], gb->apu_output.highpass_diff.left,
            gb->apu_output.band_limited[GB_NOISE].output.left,
            gb->apu_output.band_limited[GB_NOISE].input.left,
            gb->apu.samples[GB_NOISE], gb->apu.is_active[GB_NOISE],
            gb->apu.noise_channel.current_lfsr_sample, gb->io_registers[GB_IO_NR42],
            gb->io_registers[GB_IO_NR51], gb->apu_output.band_limited[GB_NOISE].pos);
    }
}

static bool trace_write(GB_gameboy_t *gb, uint16_t address, uint8_t value)
{
    capture_t *capture = GB_get_user_data(gb);
    if (capture->trace && address >= 0xff10 && address <= 0xff3f) {
        fprintf(capture->trace, "%" PRIu64 ",%" PRIu64 ",%.9f,0x%04x,0x%02x\n",
                capture->writes++, capture->ticks,
                (double)capture->ticks / (2.0 * capture->clock_rate), address, value);
    }
    if (address == 0xff80) {
        capture->case_id = (capture->case_id & 0xff00) | value;
    }
    else if (address == 0xff81) {
        capture->case_id = (capture->case_id & 0x00ff) | (uint16_t)value << 8;
    }
    else if (address == 0xff82) {
        if (capture->markers) {
            fprintf(capture->markers, "%" PRIu64 ",%" PRIu64 ",%.9f,%u,%u\n",
                    capture->marker_events, capture->ticks,
                    (double)capture->ticks / (2.0 * capture->clock_rate), capture->case_id, value);
        }
        capture->marker_events++;
        if (value == 4) capture->suite_done = true;
    }
    return true;
}

static bool write_screenshot(const char *path, const uint32_t *pixels)
{
    /* DMG LCD geometry, fixed by the hardware model used by this frontend. */
    FILE *file = fopen(path, "wb");
    if (!file) {
        perror(path);
        return false;
    }
    bool success = fprintf(file, "P6\n160 144\n255\n") > 0;
    for (unsigned i = 0; success && i < 160 * 144; i++) {
        uint8_t rgb[] = {pixels[i] >> 16, pixels[i] >> 8, pixels[i]};
        success = fwrite(rgb, sizeof(rgb), 1, file) == 1;
    }
    if (fclose(file)) success = false;
    if (!success) fprintf(stderr, "Could not write screenshot: %s\n", path);
    return success;
}

int main(int argc, char **argv)
{
    const char *rom = NULL, *sav = NULL, *boot = NULL, *wav = NULL, *screenshot = NULL;
    const char *buttons = NULL, *trace = NULL, *markers = NULL;
    double seconds = -1;
    uint64_t seed = 0;
    bool has_seed = false, until_done = false;
    GB_highpass_mode_t highpass = GB_HIGHPASS_ACCURATE;
    for (signed i = 1; i < argc; i++) {
        const char *option = argv[i];
        if (!strcmp(option, "--help")) {
            usage(argv[0]);
            return 0;
        }
        if (!strcmp(option, "--until-done")) {
            until_done = true;
            continue;
        }
        if (++i == argc) {
            usage(argv[0]);
            return 1;
        }
        const char *value = argv[i];
        if (!strcmp(option, "--rom")) rom = value;
        else if (!strcmp(option, "--sav")) sav = value;
        else if (!strcmp(option, "--boot")) boot = value;
        else if (!strcmp(option, "--wav")) wav = value;
        else if (!strcmp(option, "--screenshot")) screenshot = value;
        else if (!strcmp(option, "--buttons")) buttons = value;
        else if (!strcmp(option, "--apu-log")) trace = value;
        else if (!strcmp(option, "--markers-log")) markers = value;
        else if (!strcmp(option, "--seconds")) {
            if (!parse_seconds(value, &seconds) || seconds == 0) {
                fprintf(stderr, "Invalid positive duration: %s\n", value);
                return 1;
            }
        }
        else if (!strcmp(option, "--seed")) {
            char *end;
            errno = 0;
            seed = strtoull(value, &end, 0);
            if (errno || end == value || *end || *value == '-') {
                fprintf(stderr, "Invalid unsigned seed: %s\n", value);
                return 1;
            }
            has_seed = true;
        }
        else if (!strcmp(option, "--highpass")) {
            if (!strcmp(value, "accurate")) highpass = GB_HIGHPASS_ACCURATE;
            else if (!strcmp(value, "off")) highpass = GB_HIGHPASS_OFF;
            else if (!strcmp(value, "dc")) highpass = GB_HIGHPASS_REMOVE_DC_OFFSET;
            else {
                fprintf(stderr, "Unknown highpass mode: %s\n", value);
                return 1;
            }
        }
        else {
            fprintf(stderr, "Unknown option: %s\n", option);
            return 1;
        }
    }
    if (until_done && seconds > 0) {
        fputs("--seconds and --until-done are mutually exclusive.\n", stderr);
        return 1;
    }
    if (!rom || !boot || !wav || !screenshot || (!until_done && seconds <= 0) || !has_seed) {
        usage(argv[0]);
        return 1;
    }

    GB_random_seed(seed);
    GB_gameboy_t *gb = GB_alloc();
    if (!gb) {
        perror("GB_alloc");
        return 1;
    }
    GB_init(gb, GB_MODEL_DMG_B);
    capture_t capture = {.clock_rate = GB_get_clock_rate(gb)};
    uint32_t pixels[160 * 144] = {0};
    button_event_t *events = NULL;
    size_t event_count = 0, next_event = 0;
    uint64_t end_tick = 0;
    signed result = 1;
    if (!until_done && !seconds_to_ticks(seconds, capture.clock_rate, &end_tick)) {
        fprintf(stderr, "Duration exceeds the tick counter's range.\n");
        goto done;
    }
    if (buttons && !read_buttons(buttons, capture.clock_rate, &events, &event_count)) goto done;
    signed error = GB_load_boot_rom(gb, boot);
    if (!error) error = GB_load_rom(gb, rom);
    if (!error && sav) error = GB_load_battery(gb, sav);
    if (error) {
        fprintf(stderr, "Could not load input: %s\n", strerror(error));
        goto done;
    }
    GB_set_user_data(gb, &capture);
    GB_set_pixels_output(gb, pixels);
    GB_set_rgb_encode_callback(gb, encode_rgb);
    GB_set_border_mode(gb, GB_BORDER_NEVER);
    GB_set_turbo_mode(gb, true, true);
    GB_set_turbo_cap(gb, 0);
    GB_set_rtc_mode(gb, GB_RTC_MODE_ACCURATE);
    GB_set_emulate_joypad_bouncing(gb, false);
    GB_set_sample_rate(gb, 48000);
    GB_set_highpass_filter_mode(gb, highpass);
    GB_set_interference_volume(gb, 0);
    GB_apu_set_sample_callback(gb, count_sample);
    if (trace) {
        capture.trace = fopen(trace, "w");
        if (!capture.trace) {
            perror(trace);
            goto done;
        }
        fputs("write_index,run_start_tick_8mhz,run_start_seconds,address,value\n", capture.trace);
    }
    if (markers) {
        capture.markers = fopen(markers, "w");
        if (!capture.markers) {
            perror(markers);
            goto done;
        }
        fputs("event_index,run_start_tick_8mhz,run_start_seconds,case_id,event\n", capture.markers);
    }
    if (trace || markers || until_done) GB_set_write_memory_callback(gb, trace_write);
    error = GB_start_audio_recording(gb, wav, GB_AUDIO_FORMAT_WAV);
    if (error) {
        fprintf(stderr, "Could not start WAV recording: %s\n", strerror(error));
        goto done;
    }
    while (until_done ? !capture.suite_done : capture.ticks < end_tick) {
        while (next_event < event_count && events[next_event].tick <= capture.ticks) {
            button_event_t *event = &events[next_event++];
            GB_set_key_state(gb, event->key, event->pressed);
        }
        capture.ticks += GB_run(gb);
    }
    error = GB_stop_audio_recording(gb);
    if (error) {
        fprintf(stderr, "Could not finalize WAV recording: %s\n", strerror(error));
        goto done;
    }
    if (!write_screenshot(screenshot, pixels)) goto done;
    fprintf(stderr, "DMG-B seed=%" PRIu64 " highpass=%u ticks_8mhz=%" PRIu64
            " seconds=%.9f samples=%" PRIu64 " inputs=%zu/%zu apu_writes=%" PRIu64 "\n",
            seed, highpass, capture.ticks, (double)capture.ticks / (2.0 * capture.clock_rate),
            capture.samples, next_event, event_count, capture.writes);
    if (markers || until_done) {
        fprintf(stderr, "markers=%" PRIu64 " suite_done=%s\n",
                capture.marker_events, capture.suite_done ? "yes" : "no");
    }
    result = 0;
done:
    if (capture.trace) {
        if (ferror(capture.trace)) result = 1;
        if (fclose(capture.trace)) result = 1;
    }
    if (capture.markers) {
        if (ferror(capture.markers)) result = 1;
        if (fclose(capture.markers)) result = 1;
    }
    free(events);
    GB_dealloc(gb);
    return result;
}
