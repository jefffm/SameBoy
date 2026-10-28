/* Exact digital-output regression for DMG feedback captured on the opposite
   selected-counter edge. No audio filter, sample-rate tolerance or local fit. */
#define GB_INTERNAL
#include <Core/gb.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { PERIOD_T = 6144, SEQUENCE_STEPS = 127 };
static const char *rom;

static GB_gameboy_t *setup(GB_model_t model, unsigned counter)
{
    GB_random_seed(1);
    GB_gameboy_t *gb = GB_alloc();
    GB_init(gb, model);
    assert(GB_load_rom(gb, rom) == 0);
    GB_set_sample_rate(gb, 0);
    GB_apu_write(gb, GB_IO_NR52, 0);
    GB_apu_write(gb, GB_IO_NR52, 0x80);
    GB_apu_write(gb, GB_IO_NR50, 0x66);
    GB_apu_write(gb, GB_IO_NR51, 0x88);
    GB_apu_write(gb, GB_IO_NR42, 0x98);
    GB_apu_write(gb, GB_IO_NR43, 0x6e);
    /* Retained phase is the independent variable. No running divider reload or
       delayed-start corner is present in these two controlled starting states. */
    gb->apu.noise_channel.counter = counter;
    gb->apu.noise_channel.counter_countdown = 0;
    gb->apu.noise_channel.alignment = 0;
    GB_apu_write(gb, GB_IO_NR44, 0x80);
    return gb;
}

static void tick(GB_gameboy_t *gb)
{
    gb->apu.apu_cycles = 1; /* SameBoy APU tick = two DMG T-cycles. */
    GB_apu_run(gb, true);
}

static void output_edges(GB_model_t model, const char *name, unsigned phase)
{
    GB_gameboy_t *gb = setup(model, phase);
    /* Clock derivation: first divider tick 52T, subsequent ticks 48T.
       Phase0 reaches the first selected rising edge at3076T; phase64 at6148T.
       Seven 7-bit shift stages put the first output change six clocks later.
       JOTO reset to0 adds one clock only when a rising edge precedes capture. */
    unsigned first = phase == 0 ? 39940 : 43012;
    if (model < GB_MODEL_CGB_0 && phase == 0) first += PERIOD_T;
    const unsigned expected[] = {first, first + 6 * PERIOD_T, first + 7 * PERIOD_T};
    unsigned event = 0;
    bool previous = false;
    printf("%s phase=%u output_edges_T=", name, phase);
    for (unsigned t = 2; t <= expected[2]; t += 2) {
        tick(gb);
        bool sample = gb->apu.noise_channel.current_lfsr_sample;
        if (sample != previous) {
            printf("%s%u", event ? "," : "", t);
            fflush(stdout);
            assert(event < sizeof(expected) / sizeof(expected[0]));
            assert(t == expected[event++]);
            previous = sample;
        }
    }
    assert(event == sizeof(expected) / sizeof(expected[0]));
    puts("");
    GB_dealloc(gb);
}

static void reset_and_roundtrip(void)
{
    GB_gameboy_t *gb = setup(GB_MODEL_DMG_B, 0);
    /* At6148T the selected counter falls: JOTO=1 while the shift register
       remains zero. The next rising edge therefore needs the saved latch. */
    for (unsigned t = 0; t < 6148; t += 2) tick(gb);
    assert(gb->apu.noise_feedback_latch);
    assert(gb->apu.noise_channel.lfsr == 0);
    size_t size = GB_get_save_state_size(gb);
    uint8_t *state = malloc(size);
    assert(state);
    GB_save_state_to_buffer(gb, state);
    const size_t ticks = SEQUENCE_STEPS * PERIOD_T / 2;
    uint8_t *samples = malloc(ticks);
    assert(samples);
    for (size_t i = 0; i < ticks; i++) {
        tick(gb);
        samples[i] = gb->apu.noise_channel.current_lfsr_sample;
    }
    /* Deliberately dirty the destination field so an omitted load is visible. */
    gb->apu.noise_feedback_latch = false;
    assert(GB_load_state_from_buffer(gb, state, size) == 0);
    assert(gb->apu.noise_feedback_latch);
    for (size_t i = 0; i < ticks; i++) {
        tick(gb);
        assert(samples[i] == gb->apu.noise_channel.current_lfsr_sample);
    }
    assert(GB_load_state_from_buffer(gb, state, size) == 0);
    GB_apu_write(gb, GB_IO_NR44, 0x80);
    /* NR44's existing synchronized start can be delayed. The feedback latch
       and shift stages clear together when that actual restart takes place. */
    unsigned delay = gb->apu.noise_channel.dmg_delayed_start;
    for (unsigned i = 0; i < delay; i++) tick(gb);
    assert(!gb->apu.noise_feedback_latch);
    assert(GB_load_state_from_buffer(gb, state, size) == 0);
    GB_apu_write(gb, GB_IO_NR52, 0);
    assert(!gb->apu.noise_feedback_latch);
    assert(GB_load_state_from_buffer(gb, state, size) == 0);
    GB_apu_init(gb);
    assert(!gb->apu.noise_feedback_latch);
    puts("NR44_clear=pass APU_off_clear=pass APU_init_clear=pass mid_halfcycle_save_load=pass full_127_step_output_identical=pass");
    free(samples);
    free(state);
    GB_dealloc(gb);
}

static GB_gameboy_t *mux_setup(unsigned counter, unsigned nr43)
{
    GB_gameboy_t *gb = setup(GB_MODEL_DMG_B, 0);
    /* Establish the pre-write state. The independent input is the mux clock
       level; no normal clock or counter-reload collision occurs in the write. */
    gb->apu.noise_channel.counter = counter;
    gb->apu.noise_channel.countdown_reloaded = false;
    gb->io_registers[GB_IO_NR43] = nr43;
    gb->apu.noise_channel.narrow = true;
    gb->apu.noise_channel.lfsr = 0;
    gb->apu.noise_feedback_latch = false;
    return gb;
}

static void nr43_clock_edges(void)
{
    /* Shift15 selects zero, bit6 of64 is one. The ordinary 0->1 write must
       consume the reset latch rather than freshly computing XNOR(0,0)=1. */
    GB_gameboy_t *gb = mux_setup(64, 0xfa);
    GB_apu_write(gb, GB_IO_NR43, 0x6e);
    assert(gb->apu.noise_channel.lfsr == 0);
    assert(!gb->apu.noise_feedback_latch);
    GB_dealloc(gb);

    gb = mux_setup(64, 0x6e);
    GB_apu_write(gb, GB_IO_NR43, 0xfa);
    assert(gb->apu.noise_channel.lfsr == 0);
    assert(gb->apu.noise_feedback_latch);
    GB_dealloc(gb);

    /* Counter128 has bit7=1, bit6=0. FF->6E passes 0->1->0: consume the old
       zero first, then capture XNOR of the shifted zero state on the fall. */
    gb = mux_setup(128, 0xfa);
    GB_apu_write(gb, GB_IO_NR43, 0x6e);
    assert(gb->apu.noise_channel.lfsr == 0);
    assert(gb->apu.noise_feedback_latch);
    GB_dealloc(gb);

    /* DMG's existing FF transition means a same-value write can clock too:
       at counter64, 6E->FF captures one, then FF->6E consumes that one. */
    gb = mux_setup(64, 0x6e);
    GB_apu_write(gb, GB_IO_NR43, 0x6e);
    assert(gb->apu.noise_channel.lfsr == 0x4040);
    assert(gb->apu.noise_feedback_latch);
    GB_dealloc(gb);
    puts("NR43_rise_consumes=pass fall_captures=pass pulse_consumes_then_refreshes=pass same_value_FF_transition=pass");
}

static void delayed_reset(void)
{
    GB_gameboy_t *gb = setup(GB_MODEL_DMG_B, 0);
    for (unsigned t = 0; t < 6148; t += 2) tick(gb);
    assert(gb->apu.noise_feedback_latch);
    assert(gb->apu.noise_channel.lfsr == 0);
    /* Put an old-channel rising edge two APU ticks into the existing six-tick
       synchronized trigger delay. It must still consume the old feedback. */
    gb->apu.noise_channel.counter = 63;
    gb->apu.noise_channel.counter_countdown = 2;
    gb->apu.noise_channel.alignment = 2;
    GB_apu_write(gb, GB_IO_NR44, 0x80);
    unsigned delay = gb->apu.noise_channel.dmg_delayed_start;
    assert(delay == 6);
    assert(gb->apu.noise_feedback_latch);
    tick(gb);
    tick(gb);
    assert(gb->apu.noise_channel.lfsr == 0x4040);
    for (unsigned i = 2; i < delay; i++) tick(gb);
    assert(!gb->apu.noise_channel.dmg_delayed_start);
    assert(!gb->apu.noise_feedback_latch);
    assert(gb->apu.noise_channel.lfsr == 0);
    puts("delayed_NR44_preserves_old_edge=pass synchronized_reset_clears_both=pass");
    GB_dealloc(gb);
}

static void legacy_state(void)
{
    GB_gameboy_t *gb = setup(GB_MODEL_DMG_B, 0);
    /* After six data shifts and their next falling edge, low bits10 imply
       feedback0. This differs from both the live latch and poisoned padding. */
    for (unsigned t = 0; t < 43012; t += 2) tick(gb);
    uint16_t saved_lfsr = gb->apu.noise_channel.lfsr;
    assert((saved_lfsr & 3) == 2);
    assert(!gb->apu.noise_feedback_latch);
    gb->rtc_real.seconds = 23;
    gb->ram[0] = 0x47;
    gb->ram[gb->ram_size - 1] = 0x42;
    gb->vram[0] = 0x59;
    size_t size = GB_get_save_state_size_no_bess(gb);
    uint8_t *state = malloc(size);
    assert(state);
    GB_save_state_to_buffer_no_bess(gb, state);
    /* Native save order is header, core_state, dma, mbc, hram, timing, apu.
       Reproduce the measured 82c17ac6 APU section, which was104 bytes including
       tail padding102..103. Native-only avoids unrelated BESS absolute offsets. */
    const size_t prefix = GB_SECTION_SIZE(header) +
        GB_SECTION_SIZE(core_state) + sizeof(uint32_t) +
        GB_SECTION_SIZE(dma) + sizeof(uint32_t) +
        GB_SECTION_SIZE(mbc) + sizeof(uint32_t) +
        GB_SECTION_SIZE(hram) + sizeof(uint32_t) +
        GB_SECTION_SIZE(timing) + sizeof(uint32_t);
    uint32_t saved_apu_size;
    memcpy(&saved_apu_size, state + prefix, sizeof(saved_apu_size));
    const uint32_t old_apu_size = 104;
    assert(saved_apu_size == GB_SECTION_SIZE(apu));
    assert(saved_apu_size > old_apu_size); /* Later fields may extend the section. */
    size_t apu_start = prefix + sizeof(uint32_t);
    state[apu_start + 102] = state[apu_start + 103] = 0xff;
    memcpy(state + prefix, &old_apu_size, sizeof(old_apu_size));
    memmove(state + apu_start + old_apu_size, state + apu_start + saved_apu_size,
            size - apu_start - saved_apu_size);
    size -= saved_apu_size - old_apu_size;
    gb->apu.noise_feedback_latch = true;
    gb->apu.noise_channel.lfsr = 0; /* Live feedback1 must not influence fallback. */
    gb->rtc_real.seconds = 0;
    gb->ram[0] = gb->ram[gb->ram_size - 1] = gb->vram[0] = 0;
    assert(GB_load_state_from_buffer(gb, state, size) == 0);
    assert(gb->apu.noise_channel.lfsr == saved_lfsr);
    /* The review deliberately drops exact legacy-latch reconstruction. Old
       padding must still become a valid bool, and the next edge must work. */
    uint8_t latch;
    memcpy(&latch, &gb->apu.noise_feedback_latch, sizeof(latch));
    assert(latch <= 1);
    for (unsigned t = 0; t < PERIOD_T; t += 2) tick(gb);
    assert(gb->rtc_real.seconds == 23);
    assert(gb->ram[0] == 0x47 && gb->ram[gb->ram_size - 1] == 0x42);
    assert(gb->vram[0] == 0x59);
    puts("legacy104byte_APU=pass poisoned_padding_normalized=pass following_sections_and_RAM=pass");
    free(state);
    GB_dealloc(gb);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    rom = argv[1];
    printf("sizeof_apu=%zu apu_section=%zu last_old_field=%zu old_fields_end=%zu",
           sizeof(GB_apu_t), (size_t) GB_SECTION_SIZE(apu),
           offsetof(GB_apu_t, noise_started_with_dac_disabled),
           offsetof(GB_apu_t, noise_started_with_dac_disabled) + sizeof(bool));
    printf(" latch_offset=%zu", offsetof(GB_apu_t, noise_feedback_latch));
    puts("");
#define MODEL(name) {name, #name}
    const struct {GB_model_t value; const char *name;} models[] = {
        MODEL(GB_MODEL_DMG_B), MODEL(GB_MODEL_MGB),
        MODEL(GB_MODEL_SGB), MODEL(GB_MODEL_SGB_PAL),
        MODEL(GB_MODEL_SGB_NTSC_NO_SFC), MODEL(GB_MODEL_SGB_PAL_NO_SFC),
        MODEL(GB_MODEL_SGB2), MODEL(GB_MODEL_SGB2_NO_SFC),
        MODEL(GB_MODEL_CGB_0), MODEL(GB_MODEL_CGB_A), MODEL(GB_MODEL_CGB_B),
        MODEL(GB_MODEL_CGB_C), MODEL(GB_MODEL_CGB_D), MODEL(GB_MODEL_CGB_E),
        MODEL(GB_MODEL_AGB), MODEL(GB_MODEL_GBP),
    };
    for (unsigned i = 0; i < sizeof(models) / sizeof(models[0]); i++) {
        output_edges(models[i].value, models[i].name, 0);
        output_edges(models[i].value, models[i].name, 64);
    }
    reset_and_roundtrip();
    delayed_reset();
    nr43_clock_edges();
    legacy_state();
    return 0;
}
