#!/usr/bin/env python3
"""Check measured stops and survivors at the rejected transport-model boundaries.

Every case runs real CPU instructions. Expectations come from the committed
physical oracle, independently of the implementation's complete transition map.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]
FIXTURE = ROOT / 'benchmark/sweep-circuit/research'


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sameboy', type=Path, default=ROOT / 'SameBoy')
    parser.add_argument('--out', type=Path, help='New output directory; otherwise a fresh build directory')
    args = parser.parse_args()
    source = args.sameboy.resolve()
    oracle_path = ROOT / 'benchmark/oracles/dmg-probe-v2-erekgdxk.json'
    oracle = json.loads(oracle_path.read_text())
    if not oracle['sweep']['all_initial_active']:
        raise ValueError('Expected packed bits require the measured initial-active controls')
    killed = {int(shift): set(values)
              for shift, values in oracle['sweep']['killed_old_frequencies_by_shift'].items()}
    if args.out:
        out = args.out.resolve()
        out.mkdir(parents=True, exist_ok=False)
    else:
        out = Path(tempfile.mkdtemp(prefix='sweep-map-', dir=ROOT / 'build'))

    runner = out / 'readback-test'
    library = source / 'build/lib/libsameboy.a'
    commands = [
        ['make', '-C', str(source), 'CONF=release', 'build/lib/libsameboy.a'],
        ['cc', '-std=gnu11', '-O2', '-Wall', '-Wextra', '-Werror', '-Wno-multichar',
         '-I', str(source), str(FIXTURE / 'readback-test.c'), str(library),
         '-lm', '-ldl', '-o', str(runner)],
    ]
    with (out / 'build.log').open('w') as log:
        for command in commands:
            subprocess.run(command, check=True, stdout=log, stderr=subprocess.STDOUT)

    # All four false stops from NAND transport, their measured neighboring
    # stops, and the first missing stop from the original inertial model.
    cases = [(2, 0x4cd), (2, 0x51a), (2, 0x51b), (2, 0x51c),
             (2, 0x51d), (2, 0x51e), (2, 0x51f), (3, 0x64f)]
    template = (FIXTURE / 'minimal-4cd.asm').read_text()
    if template.count('ld de, $04cd') != 1 or template.count('write_reg $ff10, $12') != 1:
        raise ValueError('The minimal probe template changed its parameterization')
    results = []
    for shift, frequency in cases:
        total = frequency + (frequency >> shift)
        if not (total < 2048 and total + (total >> shift) < 2048):
            raise ValueError('Requested boundary is outside the physically measured domain')
        name = f'shift-{shift}-old-{frequency:03x}'
        asm, rom = out / f'{name}.asm', out / f'{name}.gb'
        boot = out / f'{name}.boot'
        text = template.replace('ld de, $04cd', f'ld de, ${frequency:04x}')
        text = text.replace('write_reg $ff10, $12', f'write_reg $ff10, ${0x10 | shift:02x}')
        asm.write_text(text)
        build_commands = [
            ['rgbasm', '-I', str(ROOT / 'benchmark/torture') + '/',
             '-o', str(asm.with_suffix('.o')), str(asm)],
            ['rgblink', '-p', '0', '-o', str(rom), str(asm.with_suffix('.o'))],
            ['rgbfix', '-v', '-p', '0', '-t', 'SWEEPMAP', '-m', '0', str(rom)],
        ]
        for command in build_commands:
            subprocess.run(command, check=True)
        commands.extend(build_commands)
        boot.write_bytes(rom.read_bytes()[:256])
        expected = 2 | int(frequency not in killed[shift])
        command = [str(runner), str(rom), str(boot), str(expected)]
        commands.append(command)
        started = time.monotonic()
        execution = subprocess.run(command, capture_output=True, text=True)
        elapsed = time.monotonic() - started
        (out / f'{name}.log').write_text(execution.stdout + execution.stderr)
        match = re.search(r'packed pre/post=([0-9a-fA-F]+)', execution.stdout)
        actual = int(match[1], 16) if match else None
        passed = execution.returncode == 0 and actual == expected
        results.append({'name': name, 'shift': shift, 'old_frequency': frequency,
                        'expected_packed_bits': expected, 'actual_packed_bits': actual,
                        'passed': passed, 'exit_code': execution.returncode,
                        'rom_sha256': digest(rom), 'assembly_sha256': digest(asm),
                        'boot_sha256': digest(boot), 'execution_seconds': elapsed})
        observed = f'{actual:02b}' if actual is not None else 'missing'
        print(f'{name}: expected={expected:02b} observed={observed} '
              f'{"PASS" if passed else "FAIL"}', flush=True)

    git = lambda *a: subprocess.check_output(['git', '-C', str(source), *a], text=True)
    table = source / 'Core/dmg_sweep.h'
    report = {
        'status': 'passed' if all(r['passed'] for r in results) else 'failed',
        'sameboy_commit': git('rev-parse', 'HEAD').strip(),
        'sameboy_diff_sha256': hashlib.sha256(git('diff', 'HEAD').encode()).hexdigest(),
        'apu_source_sha256': digest(source / 'Core/apu.c'),
        'sweep_table_sha256': digest(table) if table.exists() else None,
        'library_sha256': digest(library), 'oracle_sha256': digest(oracle_path),
        'physical_wav_sha256': oracle['hardware_wav']['sha256'],
        'source_sha256': {str(path.relative_to(ROOT)): digest(path) for path in
                          [Path(__file__).resolve(), FIXTURE / 'minimal-4cd.asm',
                           FIXTURE / 'readback-test.c', ROOT / 'benchmark/torture/macros.inc']},
        'expectation_source': 'Committed physical oracle, never the emulator transition table',
        'completion': 'Each finite ROM writes FF82=4 after both NR52 observations',
        'checked_cases': len(results), 'failed_cases': sum(not r['passed'] for r in results),
        'commands': commands, 'results': results,
    }
    (out / 'results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'{report["status"]}: {report["checked_cases"]} cases, '
          f'{report["failed_cases"]} failed; {out / "results.json"}')
    return bool(report['failed_cases'])


if __name__ == '__main__':
    raise SystemExit(main())
