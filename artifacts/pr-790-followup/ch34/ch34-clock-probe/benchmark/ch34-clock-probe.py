#!/usr/bin/env python3
"""Build a finite headphone-output probe for observable CH3/CH4 silent clocks.

This does not equate timer activity with an advancing hidden index or LFSR.
The latter states reset at trigger and need a separate observable mechanism.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parent.parent
HERE = ROOT / 'benchmark/torture'
sys.path.insert(0, str(HERE))
from cases import Case, FRAME, FRAME_STEP


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def probe_cases():
    cases = []
    calibration = Case('calibration-pulse', 'calibration', 'CH2 256 Hz, 50% duty, volume 8.')
    calibration.pulse(2, volume=0x88)
    calibration.wait(8 * FRAME)
    cases.append(calibration)

    # All 16 bytes are identical, deliberately removing the unknown inactive
    # RAM-address mux from this test. A running fetch can replace the held byte;
    # a frozen fetch cannot. The first post-trigger sample lasts about 1 ms.
    for initial in (0x00, 0xff):
        replacement = initial ^ 0xff
        for mode in ('expired-dac-on', 'dac-off', 'never-triggered'):
            for gap in (0, 1024, 2048, 4096, 8192, 16384):
                c = Case(f'ch3-{initial:02x}-{replacement:02x}-{mode}-{gap}', 'ch3-fetch',
                         'Expire length, replace wave RAM, wait, retrigger; inspect the held first sample.',
                         parameters={'initial_byte': initial, 'replacement_byte': replacement,
                                     'gap_T': gap, 'mode': mode,
                                     'acquisition_frequency_register': 1, 'acquisition_sample_period_T': 4094,
                                     'probe_frequency_register': 0, 'probe_sample_period_T': 4096,
                                     'observable': 'Inactive wave sample-buffer refresh, not hidden index progression.'})
                c.write(0x25, 0x44)  # Keep primary-kernel routing stable.
                c.write(0x1a, 0)
                for address in range(16):
                    c.write(0x30 + address, initial)
                c.write(0x1b, 0xfc)  # Four length ticks establish the initial byte.
                c.write(0x1c, 0x20)
                c.write(0x1d, 1)
                c.write(0x1a, 0x80)
                c.write(0x1e, 0x40 if mode == 'never-triggered' else 0xc0)
                c.wait(64)
                c.read(0x26, 'NR52-after-initial-setup')
                c.wait(81920)  # At least five length clocks, whatever their phase.
                c.read(0x26, 'NR52-after-length-expiry')
                c.write(0x1a, 0 if mode == 'dac-off' else 0x80)
                for address in range(16):
                    c.write(0x30 + address, replacement)
                # Explicit RAM reads make data available on the wave bus;
                # 4094-T sample clocks walk across the 8-T read phases.
                c.emit('    ld hl, $ff3f', f'    REPT {gap // 8}', '        ld a, [hl]', '    ENDR')
                c.read(0x26, 'NR52-before-retrigger')
                c.write(0x1a, 0x80)  # Same write/timing in both treatments.
                c.write(0x1d, 0)
                c.write(0x1e, 0x80)  # Length off; does not clear the held byte.
                c.wait(64)
                c.read(0x26, 'NR52-after-retrigger')
                c.wait(8192)
                c.write(0x1c, 0)  # Known endpoint without a DAC/routing change.
                c.wait(FRAME)
                cases.append(c)

    # A retained upstream divider changes the first noise edges after trigger,
    # even though trigger clears the LFSR. Sweep across one 1536-T selected-bit
    # period, plus longer gaps; include both noise widths and DAC-off controls.
    for polynomial in (0x53, 0x5b):
        for mode in ('expired-dac-on', 'dac-off', 'active-muted'):
            for gap in (0, 256, 512, 768, 1024, 1280, 1536, 4096):
                c = Case(f'ch4-{polynomial:02x}-{mode}-{gap}', 'ch4-divider',
                         'Retrigger after a controlled silent interval; inspect initial noise-edge phase.',
                         parameters={'polynomial': polynomial, 'gap_T': gap, 'mode': mode,
                                     'selected_counter_period_T': 1536,
                                     'observable': 'Retained noise-divider phase, not hidden LFSR progression.'})
                c.write(0x25, 0 if mode == 'active-muted' else 0x88)
                c.write(0x20, 0x3f)
                c.write(0x21, 0xf0)
                c.write(0x22, polynomial)
                c.write(0x23, 0x80 if mode == 'active-muted' else 0xc0)
                c.wait(64)
                c.read(0x26, 'NR52-after-initial-trigger')
                c.wait(32768)
                c.read(0x26, 'NR52-after-silent-run')
                # Avoid a live NR42 rewrite in the DAC-on treatments: that
                # would introduce zombie-envelope behavior in the active control.
                if mode == 'dac-off':
                    c.write(0x21, 0)
                else:
                    c.wait(20)
                c.wait(gap)
                c.read(0x26, 'NR52-before-retrigger')
                if mode == 'active-muted':
                    c.write(0x25, 0x88)
                else:
                    c.wait(20)
                if mode == 'dac-off':
                    c.write(0x21, 0xf0)
                else:
                    c.wait(20)
                c.write(0x23, 0x80)
                c.wait(64)
                c.read(0x26, 'NR52-after-retrigger')
                c.wait(FRAME)
                cases.append(c)

    for c in cases:
        c.finish()
    return cases


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=ROOT / 'artifacts/ch34-clocks/v1')
    args = parser.parse_args()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    cases = probe_cases()
    table, assembly = [], []
    for i, c in enumerate(cases, 1):
        label = f'Case_{i:04x}'
        table.append(f'db BANK({label})\ndw {label}\ndw {i}')
        assembly += [f'SECTION "{c.name}", ROMX', label + ':', *c.lines]
    (out / 'case_table.inc').write_text('\n'.join(table) + '\ndb 0\n')
    (out / 'cases.inc').write_text('\n'.join(assembly) + '\n')
    rom = out / 'ch34-clock-probe.gb'
    commands = [
        ['rgbasm', '-I', str(HERE) + '/', '-I', str(out) + '/', '-o', str(out / 'engine.o'), str(HERE / 'engine.asm')],
        ['rgblink', '-m', str(out / 'ch34-clock-probe.map'), '-n', str(out / 'ch34-clock-probe.sym'), '-o', str(rom), str(out / 'engine.o')],
        ['rgbfix', '-v', '-p', '0', '-t', 'CH34 CLOCKS V1', '-m', '0x1b', '-r', '0x04', str(rom)],
    ]
    for command in commands:
        subprocess.run(command, check=True)
    symbols = (out / 'ch34-clock-probe.sym').read_text()
    positions = {}
    for bank, address, identifier in re.findall(r'^([0-9a-fA-F]+):([0-9a-fA-F]+) Case_([0-9a-fA-F]+)$', symbols, re.M):
        bank, address, identifier = int(bank, 16), int(address, 16), int(identifier, 16)
        assert 1 <= bank <= 255, 'Engine switches only the low MBC5 bank byte'
        positions[identifier] = {'bank': bank, 'address': address}
    assert len(positions) == len(cases)
    data = rom.read_bytes()
    assert data[0x147] == 0x1b and data[0x149] == 4
    assert data[0x14d] == (-sum(data[0x134:0x14d]) - 25) & 255
    assert int.from_bytes(data[0x14e:0x150], 'big') == (sum(data[:0x14e]) + sum(data[0x150:])) & 65535
    manifest = {
        'format': 'dmg-audio-torture-v1',
        'rom': str(rom), 'rom_sha256': digest(rom), 'rom_bytes': len(data),
        'behavior': 'Auto-start; LCD off; one pass; acoustic end packet then silence; no SRAM accesses.',
        'clock_T_hz': 4194304,
        'protocol': {'sync': [0xd3, 0x91], 'bit_order': 'MSB first', 'symbol_T': FRAME_STEP,
                     'zero_hz': 2048, 'one_hz': 4096, 'header': ['kind', 'id_lo', 'id_hi', 'payload_length'],
                     'checksum': 'XOR of header and payload; sync excluded',
                     'kinds': {'1': 'case marker', '2': 'readback', '3': 'suite end'},
                     'end_id': 65535, 'silence_each_side_T': FRAME},
        'markers': {'1': 'case packet', '2': 'stimulus start', '3': 'stimulus end',
                    '4': 'suite complete', '5': 'readback packet', '6': 'end packet'},
        'cases': [{'id': i, 'name': c.name, 'group': c.group, 'description': c.description,
                   'parameters': c.parameters, 'readback_labels': c.readback_labels,
                   'readback_length': len(c.readback_labels), **positions[i]} for i, c in enumerate(cases, 1)],
        'commands': commands,
        'source_sha256': {str(p.relative_to(ROOT)): digest(p) for p in
                          [Path(__file__).resolve(), HERE / 'engine.asm', HERE / 'macros.inc', HERE / 'cases.py']},
        'limits': ['One physical DMG does not establish other revisions or models.',
                   'CH3 tests inactive sample-buffer refresh, not progression of the hidden playback index.',
                   'CH4 tests retained divider phase, not progression of the hidden LFSR; retrigger clears it.',
                   'Unexpected CH3 contents or a changed never-triggered buffer require investigating the inactive RAM bus, not choosing an index hypothesis.',
                   'DAC-off is a distinct treatment, not an assumption that every background noise counter stops.',
                   'Short-gap CH3 cases can retain the old byte legitimately; long-gap and inverted patterns are the discriminator.',
                   'Readback packets verify NR52 channel activity; they do not expose the hidden waveform state.',
                   'No hardware pass threshold or silicon result is asserted before recording.'],
    }
    (out / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(f'{rom}\n{len(cases)} cases; {len(data)} bytes; SHA256 {digest(rom)}')


if __name__ == '__main__':
    main()
