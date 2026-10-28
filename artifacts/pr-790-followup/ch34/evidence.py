#!/usr/bin/env python3
"""Rebuild ROMs, decode recordings, and check a supplied SameBoy source tree."""
import argparse
import hashlib
import json
import math
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parent
FOCUSED = ROOT / 'ch34-focused-probe'
INITIAL = ROOT / 'ch34-clock-probe'
DATA = ROOT / 'ch34-focused-hardware-ew_png0d'
TESTS = ROOT / 'ch34-focused-fixes/tests'


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def compare_calibration(measured, original):
    if isinstance(original, dict):
        assert measured.keys() == original.keys()
        return max((compare_calibration(measured[k], original[k]) for k in original), default=0)
    if isinstance(original, list):
        assert len(measured) == len(original)
        return max((compare_calibration(a, b) for a, b in zip(measured, original)), default=0)
    if isinstance(original, float):
        # BLAS reduction order can change the last digits (observed differences < 1e-16).
        assert math.isclose(measured, original, rel_tol=1e-12, abs_tol=1e-12), (measured, original)
        return abs(measured - original)
    assert measured == original, (measured, original)
    return 0


def run(command, log, cwd=ROOT):
    environment = dict(os.environ, PYTHONDONTWRITEBYTECODE='1', OPENBLAS_NUM_THREADS='1')
    environment.pop('CH34_MODEL', None)
    environment.pop('CH34_ROUNDTRIP', None)
    with log.open('w') as stream:
        stream.write(' '.join(map(str, command)) + '\n')
        stream.flush()
        subprocess.run(list(map(str, command)), cwd=cwd, env=environment,
                       stdout=stream, stderr=subprocess.STDOUT, check=True)
    print(f'PASS: {log.name}', flush=True)


def verify():
    entries = (ROOT / 'SHA256SUMS').read_text().splitlines()
    for line in entries:
        expected, name = line.split('  ', 1)
        path = (ROOT / name).resolve()
        assert ROOT in path.parents and digest(path) == expected, name
    print(f'PASS: {len(entries)} packaged file hashes')


def rebuild(out):
    for kit, builder in ((INITIAL, 'ch34-clock-probe.py'), (FOCUSED, 'build.py')):
        target = out / kit.name
        run([sys.executable, kit / 'benchmark' / builder, '--out', target],
            out / f'{kit.name}.log')
        for name in (f'{kit.name}.gb', 'cases.inc', 'case_table.inc'):
            assert (target / name).read_bytes() == (kit / 'rom' / name).read_bytes(), name
        assert json.loads((target / 'manifest.json').read_text())['cases'] == json.loads(
            (kit / 'rom/manifest.json').read_text())['cases']
    print('PASS: both ROMs and generated measurement bodies are byte-identical')


def decode(out):
    results = []
    for take, kit in (('xryrmxjl', INITIAL), ('qw3uz2uv', INITIAL), ('ew_png0d', FOCUSED)):
        archived = DATA if kit == FOCUSED else kit / f'hardware-{take}'
        for channel, folder in enumerate(('reference-comparison', 'right-channel-check')):
            target = out / f'{take}-{channel}'
            run([sys.executable, kit / 'benchmark/torture/analyze.py',
                 '--reference', kit / 'reference/audio.wav', '--markers', kit / 'reference/markers.csv',
                 '--manifest', kit / 'rom/manifest.json', '--hardware', ROOT / 'hardware' / f'{take}.wav',
                 '--channel', channel, '--out', target], out / f'{take}-{channel}.log')
            report = json.loads((target / 'metrics.json').read_text())
            original = json.loads((archived / folder / 'metrics.json').read_text())
            assert report['complete_hardware'] and report['packet_coverage']['complete']
            assert report['hardware_packets'] == original['hardware_packets']
            calibration_delta = compare_calibration(report['alignment'], original['alignment'])
            assert [c['readback'] for c in report['cases']] == [c['readback'] for c in original['cases']]
            results.append({'take': take, 'channel': channel, 'coverage': report['packet_coverage'],
                            'calibration_max_absolute_delta': calibration_delta})
    (out / 'summary.json').write_text(json.dumps(results, indent=2) + '\n')
    print('PASS: all three raw recordings, both channels; archived calibration and readbacks reproduced')


def measure(out):
    for name, script in (('ch3-analysis', 'analyze_wave.py'), ('ch4-analysis', 'analyze_ch4.py')):
        run([sys.executable, DATA / name / script, '--out', out / name], out / f'{name}.log')
    wave = json.loads((out / 'ch3-analysis/results.json').read_text())
    original_wave = json.loads((DATA / 'ch3-analysis/results.json').read_text())
    # Only the relocated WAV path changes; all measurement/calibration values must match.
    for report in (wave, original_wave):
        for channel in report['recording_quality']:
            channel['metadata'].pop('path')
    assert wave == original_wave, 'CH3 measurements'
    for name in ('measurements.json', 'sensitivity.json'):
        measured = json.loads((out / 'ch4-analysis' / name).read_text())
        original = json.loads((DATA / 'ch4-analysis' / name).read_text())
        if name == 'measurements.json':
            # Input hashes are verified separately; relocation/adapted script changes provenance.
            measured.pop('sha256')
            original.pop('sha256')
        assert measured == original, name
    print('PASS: focused CH3/CH4 measurements and all 14 sensitivity variants reproduced')


def regress(out, source, cc):
    source = source.resolve()
    library = out / 'lib/libsameboy.a'
    run(['make', '-j2', f'CC={cc}', 'CONF=release', f'OBJ={out / "obj"}',
         f'LIBDIR={library.parent}', library], out / 'build.log', cwd=source)
    common = [cc, '-std=gnu11', '-O2', '-Wno-multichar', f'-I{source}']
    run([*common, TESTS / 'capture-model.c', library, '-lm', '-ldl', '-o', out / 'capture'],
        out / 'capture-build.log')
    for kit, folder, regression in ((FOCUSED, 'focused', 'recording_regression.py'),
                                    (INITIAL, 'initial', 'previous_recordings.py')):
        target = out / folder
        target.mkdir()
        run([out / 'capture', '--rom', kit / 'rom' / f'{kit.name}.gb',
             '--boot', ROOT / 'boot/dmg_boot.bin', '--until-done', '--seed', '1',
             '--wav', target / 'audio.wav', '--screenshot', target / 'screen.ppm',
             '--apu-log', target / 'apu.csv', '--markers-log', target / 'markers.csv'],
            out / f'{folder}-render.log')
        run([sys.executable, TESTS / regression, target], out / f'{folder}-regression.log')
    for name in ('mixer-dac', 'noise-feedback', 'pulse-clock', 'pulse-duty',
                 'dac-edge', 'dac-reconnect', 'dac-timing', 'pending-wave'):
        test = TESTS / 'pending-wave.c' if name == 'pending-wave' else ROOT / 'companion-tests' / f'{name}-test.c'
        run([*common, '-DPULSE_CLOCK_STATE', '-DPULSE_DUTY_STATE', test, library,
             '-lm', '-ldl', '-o', out / name], out / f'{name}-build.log')
        needs_rom = name in ('noise-feedback', 'pulse-clock', 'pulse-duty', 'pending-wave')
        run([out / name, *([FOCUSED / 'rom/ch34-focused-probe.gb'] if needs_rom else [])],
            out / f'{name}.log')
    provenance = {'compiler': subprocess.check_output([cc, '--version'], text=True),
                  'source': str(source), 'library_sha256': digest(library),
                  'core_sha256': {str(p.relative_to(source)): digest(p)
                                  for p in sorted((source / 'Core').iterdir()) if p.is_file()}}
    (out / 'build-provenance.json').write_text(json.dumps(provenance, indent=2) + '\n')
    print('PASS: three-recording regression, seven companion tests, pending-wave boundary check')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=('verify', 'rebuild', 'decode', 'measure', 'regress'))
    parser.add_argument('--out', type=Path, help='New directory outside this package')
    parser.add_argument('--source', type=Path, help='SameBoy source checkout; only needed by regress')
    parser.add_argument('--cc', default='clang', help='Compiler for regress; clang or gcc')
    args = parser.parse_args()
    if args.command == 'verify':
        verify()
        return
    if args.out is None:
        parser.error('--out is required')
    if args.command == 'regress' and args.source is None:
        parser.error('--source is required for regress')
    out = args.out.resolve()
    if out == ROOT or ROOT in out.parents:
        parser.error('--out must be outside the evidence package')
    out.mkdir(parents=True, exist_ok=False)
    if args.command == 'regress':
        regress(out, args.source, args.cc)
    else:
        {'rebuild': rebuild, 'decode': decode, 'measure': measure}[args.command](out)


if __name__ == '__main__':
    main()
