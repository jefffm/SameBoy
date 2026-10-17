#!/usr/bin/env python3
"""Run the physical v2 wave stimuli through SameBoy's CPU and compare WRAM results.

Each generated ROM contains one unchanged probe case. Expected bytes come from
the committed acoustic readbacks, never from a corruption formula.
"""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parent.parent
ENGINE = ROOT / 'benchmark/torture'
ORACLE = ROOT / 'benchmark/oracles/dmg-probe-v2-erekgdxk.json'


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def load_probe():
    spec = importlib.util.spec_from_file_location('wave_probe', ROOT / 'benchmark/wave-retrigger-probe.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return {case.name: case for case in module.probe_cases()[1:] + module.additional_probe_cases()}


def build_case(case, case_id, out):
    out.mkdir(parents=True, exist_ok=True)
    label = f'Case_{case_id:04x}'
    (out / 'case_table.inc').write_text(f'db BANK({label})\ndw {label}\ndw {case_id}\ndb 0\n')
    (out / 'cases.inc').write_text('\n'.join([f'SECTION "{case.name}", ROMX', label + ':', *case.lines]) + '\n')
    rom = out / 'wave-probe.gb'
    for command in [
        ['rgbasm', '-I', str(ENGINE) + '/', '-I', str(out) + '/', '-o', str(out / 'engine.o'), str(ENGINE / 'engine.asm')],
        ['rgblink', '-n', str(out / 'wave-probe.sym'), '-o', str(rom), str(out / 'engine.o')],
        ['rgbfix', '-v', '-p', '0', '-t', 'WAVE RAM REPRO', '-m', '0x1b', '-r', '0x04', str(rom)],
    ]:
        subprocess.run(command, check=True)
    symbols = dict((name, address) for address, name in re.findall(
        r'^00:([0-9a-fA-F]+) (wReadback(?:Size)?)$', (out / 'wave-probe.sym').read_text(), re.M))
    return rom, symbols


def build_minimal(out):
    rom = out / 'wave-majority-repro.gb'
    for command in [
        ['rgbasm', '-o', str(out / 'minimal.o'), str(ROOT / 'benchmark/wave-majority-repro.asm')],
        ['rgblink', '-n', str(out / 'minimal.sym'), '-o', str(rom), str(out / 'minimal.o')],
        ['rgbfix', '-v', '-p', '0', '-t', 'WAVE MAJORITY', str(rom)],
    ]:
        subprocess.run(command, check=True)
    symbols = dict((name, address) for address, name in re.findall(
        r'^00:([0-9a-fA-F]+) (wReadback(?:Size)?)$', (out / 'minimal.sym').read_text(), re.M))
    return rom, symbols


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sameboy', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    scope = parser.add_mutually_exclusive_group()
    scope.add_argument('--case', help='One exact case name for a standalone repro; default is all 17 wave cases')
    scope.add_argument('--minimal', action='store_true', help='Reduced ROM for physical case 35, with no acoustic packet engine')
    args = parser.parse_args()
    source, out = args.sameboy.resolve(), args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    oracle = json.loads(ORACLE.read_text())
    stimuli = load_probe()
    cases = [case for case in oracle['wave']['cases']
             if (case['case'] == 35 if args.minimal else not args.case or case['name'] == args.case)]
    if not cases:
        parser.error('No physical wave case has that name')
    subprocess.run(['make', '-C', str(source), 'CONF=release', 'build/lib/libsameboy.a', 'bootroms'],
                   check=True, stdout=subprocess.DEVNULL)
    exe = out / 'readback'
    subprocess.run(['cc', '-std=gnu11', '-O2', '-Wall', '-Wextra', '-Werror', '-I', str(source),
                    str(ROOT / 'benchmark/wave-readback.c'), str(source / 'build/lib/libsameboy.a'),
                    '-lm', '-ldl', '-o', str(exe)], check=True)
    results = []
    for case in cases:
        stimulus = stimuli[case['name']]
        if stimulus.parameters != case['parameters']:
            raise ValueError(f"Stimulus parameters differ from the physical oracle: {case['name']}")
        rom, symbols = build_minimal(out) if args.minimal else build_case(stimulus, case['case'], out / case['name'])
        observed = json.loads(subprocess.check_output([
            str(exe), str(rom), str(source / 'build/bin/BootROMs/dmg_boot.bin'),
            symbols['wReadbackSize'], symbols['wReadback'],
        ], text=True).splitlines()[-1])
        expected = case['readback']
        differences = [{'offset': i, 'hardware': a, 'sameboy': b}
                       for i, (a, b) in enumerate(zip(expected, observed)) if a != b]
        results.append({'id': case['case'], 'name': case['name'], 'rom_sha256': digest(rom),
                        'stimulus_sha256': digest(ROOT / 'benchmark/wave-majority-repro.asm') if args.minimal else
                                           hashlib.sha256('\n'.join(stimulus.lines).encode()).hexdigest(),
                        'expected': expected, 'observed': observed,
                        'length_match': len(observed) == len(expected), 'differences': differences})
        print(f"{case['case']} {case['name']}: {len(differences)} differing bytes", flush=True)
    passed = all(case['length_match'] and not case['differences'] for case in results)
    report = {'passed': passed, 'sameboy': str(source), 'minimal': args.minimal,
              'sameboy_commit': subprocess.check_output(['git', '-C', str(source), 'rev-parse', 'HEAD'], text=True).strip(),
              'sameboy_apu_sha256': digest(source / 'Core/apu.c'),
              'oracle': str(ORACLE.relative_to(ROOT)), 'oracle_sha256': digest(ORACLE),
              'hardware_wav_sha256': oracle['hardware_wav']['sha256'],
              'bytes_compared': sum(len(case['expected']) for case in results), 'cases': results}
    (out / 'results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f"{'PASS' if passed else 'FAIL'}: {len(results)} cases, {report['bytes_compared']} bytes; {out / 'results.json'}")
    return 0 if passed else 1


if __name__ == '__main__':
    raise SystemExit(main())
