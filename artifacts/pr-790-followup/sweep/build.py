#!/usr/bin/env python3
"""Rebuild and verify both frozen CH1 sweep variants, without altering evidence."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parent
INPUT = ROOT / 'inputs'
HASHES = {
    'sweep-verify.gb': 'cfb29be1b03c7cb6ebc76a63e2260455b745e09aea4966e26de8fafbf4217d09',
    'sweep-verify-mbc5.gb': '784825ebfa76fcb33f45aeb12275d34cfdf373a4e9f176f2bea6452e3d507b03',
}


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def check_roms(directory):
    images = []
    for name, expected in HASHES.items():
        path = directory / name
        data = path.read_bytes()
        assert digest(path) == expected, f'{path}: SHA-256 mismatch'
        assert len(data) == 32768
        header = b'\0\0\0' if name == 'sweep-verify.gb' else b'\x1b\x00\x04'
        assert data[0x147:0x14a] == header, f'{path}: cartridge/ROM/RAM header mismatch'
        assert data[0x134:0x144] == b'SWEEP COMPARE 1\0'
        assert data[0x14d] == (-sum(data[0x134:0x14d]) - 25) & 255
        assert int.from_bytes(data[0x14e:0x150], 'big') == (sum(data[:0x14e]) + sum(data[0x150:])) & 65535
        images.append(data)
    changes = [(i, a, b) for i, (a, b) in enumerate(zip(*images)) if a != b]
    assert changes == [(0x147, 0x00, 0x1b), (0x149, 0x00, 0x04), (0x14d, 0xeb, 0xcc)], changes
    return {name: digest(directory / name) for name in HASHES}


def build(OUT, tools):
    oracle = json.loads((INPUT / 'dmg-probe-v2-erekgdxk.json').read_text())
    assert digest(INPUT / 'dmg-probe-v2-erekgdxk.json') == 'e2251d1ccdd6cf9558ea95a9da574d6d60b973f541708b67daff8b6a00786277'
    manifest = json.loads((INPUT / 'sweep-manifest.json').read_text())
    frozen = (INPUT / 'sweep-cases.inc').read_text()
    assert digest(INPUT / 'sweep-cases.inc') == '59195950833d763cea6d3cb9682fe334b564c876af11e70bcfe92fdc36185cff'
    cases = [c for c in manifest['cases'] if c['group'] == 'ch1-sweep-hazard']
    domains, reference, counts = [], bytearray(), []
    for shift in range(1, 8):
        domain = [f for f in range(2048) if (n := f + (f >> shift)) + (n >> shift) < 2048]
        assert domain == list(range(len(domain)))
        domains.append(len(domain))
        stops = oracle['sweep']['killed_old_frequencies_by_shift'][str(shift)]
        assert len(set(stops)) == len(stops) and set(stops) <= set(domain)
        counts.append(len(stops))
        bits = bytearray(256)
        for frequency in stops:
            bits[frequency // 8] |= 1 << (frequency % 8)
        reference += bits
    assert domains == [911, 1312, 1619, 1815, 1926, 1986, 2018]
    assert counts == [127, 54, 7, 0, 2, 1, 1] and len(cases) == 13
    (OUT / 'reference.bin').write_bytes(reference)
    table, assembly, coverage = ['BatchTable:'], [], set()
    for case in cases:
        p = case['parameters']
        shift, first, end = p['shift'], p['first_frequency'], p['end_frequency_exclusive']
        assert first % 4 == 0
        for f in range(first, end):
            assert (shift, f) not in coverage
            coverage.add((shift, f))
        label = f"Case_{case['id']:04x}"
        # Reuse the complete measured batch verbatim, including both waits,
        # reset sequence, read packing, loop overhead, and final padding.
        body = re.search(rf'^{label}:\n(.*?)(?=^SECTION |\Z)', frozen, re.M | re.S).group(0).rstrip()
        assembly += [body]
        table += [f'    dw {label}, wObserved + {(shift - 1) * 512 + first // 4}']
    assert coverage == {(s, f) for s in range(1, 8) for f in range(domains[s - 1])}
    (OUT / 'batches.inc').write_text('\n\n'.join(assembly) + '\n')
    (OUT / 'tables.inc').write_text('\n'.join(table + ['    dw 0', 'Limits:',
        '    dw ' + ', '.join(map(str, domains)), 'ReferenceCounts:',
        '    dw ' + ', '.join(map(str, counts))]) + '\n')

    # Existing SameBoy SDL font; no new graphics dependency or font licence.
    source = (INPUT / 'font.c').read_text().split('uint8_t font[] = {', 1)[1].split('};', 1)[0]
    source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
    pixels = re.findall(r'\b[X_]\b', source)
    assert len(pixels) % 48 == 0 and len(pixels) >= 96 * 48
    font = bytearray()
    for row in range(96 * 8):
        bits = sum((pixels[row * 6 + x] == 'X') << (6 - x) for x in range(6))
        font += bytes([bits, bits])
    (OUT / 'font.bin').write_bytes(font)

    screens = {
        'IntroScreen': {
            0: 'SWEEP COMPARE V1', 2: 'CH1 FIRST UPDATE', 4: '11587 TRIALS',
            5: 'ABOUT 100 SECONDS', 7: 'LCD OFF DURING TEST', 8: 'NO SOUND / NO SAVE',
            9: 'RESULTS STAY ON', 12: "COMPARES JEFF'S DMG", 13: 'NOT UNIVERSAL PASS', 16: 'START: RUN'},
        'SummaryScreen': {
            0: 'SWEEP COMPARE V1', 1: "VS JEFF'S DMG", 3: 'S STOP REF DIFF  BAD',
            12: 'TRIALS 11587', 14: 'NOT A HARDWARE PASS',
            16: '< > SHIFT DETAILS', 17: 'POWER OFF TO RESTART'},
        'DetailScreen': {
            0: 'SWEEP COMPARE V1', 1: "SHIFT   / JEFF'S DMG", 2: 'TRIALS',
            3: 'STOP      REF', 4: 'DIFF      BAD', 6: 'DIFFERING FREQUENCY',
            14: 'NOT A HARDWARE PASS', 16: 'UP/DOWN: PREV/NEXT', 17: '< > SHIFT  START SUM'},
    }
    text = []
    for label, lines in screens.items():
        data = bytearray()
        for row in range(18):
            line = lines.get(row, '')
            assert len(line) <= 20, line
            data += bytes(ord(c) - 32 for c in line.ljust(32))
        path = OUT / f'{label}.bin'
        path.write_bytes(data)
        text += [label + ':', f'    INCBIN "{path.name}"']
    (OUT / 'screens.inc').write_text('\n'.join(text) + '\n')
    rom = OUT / 'sweep-verify.gb'
    commands = [
        [tools['rgbasm'], '-I', str(ROOT / 'src') + '/', '-I', str(OUT) + '/', '-o', str(OUT / 'sweep.o'), str(ROOT / 'src/sweep.asm')],
        [tools['rgblink'], '-m', str(OUT / 'sweep-verify.map'), '-n', str(OUT / 'sweep-verify.sym'), '-o', str(rom), str(OUT / 'sweep.o')],
        [tools['rgbfix'], '-v', '-p', '0', '-t', 'SWEEP COMPARE 1', '-m', '0', '-r', '0', str(rom)],
    ]
    for command in commands:
        subprocess.run(command, check=True)
    variant = OUT / 'sweep-verify-mbc5.gb'
    shutil.copyfile(rom, variant)
    subprocess.run([tools['rgbfix'], '-v', '-m', '0x1b', '-r', '0x04', str(variant)], check=True)
    return check_roms(OUT)


def main():
    if not __debug__:
        raise SystemExit('Checks require assertions: do not use python -O or PYTHONOPTIMIZE.')
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rgbds', type=Path, help='directory containing RGBDS 1.0.0 executables (otherwise PATH)')
    parser.add_argument('--out', type=Path, default=ROOT / 'rebuilt', help='new output directory; evidence is never overwritten')
    parser.add_argument('--check-only', action='store_true', help='verify bundled sources and both ROMs without RGBDS')
    args = parser.parse_args()
    frozen = json.loads((ROOT / 'archive/build-manifest.json').read_text())
    for name, expected in frozen['sources'].items():
        if name != 'build.py':  # This wrapper was adapted; assembly and inputs were not.
            assert digest(ROOT / name) == expected, f'{name}: frozen input changed'
    check_roms(ROOT / 'rom')
    if args.check_only:
        print('PASS: frozen inputs, both ROM hashes, headers, checksums, exactly three header-byte differences')
        return
    tools = {name: str(args.rgbds.resolve() / name) if args.rgbds else name
             for name in ('rgbasm', 'rgblink', 'rgbfix')}
    versions = {name: subprocess.check_output([tool, '--version'], text=True).strip()
                for name, tool in tools.items()}
    for name, version in versions.items():
        assert version == f'{name} v1.0.0', f'Expected RGBDS 1.0.0, got {version}'
    output = args.out.resolve()
    output.mkdir(parents=True, exist_ok=False)
    with tempfile.TemporaryDirectory(prefix='.work-', dir=output) as temporary:
        work = Path(temporary)
        hashes = build(work, tools)
        for name in (*HASHES, 'sweep-verify.map', 'sweep-verify.sym'):
            shutil.copyfile(work / name, output / name)
    report = {'all_passed': True, 'rom_sha256': hashes, 'rgbds': versions,
              'header_changes': {'0x147': ['00', '1b'], '0x149': ['00', '04'], '0x14d': ['eb', 'cc']},
              'all_other_bytes_identical': True, 'frozen_sources_checked': True,
              'build_script_sha256': digest(Path(__file__))}
    (output / 'build-checks.json').write_text(json.dumps(report, indent=2) + '\n')
    for name, sha in hashes.items():
        print(f'{name}: 32768 bytes; SHA256 {sha}')
    print('PASS: both rebuilt variants, headers/checksums, exactly three header-byte differences; measurement bytes unchanged')


if __name__ == '__main__':
    main()
