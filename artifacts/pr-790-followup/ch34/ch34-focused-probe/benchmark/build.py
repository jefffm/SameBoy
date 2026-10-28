#!/usr/bin/env python3
"""Build the focused CH3 retained-sample / CH4 DAC-phase follow-up ROM."""
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
from wave_cases import wave_cases
from noise_cases import noise_cases


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=ROOT / 'rom')
    args = parser.parse_args()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    calibration = Case('calibration-pulse', 'calibration', 'CH2 256 Hz, 50% duty, volume 8.')
    calibration.pulse(2, volume=0x88)
    calibration.wait(8 * FRAME)
    cases = [calibration, *wave_cases(), *noise_cases()]
    assert len(cases) == 117 and len({c.name for c in cases}) == len(cases)
    table, assembly = [], []
    for i, c in enumerate(cases, 1):
        assert len(c.readback_labels) == (0 if i == 1 else 4)
        c.finish()
        label = f'Case_{i:04x}'
        table.append(f'db BANK({label})\ndw {label}\ndw {i}')
        assembly += [f'SECTION "{c.name}", ROMX', label + ':', *c.lines]
    (out / 'case_table.inc').write_text('\n'.join(table) + '\ndb 0\n')
    (out / 'cases.inc').write_text('\n'.join(assembly) + '\n')
    rom = out / 'ch34-focused-probe.gb'
    commands = [
        ['rgbasm', '-I', str(HERE) + '/', '-I', str(out) + '/', '-o', str(out / 'engine.o'), str(HERE / 'engine.asm')],
        ['rgblink', '-m', str(out / 'ch34-focused-probe.map'), '-n', str(out / 'ch34-focused-probe.sym'), '-o', str(rom), str(out / 'engine.o')],
        ['rgbfix', '-v', '-p', '0', '-t', 'CH34 FOCUSED V1', '-m', '0x1b', '-r', '0x04', str(rom)],
    ]
    for command in commands:
        subprocess.run(command, check=True)
    symbols = (out / 'ch34-focused-probe.sym').read_text()
    positions = {}
    for bank, address, identifier in re.findall(r'^([0-9a-fA-F]+):([0-9a-fA-F]+) Case_([0-9a-fA-F]+)$', symbols, re.M):
        bank, address, identifier = int(bank, 16), int(address, 16), int(identifier, 16)
        assert 1 <= bank <= 255, 'Engine switches only the low MBC5 bank byte'
        positions[identifier] = {'bank': bank, 'address': address}
    assert len(positions) == len(cases)
    data = rom.read_bytes()
    assert data[0x147] == 0x1b and data[0x149] == 4
    assert len(data) == 32768 << data[0x148]
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
                          [Path(__file__).resolve(), ROOT / 'benchmark/wave_cases.py',
                           ROOT / 'benchmark/noise_cases.py', HERE / 'engine.asm',
                           HERE / 'macros.inc', HERE / 'cases.py']},
        'limits': [
            'No hardware result or universal pass/fail threshold is asserted before recording.',
            'Activity expectations validate setup; they do not reveal hidden waveform state.',
            'CH3 measures retained sample state, not hidden index progression.',
            'CH3 expiry polling is bounded (4096 iterations, 56 T each). A nonzero expiry activity bit invalidates that case.',
            'CH3 post-expiry waits start after the successful NR52 observation and storage, not exactly at length expiry.',
            'CH3 zero-wait probe has a 104-T successful-read-to-trigger interval plus up to 56 T of poll detection delay; earlier changes are not localized to expiry.',
            'The first post-trigger sample is a proxy: a trigger-dependent sample change can resemble an inactive-history change.',
            'No-wave-access and idle controls still perform CPU bus transactions; they are not electrically neutral.',
            'Never-triggered controls use a fixed dwell, not an exactly matched expiry schedule.',
            'CH4 measures surviving divider phase, not hidden LFSR progression; trigger resets the LFSR.',
            'CH4 timing axes exclude fixed instruction overhead. Disable-to-enable is D+20 T; enable-to-trigger is R+48 T.',
            'A reset at disable that freezes until enable can be equivalent to a reset at enable.',
            'Analog DAC transients can affect onset estimates; compare multiple edges and timing-matched controls.',
            'One physical unit does not establish other revisions or models.',
        ],
    }
    (out / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(f'{rom}\n{len(cases)} cases; {len(data)} bytes; SHA256 {digest(rom)}')


if __name__ == '__main__':
    main()
