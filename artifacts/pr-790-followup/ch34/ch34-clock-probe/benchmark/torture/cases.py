"""Deterministic DMG stimuli. Field ranges come from the hardware registers.

Cases produce observations, not assumed hardware pass/fail answers. Generated
assembly and the emulator write trace are the instruction-schedule authority.
"""
from dataclasses import dataclass, field

FRAME_STEP = 8192
FRAME = 8 * FRAME_STEP
LENGTH_TICK = 2 * FRAME_STEP
SWEEP_TICK = 4 * FRAME_STEP
WAVE = [(i << 4) | ((i + 1) & 15) for i in range(16)]
REGS = {
    1: (0x11, 0x12, 0x13, 0x14, 0x11),
    2: (0x16, 0x17, 0x18, 0x19, 0x22),
    4: (0x20, 0x21, 0x22, 0x23, 0x88),
}


@dataclass
class Case:
    name: str
    group: str
    description: str
    lines: list = field(default_factory=list)
    readback_labels: list = field(default_factory=list)
    parameters: dict = field(default_factory=dict)

    def emit(self, *lines):
        self.lines.extend(lines)

    def write(self, reg, value):
        self.emit(f"    write_reg ${0xff00 | reg:04x}, ${value:02x}")

    def wait(self, cycles):
        assert cycles >= 0 and cycles % 4 == 0
        self.emit(f"    wait_cycles {cycles}")

    def reset(self):
        for reg, value in [(0x26, 0), (4, 0), (0x26, 0x80), (0x24, 0x77), (0x25, 0)]:
            self.write(reg, value)

    def read(self, reg, label):
        index = len(self.readback_labels)
        if index >= 255:
            raise ValueError('Readback exceeds the protocol uint8 length field')
        self.emit(f"    ldh a, [${0xff00 | reg:04x}]", f"    ld [wReadback + {index}], a")
        self.readback_labels.append(label)

    def ram(self, label):
        for address in range(16):
            self.read(0x30 + address, f'{label}/wave{address:02x}')

    def pulse(self, channel, volume=0xf0, frequency=0x600, duty=2, sweep=0, length=0, enabled=False):
        a, b, c, d, route = REGS[channel]
        self.write(0x25, route)
        if channel == 1:
            self.write(0x10, sweep)
        self.write(a, (duty << 6) | length)
        self.write(b, volume)
        self.write(c, frequency & 255)
        self.write(d, 0x80 | (0x40 if enabled else 0) | (frequency >> 8))

    def wave(self, frequency=0x700, level=1, length=0, enabled=False, fill=True):
        self.write(0x25, 0x44)
        self.write(0x1a, 0)
        if fill:
            for i, value in enumerate(WAVE):
                self.write(0x30 + i, value)
        self.write(0x1b, length)
        self.write(0x1c, level << 5)
        self.write(0x1d, frequency & 255)
        self.write(0x1a, 0x80)
        self.write(0x1e, 0x80 | (0x40 if enabled else 0) | (frequency >> 8))

    def noise(self, polynomial=0x6e, volume=0x98, length=0, enabled=False):
        self.write(0x25, 0x88)
        self.write(0x20, length)
        self.write(0x21, volume)
        self.write(0x22, polynomial)
        self.write(0x23, 0xc0 if enabled else 0x80)

    def channel(self, channel, **kwargs):
        if channel in (1, 2):
            self.pulse(channel, **kwargs)
        elif channel == 3:
            self.wave(**kwargs)
        else:
            self.noise(**kwargs)

    def finish(self):
        self.emit(f"    ld a, {len(self.readback_labels)}", "    ld [wReadbackSize], a", "    ret")


def catalog():
    result = []

    def add(name, group, description, **parameters):
        c = Case(name, group, description, parameters=parameters)
        result.append(c)
        return c

    c = add('calibration-pulse', 'calibration',
            'Uninterrupted CH2 256 Hz, duty 50%, volume 8 for eight complete frame-sequencer cycles.',
            frequency_hz=256, duty=2, volume=8)
    c.pulse(2, volume=0x88)
    c.wait(8 * FRAME)

    for channel in (1, 2):
        group = f'ch{channel}'
        a, b, low, high, route = REGS[channel]
        for active in (False, True):
            c = add(f'{group}-duty-{"live" if active else "trigger"}', group,
                    'All four duty encodings, changed during playback or with fresh triggers.', duty=list(range(4)))
            c.pulse(channel)
            for duty in range(4):
                c.write(a, duty << 6)
                if not active:
                    c.write(high, 0x86)
                c.wait(FRAME)

        c = add(f'{group}-period-domain', group,
                'All 2048 period values, low then high register writes without retrigger; each hold spans the slowest pulse period.',
                frequency_register_domain=[0, 2047], hold_T=FRAME)
        c.pulse(channel)
        c.emit('    ld de, 0', '.period:')
        c.emit('    ld a, e', f'    ldh [${0xff00 | low:04x}], a',
               '    ld a, d', f'    ldh [${0xff00 | high:04x}], a')
        c.wait(FRAME)
        c.emit('    inc de', '    bit 3, d', '    jr z, .period')

        c = add(f'{group}-period-boundaries', group,
                'Byte carries, timer extrema, and >=700 to <700 live writes; both register orders and trigger controls.')
        for old, new in [(0xff, 0x100), (0x100, 0xff), (0x6ff, 0x700), (0x700, 0x6ff), (0x7fe, 0x7ff), (0x7ff, 0x7fe), (0, 0x7ff), (0x7ff, 0)]:
            for order in ('low-high', 'high-low'):
                c.reset()
                c.pulse(channel, frequency=old)
                c.wait(FRAME)
                writes = [(low, new & 255), (high, new >> 8)]
                for reg, val in writes if order == 'low-high' else reversed(writes):
                    c.write(reg, val)
                c.wait(FRAME)
                c.write(high, 0x80 | (new >> 8))
                c.wait(FRAME)

        for reset_kind in ('active', 'dac', 'power'):
            c = add(f'{group}-restart-{reset_kind}', group,
                    'Retrigger after each of the eight duty-step offsets; contrast retained duty, DAC cycling and APU power cycling.',
                    duty_offsets_T=[n * 2048 for n in range(8)])
            for phase in range(8):
                c.reset()
                c.pulse(channel)
                c.wait(phase * 2048)
                if reset_kind == 'dac':
                    c.write(b, 0)
                    c.write(b, 0xf0)
                elif reset_kind == 'power':
                    c.reset()
                    c.pulse(channel)
                c.write(high, 0x86)
                c.wait(FRAME)

    for channel in (1, 2, 4):
        group = f'ch{channel}'
        length, env, period, trigger, _ = REGS[channel]
        c = add(f'{group}-volume-domain', group,
                'All 16 initial volumes, pace zero, DAC enabled even for volume zero.', initial_volume=list(range(16)))
        for volume in range(16):
            c.reset()
            c.channel(channel, volume=(volume << 4) | 8)
            c.wait(FRAME)

        for direction in (0, 1):
            for pace in range(8):
                c = add(f'{group}-envelope-{"up" if direction else "down"}-{pace}', group,
                        'Observe every volume step through saturation, including one terminal-volume step; pace zero is a separate disabled-envelope control.',
                        direction=direction, pace=pace,
                        hold_T=(16 * pace * FRAME if pace else FRAME))
                c.channel(channel, volume=((8 if direction else 0xf0) | pace))
                c.wait(16 * pace * FRAME if pace else FRAME)
                c.read(0x26, 'NR52-after-envelope')

        c = add(f'{group}-envelope-live', group,
                'Sixteen no-trigger 08 writes from F8, then pace/direction changes with matching retrigger observations; zombie behavior may vary by unit.')
        c.channel(channel, volume=0xf8)
        for i in range(16):
            c.write(env, 8)
            c.wait(FRAME)
        for value in (9, 10, 9, 8, 9, 1, 9):
            c.write(env, value)
            c.wait(FRAME)
            c.write(trigger, 0x80 if channel == 4 else 0x86)
            c.wait(FRAME)

    for channel in (1, 2, 3, 4):
        group = f'ch{channel}'
        length, trigger, maximum = ((0x1b, 0x1e, 256) if channel == 3 else (REGS[channel][0], REGS[channel][3], 64))
        high = 0xc7 if channel == 3 else (0xc0 if channel == 4 else 0xc6)
        c = add(f'{group}-length-domain', group,
                'Every length load through expiry, followed by an entire frame of silence. Runtime loop preserves the full 64/256-value hardware domain.',
                length_register_domain=[0, maximum - 1], length_tick_T=LENGTH_TICK)
        c.channel(channel)
        c.emit('    ld d, 0', '.length:')
        c.emit('    ld a, d', f'    ldh [${0xff00 | length:04x}], a')
        c.write(trigger, high)
        c.emit('    ld a, d', '    cpl', '    inc a')
        if maximum == 64:
            c.emit('    and $3f', '    jr nz, .count', '    ld a, 64', '.count:')
        c.emit('    ld e, a', '.ticks:')
        c.wait(LENGTH_TICK)
        c.emit('    dec e', '    jr nz, .ticks')
        c.wait(FRAME)
        c.emit('    inc d')
        if maximum == 64:
            c.emit('    ld a, d', '    cp 64', '    jr nz, .length')
        else:
            c.emit('    jr nz, .length')
        c.read(0x26, 'NR52-after-all-expiries')

        c = add(f'{group}-length-div-phases', group,
                'One count remaining; enable length without retrigger across every frame position and both DIV-bit halves, then reset DIV. Status is sampled before and after a length tick.',
                frame_positions=list(range(8)), div_half_offsets_T=[0, FRAME_STEP // 2])
        for phase in range(8):
            for half in (0, FRAME_STEP // 2):
                c.reset()
                c.channel(channel, length=maximum - 1)
                c.write(4, 0)
                c.wait(phase * FRAME_STEP + half)
                c.write(trigger, high & 0x7f)
                c.write(4, 0)
                c.read(0x26, f'phase{phase}-half{half}-immediate')
                c.wait(LENGTH_TICK)
                c.read(0x26, f'phase{phase}-half{half}-later')

    for direction in (0, 1):
        c = add(f'ch1-sweep-shift-{direction}', 'ch1',
                'All shift values at pace 1; status before and after two sweep clocks.', negate=direction, shifts=list(range(8)))
        for shift in range(8):
            c.reset()
            c.pulse(1, frequency=0x400, sweep=0x10 | direction * 8 | shift)
            c.read(0x26, f'shift{shift}-trigger')
            c.wait(2 * SWEEP_TICK)
            c.read(0x26, f'shift{shift}-after')
        c = add(f'ch1-sweep-pace-{direction}', 'ch1',
                'All sweep paces, shift 1; zero includes the internal eight-tick control interval.', negate=direction, paces=list(range(8)))
        for pace in range(8):
            c.reset()
            c.pulse(1, frequency=0x400, sweep=(pace << 4) | direction * 8 | 1)
            c.wait(2 * (pace or 8) * SWEEP_TICK)
            c.read(0x26, f'pace{pace}-after')

    for name, sweep, frequency in [('trigger-overflow', 0x11, 0x600), ('pace-zero-overflow', 1, 0x600), ('shift-zero-control', 0, 0x600), ('second-overflow', 0x11, 0x400), ('negate-clear', 0x19, 0x600), ('negate-clear-no-calculation', 0x18, 0x600)]:
        c = add(f'ch1-sweep-{name}', 'ch1', 'Targeted sweep overflow/negate control with immediate and later NR52 readback.', nr10=sweep, frequency_register=frequency)
        c.pulse(1, frequency=frequency, sweep=sweep)
        c.read(0x26, 'after-trigger')
        if name.startswith('negate-clear'):
            c.write(0x10, sweep & ~8)
            c.read(0x26, 'after-negate-clear')
        c.wait(2 * SWEEP_TICK)
        c.read(0x26, 'after-two-sweep-ticks')
    for retrigger in (False, True):
        c = add(f'ch1-sweep-shadow-{"retrigger" if retrigger else "live"}', 'ch1',
                'Change live period from 200 to100 before sweep, with/without shadow reload.')
        c.pulse(1, frequency=0x200, sweep=0x11)
        c.write(0x13, 0)
        c.write(0x14, 0x81 if retrigger else 1)
        c.wait(8 * SWEEP_TICK)
        c.read(0x26, 'after-shadow-sweep')

    for active in (False, True):
        c = add(f'ch3-volume-{"live" if active else "trigger"}', 'ch3',
                'All four CH3 output shifts on a 32-position asymmetric wave, including digital mute.', wave_bytes=WAVE, volume_codes=list(range(4)))
        c.wave()
        for level in range(4):
            c.write(0x1c, level << 5)
            if not active:
                c.write(0x1e, 0x87)
            c.wait(FRAME)

    c = add('ch3-period-domain', 'ch3', 'All 2048 live periods; each hold spans the slowest full 32-sample wave period.', frequency_register_domain=[0, 2047], hold_T=2 * FRAME)
    c.wave()
    c.emit('    ld de, 0', '.period:', '    ld a, e', '    ldh [$ff1d], a', '    ld a, d', '    ldh [$ff1e], a')
    c.wait(2 * FRAME)
    c.emit('    inc de', '    bit 3, d', '    jr z, .period')

    for enabled in (False, True):
        c = add(f'ch3-inactive-ram-dac-{int(enabled)}', 'ch3',
                'Write and read all wave RAM while inactive, with DAC off or on. Subsequent playback exposes retained bytes.')
        c.write(0x1a, 0x80 if enabled else 0)
        for i, value in enumerate(WAVE):
            c.write(0x30 + i, value)
        c.ram('inactive')
        c.wave(fill=False)
        c.wait(FRAME)

    for phase in range(0, 32, 4):
        c = add(f'ch3-active-read-phase-{phase}', 'ch3',
                'Every CPU wave-RAM address, independently triggered then read at this 4T-reachable fetch offset. F=7F8 gives16T per sample/32T per byte.', offset_T=phase)
        for address in range(16):
            c.reset()
            c.wave(frequency=0x7f8)
            c.wait(phase)
            c.read(0x30 + address, f'active-address{address:02x}')
            c.write(0x1a, 0)
        c.wave(fill=False)
        c.wait(FRAME)

    for address in range(16):
        c = add(f'ch3-active-write-address-{address:02x}', 'ch3',
                'Write this address at every 4T-reachable offset over a32T fetch period, stop and transmit all16 bytes after each attempt.', address=0xff30 + address, offsets_T=list(range(0, 32, 4)))
        for phase in range(0, 32, 4):
            c.reset()
            c.wave(frequency=0x7f8)
            c.wait(phase)
            c.write(0x30 + address, WAVE[address] ^ 255)
            c.write(0x1a, 0)
            c.ram(f'offset{phase}')
        c.wave(fill=False)
        c.wait(FRAME)

    for safe in (False, True):
        for byte in range(16):
            c = add(f'ch3-retrigger-{"safe" if safe else "active"}-byte-{byte:02x}', 'ch3',
                    'Retrigger across a32T delay bucket, then transmit all wave RAM. Instruction/startup overhead means readback identifies the affected byte. Safe control disables DAC first; active corruption can vary between DMGs.',
                    nominal_32T_delay_bucket=byte, offsets_T=[byte * 32 + n for n in range(0, 32, 4)])
            for offset in range(byte * 32, (byte + 1) * 32, 4):
                c.reset()
                c.wave(frequency=0x7f8)
                c.wait(offset)
                if safe:
                    c.write(0x1a, 0)
                    c.write(0x1a, 0x80)
                c.write(0x1e, 0x87)
                c.write(0x1a, 0)
                c.ram(f'offset{offset}')
            c.wave(fill=False)
            c.wait(FRAME)

    for reset in (False, True):
        c = add(f'ch3-stale-buffer-{"power" if reset else "dac"}', 'ch3',
                'Stop after each sample-position delay, change first wave byte, then restart; APU reset control contrasts stale-buffer retention.', sample_offsets_T=[n * 16 for n in range(32)])
        for position in range(32):
            c.reset()
            c.wave(frequency=0x7f8)
            c.wait(position * 16)
            c.write(0x1a, 0)
            if reset:
                c.reset()
            c.write(0x30, 0xfe)
            c.wave(fill=False)
            c.wait(FRAME)

    # Full NR43 domain, split by shift so each section fits a physical ROM bank
    # and hardware differences have useful per-shift identifiers.
    for shift in range(16):
        c = add(f'ch4-polynomial-shift-{shift}', 'ch4',
                'Both widths and all eight divider encodings at this shift. Running clocks are observed through16 shifts (15-bit register plus feedback); stopped shifts14/15 use one frame.',
                shift=shift, widths=[0, 1], divisors=list(range(8)))
        for width in range(2):
            for divisor in range(8):
                c.reset()
                c.noise(polynomial=(shift << 4) | (width << 3) | divisor)
                period = (8 if divisor == 0 else 16 * divisor) << shift
                c.wait(max(FRAME, 16 * period) if shift < 14 else FRAME)
                c.read(0x26, f'width{width}-divider{divisor}-NR52')

    for retrigger in (False, True):
        c = add(f'ch4-live-NR43-{"trigger" if retrigger else "held"}', 'ch4',
                'Width/divider/shift/same-value/stopped-clock transitions in fixed780288T windows (one127-step 7-bit sequence only at6E); triggers distinguish recovery from retained state.')
        c.noise()
        for value in (0x6e, 0x66, 0x6e, 0x6f, 0x6e, 0x7e, 0x6e, 0xee, 0x6e, 0xfe, 0x6e):
            c.write(0x22, value)
            if retrigger:
                c.write(0x23, 0x80)
            c.wait(127 * 6144)

    for channel in (1, 2, 3, 4):
        c = add(f'mixer-route-ch{channel}', 'mixer', 'Off/right/left/both routing for an isolated channel, including the unrouted but enabled DAC.', channel=channel)
        c.channel(channel)
        for mask in (0, 1 << (channel - 1), 1 << (channel + 3), (1 << (channel - 1)) | (1 << (channel + 3))):
            c.write(0x25, mask)
            c.wait(FRAME)
    c = add('mixer-all-subsets', 'mixer', 'All16 channel subsets, same subset on both outputs; every DAC remains enabled.', channel_subsets=list(range(16)))
    for channel in (1, 2, 3, 4):
        c.channel(channel)
    for subset in range(16):
        c.write(0x25, subset * 17)
        c.wait(FRAME)
    c = add('mixer-NR50-domain', 'mixer', 'All64 left/right master-gain combinations with VIN disabled. Zero gain field is not mute.', gain_field_domain=[0, 7])
    c.pulse(2, volume=0x88)
    for left in range(8):
        for right in range(8):
            c.write(0x24, (left << 4) | right)
            c.wait(FRAME)

    for channel in (1, 2, 3, 4):
        for keep_other in (False, True):
            c = add(f'ch{channel}-DAC-off-{"control" if keep_other else "all-off"}', 'mixer',
                    'Tone, digital silence, DAC disable and re-enable. Control leaves another inactive unrouted DAC enabled.', channel=channel, keep_other_DAC=keep_other)
            c.channel(channel)
            if keep_other:
                c.write(0x17 if channel == 1 else 0x12, 8)
            c.wait(FRAME)
            c.write(0x1c if channel == 3 else REGS[channel][1], 0 if channel == 3 else 8)
            if channel != 3:
                c.write(REGS[channel][3], 0x80 if channel == 4 else 0x86)
            c.wait(FRAME)
            c.write(0x1a if channel == 3 else REGS[channel][1], 0)
            c.wait(FRAME)
            c.channel(channel)
            c.wait(FRAME)

    c = add('global-NR52-off-state', 'global',
            'NR52 low bits, power cycling, off-state wave/length writes, and post-on length expiry. Readback includes status and all wave RAM.')
    for channel in (1, 2, 3, 4):
        c.channel(channel)
    c.write(0x25, 0xff)
    for value in (0x80, 0x8f, 0):
        c.write(0x26, value)
        c.read(0x26, f'NR52-write-{value:02x}')
        c.wait(FRAME)
    for i, value in enumerate(WAVE):
        c.write(0x30 + i, value)
    for reg, value in [(0x11, 63), (0x16, 63), (0x1b, 255), (0x20, 63)]:
        c.write(reg, value)
    c.write(0x26, 0x80)
    c.ram('power-on')
    c.write(0x24, 0x77)
    c.write(0x25, 0xff)
    for env in (0x12, 0x17, 0x21):
        c.write(env, 0xf0)
    c.write(0x1a, 0x80)
    c.write(0x1c, 0x20)
    for reg, value in [(0x13, 0), (0x14, 0xc6), (0x18, 0), (0x19, 0xc6), (0x1d, 0), (0x1e, 0xc7), (0x22, 0x6e), (0x23, 0xc0)]:
        c.write(reg, value)
    c.read(0x26, 'off-written-lengths-trigger')
    c.wait(LENGTH_TICK)
    c.read(0x26, 'off-written-lengths-expired')

    for channel in (1, 2, 4):
        c = add(f'ch{channel}-DIV-envelope-phases', 'global',
                'NR52 power-on with DIV low/high, then DIV writes at all eight frame positions while an envelope runs. Delays are instruction-aligned; exact writes are in the trace.',
                power_on_offsets_T=[0, FRAME_STEP // 2], frame_positions=list(range(8)))
        for half in (0, FRAME_STEP // 2):
            for phase in range(8):
                c.reset()
                c.write(0x26, 0)
                c.write(4, 0)
                c.wait(half)
                c.write(0x26, 0x80)
                c.write(0x24, 0x77)
                c.channel(channel, volume=0x19)
                c.wait(phase * FRAME_STEP)
                c.write(4, 0)
                c.wait(2 * FRAME)
                c.read(0x26, f'power-half{half}-frame{phase}')

    c = add('interaction-gray-register-walk', 'interaction',
            'Deterministic stateful interaction walk: every8-bit Gray-code value reaches every listed register, with address-dependent rotations; no resets between writes. This is not all interleavings.',
            values=256, hold_per_round_T=FRAME_STEP,
            registers=[0xff00 | r for r in (0x10, 0x11, 0x12, 0x13, 0x14, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25)])
    # Compact loop, invertible XOR rotations enumerate each register's8-bit domain.
    c.emit('    ld d, 0', '.walk:', '    ld a, d', '    srl a', '    xor d', '    ld e, a')
    for index, reg in enumerate((0x10, 0x11, 0x12, 0x13, 0x14, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25)):
        c.emit('    ld a, e', f'    xor ${(index * 17) & 255:02x}', f'    ldh [${0xff00 | reg:04x}], a')
    c.wait(FRAME_STEP)
    c.emit('    inc d', '    jp nz, .walk')
    c.read(0x26, 'final-NR52')
    c.write(0x1a, 0)
    c.ram('final-wave')

    for case in result:
        case.finish()
    assert len({c.name for c in result}) == len(result)
    return result
