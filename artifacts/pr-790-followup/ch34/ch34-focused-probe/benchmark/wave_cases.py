"""Separate CH3 expiry time, RAM contents, bus access, and DAC history.

Cases expose the retained sample on retrigger, not the hidden playback index.
The caller supplies the existing audio engine and calls finish() exactly once.
"""
from cases import Case, FRAME

EXPOSURE_T = 8192


def fill(c, value):
    for address in range(16):
        c.write(0x30 + address, value)


def begin(name, group, initial, mode='expired-dac-on', **parameters):
    c = Case(name, group, 'Observe the held CH3 sample after controlled inactive history.',
             parameters={
                 'initial_byte': initial, 'mode': mode,
                 'acquisition_frequency_register': 1, 'probe_frequency_register': 1,
                 'sample_period_T': 4094, 'pre_trigger_wait_T': 1024,
                 'expected_activity': [0 if mode == 'never-triggered' else 4, 0, 0, 4],
                 'observable': 'Retained sample value, not hidden playback-index advancement.',
                 **parameters,
             })
    # C800 is outside the engine's C000-C20C packet/readback buffers and stack.
    c.emit('    ld a, $3c', '    ld [$c800], a')
    c.write(0x25, 0x44)
    c.write(0x1a, 0)
    fill(c, initial)
    c.write(0x1b, 0xfc)
    c.write(0x1c, 0x20)
    c.write(0x1d, 1)
    c.write(0x1a, 0x80)
    c.wait(1024)
    c.write(0x1e, 0x40 if mode == 'never-triggered' else 0xc0)
    c.wait(8192)
    c.read(0x26, 'NR52-after-acquisition')
    if mode == 'never-triggered':
        # Deliberately not an exactly time-matched expiry treatment: this
        # control has no active state to poll. Its frequency gate never starts.
        c.wait(65536)
        c.read(0x26, 'NR52-after-never-triggered-dwell')
    elif mode == 'dac-stop-active':
        c.write(0x1a, 0)
        c.read(0x26, 'NR52-after-DAC-stop')
    else:
        # Successful inactive observation is at most one 56-T iteration late.
        # BIT preserves the raw NR52 byte. On timeout the BC zero check has
        # clobbered A, so explicitly read NR52 again rather than report zero.
        slot = len(c.readback_labels)
        c.emit('    ld bc, 4096', '.expiry_poll:',
               '    ldh a, [$ff26]', '    bit 2, a', '    jr z, .expired',
               '    dec bc', '    ld a, b', '    or c', '    jr nz, .expiry_poll',
               '    ldh a, [$ff26]', '.expired:',
               f'    ld [wReadback + {slot}], a')
        c.readback_labels.append('NR52-observed-length-expiry')
        if mode == 'dac-off-after-expiry':
            c.write(0x1a, 0)
    return c


def expose(c, mode, duration=EXPOSURE_T):
    # Each variant spends 12 T loading HL, then exactly duration T. No mode
    # changes the wave RAM contents. NOPs still fetch ROM: not a neutral bus.
    address = 0xc800 if mode == 'wram-read' else 0xff3f
    c.emit(f'    ld hl, ${address:04x}', f'    REPT {duration // 8}')
    if mode == 'idle':
        c.emit('        nop', '        nop')
    else:
        c.emit('        ld a, [hl]')
    c.emit('    ENDR')


def probe(c):
    c.read(0x26, 'NR52-before-retrigger')
    if c.parameters['mode'] in ('dac-stop-active', 'dac-off-after-expiry'):
        c.write(0x1a, 0x80)
    else:
        c.wait(20)  # Avoid same-value NR30 writes in DAC-on treatments.
    # NR33 stays at 1: only the trigger/length setting changes.
    c.write(0x1e, 0x80)
    c.wait(64)
    c.read(0x26, 'NR52-after-retrigger')
    c.wait(8192)
    c.write(0x1c, 0)
    c.wait(FRAME)
    return c


def wave_cases():
    result = []
    for initial in (0x00, 0x55, 0xaa, 0xff):
        for wait in (0, 256, 1024, 4096, 16384):
            c = begin(f'ch3-hold-{initial:02x}-{wait}', 'ch3-expiry-retain', initial,
                      post_expiry_wait_T=wait, subsequent_wave_RAM_access=False)
            c.wait(wait)
            result.append(probe(c))

    for delay in (0, 4096):
        for replacement in (0x00, 0x55, 0xaa, 0xff):
            for access in ('idle', 'wave-read', 'wram-read'):
                c = begin(f'ch3-bus-{delay}-{replacement:02x}-{access}', 'ch3-bus', 0x55,
                          pre_rewrite_wait_T=delay, replacement_byte=replacement,
                          exposure=access, exposure_T=EXPOSURE_T, exposure_setup_T=12)
                c.wait(delay)
                fill(c, replacement)
                expose(c, access)
                result.append(probe(c))

        # Match the idle bus arm's duration without rewriting RAM. Replace
        # each 20-T write separately; keep the same subsequent NOP exposure.
        c = begin(f'ch3-no-rewrite-{delay}', 'ch3-bus-no-write', 0x55,
                  pre_rewrite_wait_T=delay, exposure='idle', exposure_T=EXPOSURE_T,
                  exposure_setup_T=12, subsequent_wave_RAM_access=False,
                  matched_case=f'ch3-bus-{delay}-55-idle')
        c.wait(delay)
        for _ in range(16):
            c.wait(20)
        expose(c, 'idle')
        result.append(probe(c))

    for replacement in (0x00, 0x55, 0xaa, 0xff):
        for access in ('idle', 'wave-read'):
            c = begin(f'ch3-never-{replacement:02x}-{access}', 'ch3-never-triggered', 0x55,
                      mode='never-triggered', replacement_byte=replacement,
                      exposure=access, exposure_T=EXPOSURE_T, exposure_setup_T=12,
                      fixed_untriggered_dwell_T=65536)
            fill(c, replacement)
            expose(c, access)
            result.append(probe(c))

    for initial in (0x00, 0x55, 0xaa, 0xff):
        for mode in ('dac-stop-active', 'dac-off-after-expiry'):
            c = begin(f'ch3-{mode}-{initial:02x}', 'ch3-dac-history', initial, mode=mode,
                      post_stop_wait_T=8192, subsequent_wave_RAM_access=False)
            c.wait(8192)
            result.append(probe(c))
    return result
