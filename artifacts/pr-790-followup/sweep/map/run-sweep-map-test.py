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
import shlex
import subprocess
import sys
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]
FIXTURE = ROOT / 'map/fixtures'


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    if not __debug__:
        raise SystemExit('Do not disable assertions with python -O or PYTHONOPTIMIZE.')
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sameboy', '--core', dest='sameboy', type=Path, required=True,
                        help='SameBoy checkout (read-only); .inc and legacy .h are supported')
    parser.add_argument('--out', type=Path, required=True, help='New output directory')
    parser.add_argument('--rgbds', type=Path, help='RGBDS 1.0.0 executable directory; otherwise PATH')
    parser.add_argument('--cc', default='cc', help='GCC/Clang-compatible C compiler executable')
    args = parser.parse_args()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=False)
    with tempfile.TemporaryDirectory(prefix='.work-', dir=out) as temporary:
        return run(args, out, Path(temporary))


def run(args, out, work):
    source = args.sameboy.resolve()
    oracle_path = ROOT / 'inputs/dmg-probe-v2-erekgdxk.json'
    oracle = json.loads(oracle_path.read_text())
    if not oracle['sweep']['all_initial_active']:
        raise ValueError('Expected packed bits require the measured initial-active controls')
    killed = {int(shift): set(values)
              for shift, values in oracle['sweep']['killed_old_frequencies_by_shift'].items()}
    table = source / 'Core/dmg_sweep.inc'
    if not table.is_file():
        table = source / 'Core/dmg_sweep.h'  # Earlier publication heads.
    tools = {name: str(args.rgbds.resolve() / name) if args.rgbds else name
             for name in ('rgbasm', 'rgblink', 'rgbfix')}
    versions = {name: subprocess.check_output([tool, '--version'], text=True).strip()
                for name, tool in tools.items()}
    for name, version in versions.items():
        if version != f'{name} v1.0.0':
            raise ValueError(f'Expected RGBDS 1.0.0, got {version}')
    core_hashes = {str(path.relative_to(source)): digest(path)
                   for path in sorted((source / 'Core').rglob('*')) if path.is_file()}
    runner = work / 'readback-test'
    commands = [
        [sys.executable, str(ROOT / 'map/generate-sweep-table.py'), '--check', str(table)],
        [args.cc, '-std=gnu11', '-O2', '-D_GNU_SOURCE', '-DGB_INTERNAL', '-Wno-multichar',
         '-DGB_VERSION="sweep-map-check"', '-I', str(source), str(FIXTURE / 'readback-test.c'),
         *map(str, sorted((source / 'Core').glob('*.c'))), '-lm', '-o', str(runner)],
    ]
    with (out / 'build.log').open('w') as log:
        for command in commands:
            log.write(shlex.join(command) + '\n')
            log.flush()
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
        asm, rom = out / f'{name}.asm', work / f'{name}.gb'
        boot = work / f'{name}.boot'
        obj = work / f'{name}.o'
        text = template.replace('ld de, $04cd', f'ld de, ${frequency:04x}')
        text = text.replace('write_reg $ff10, $12', f'write_reg $ff10, ${0x10 | shift:02x}')
        asm.write_text(text)
        build_commands = [
            [tools['rgbasm'], '-I', str(ROOT / 'src') + '/', '-o', str(obj), str(asm)],
            [tools['rgblink'], '-p', '0', '-o', str(rom), str(obj)],
            [tools['rgbfix'], '-v', '-p', '0', '-t', 'SWEEPMAP', '-m', '0', str(rom)],
        ]
        for command in build_commands:
            subprocess.run(command, check=True)
        commands.extend(build_commands)
        boot.write_bytes(rom.read_bytes()[:256])
        expected = 2 | int(frequency not in killed[shift])
        command = [str(runner), str(rom), str(boot), str(expected)]
        commands.append(command)
        started = time.monotonic()
        execution = subprocess.run(command, capture_output=True, text=True, timeout=30)
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

    for name, before in core_hashes.items():
        assert digest(source / name) == before, f'{name} changed during validation'
    git = subprocess.run(['git', '-C', str(source), 'rev-parse', 'HEAD'], capture_output=True, text=True)
    report = {
        'status': 'passed' if all(r['passed'] for r in results) else 'failed',
        'sameboy_commit': git.stdout.strip() if git.returncode == 0 else None,
        'core_sources_sha256': core_hashes,
        'apu_source_sha256': digest(source / 'Core/apu.c'),
        'sweep_table_path': str(table.relative_to(source)),
        'sweep_table_sha256': digest(table), 'sweep_table_reproduced_byte_for_byte': True,
        'runner_sha256': digest(runner), 'oracle_sha256': digest(oracle_path),
        'rgbds': versions,
        'compiler': subprocess.check_output([args.cc, '--version'], text=True).splitlines()[0],
        'physical_wav_sha256': oracle['hardware_wav']['sha256'],
        'source_sha256': {str(path.relative_to(ROOT)): digest(path) for path in
                          [Path(__file__).resolve(), FIXTURE / 'minimal-4cd.asm',
                           FIXTURE / 'readback-test.c', ROOT / 'src/macros.inc',
                           ROOT / 'map/generate-sweep-table.py']},
        'expectation_source': 'Committed physical oracle, never the emulator transition table',
        'physical_hardware_exercised_by_this_check': False,
        'completion': 'Each finite ROM writes FF82=4 after both NR52 observations',
        'checked_cases': len(results), 'failed_cases': sum(not r['passed'] for r in results),
        'path_note': 'Absolute command/log paths are execution provenance, not portable dependencies; temporary binaries are deleted.',
        'commands': commands, 'results': results,
    }
    (out / 'results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'{report["status"]}: {report["checked_cases"]} cases, '
          f'{report["failed_cases"]} failed; {out / "results.json"}')
    return bool(report['failed_cases'])


if __name__ == '__main__':
    raise SystemExit(main())
