#!/usr/bin/env python3
"""Assert the exact physical probe's CPU readbacks against the recorded DMG."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sameboy', type=Path, default=ROOT / 'SameBoy')
    parser.add_argument('--out', type=Path, help='New output directory; defaults to a fresh build directory')
    parser.add_argument('--group', choices=['all', 'sweep', 'wave'], default='all')
    args = parser.parse_args()
    source = args.sameboy.resolve()
    probe = ROOT / 'artifacts/audio-oracle-probe-v2'
    manifest_path = probe / 'reference/manifest.json'
    oracle_path = ROOT / 'benchmark/oracles/dmg-probe-v2-erekgdxk.json'
    manifest = json.loads(manifest_path.read_text())
    oracle = json.loads(oracle_path.read_text())
    rom = probe / Path(manifest['rom']).name
    if digest(manifest_path) != oracle['manifest_sha256'] or digest(rom) != oracle['rom_sha256']:
        raise ValueError('Probe inputs differ from the physical oracle')
    if args.out:
        out = args.out.resolve()
        out.mkdir(parents=True, exist_ok=False)
    else:
        out = Path(tempfile.mkdtemp(prefix='physical-probe-', dir=ROOT / 'build'))
    symbols = {name: address for address, name in re.findall(
        r'^00:([0-9a-fA-F]+) (wReadback(?:Size)?)$',
        (probe / 'audio-torture.sym').read_text(), re.M)}
    runner = out / 'probe-readbacks'
    commands = [
        ['make', '-C', str(source), 'CONF=release', 'build/lib/libsameboy.a', 'bootroms'],
        ['cc', '-std=gnu11', '-O2', '-Wall', '-Wextra', '-Werror', '-I', str(source),
         str(ROOT / 'benchmark/probe-readbacks.c'), str(source / 'build/lib/libsameboy.a'),
         '-lm', '-ldl', '-o', str(runner)],
    ]
    with (out / 'build.log').open('w') as log:
        for command in commands:
            subprocess.run(command, stdout=log, stderr=subprocess.STDOUT, check=True)
    command = [str(runner), str(rom), str(source / 'build/bin/BootROMs/dmg_boot.bin'),
               symbols['wReadbackSize'], symbols['wReadback']]
    started = time.monotonic()
    with (out / 'execution.log').open('w') as log:
        raw = subprocess.check_output(command, text=True, stderr=log)
    duration = time.monotonic() - started
    (out / 'readbacks.jsonl').write_text(raw)
    observed = [json.loads(line) for line in raw.splitlines()]
    if [r['id'] for r in observed] != [c['id'] for c in manifest['cases']]:
        raise ValueError('Execution did not return every case in manifest order')
    expected = {r['case']: [r['NR52']] for r in oracle['prefix_readbacks']}
    expected.update({r['case']: r['readback'] for r in oracle['wave']['cases']})
    kills = {int(shift): set(values) for shift, values in oracle['sweep']['killed_old_frequencies_by_shift'].items()}
    if not oracle['sweep']['all_initial_active']:
        raise ValueError('Oracle packing requires the measured initial-active control')
    trials = 0
    for case in manifest['cases']:
        if case['group'] != 'ch1-sweep-hazard':
            continue
        p = case['parameters']
        payload = [0] * case['readback_length']
        for index, frequency in enumerate(range(p['first_frequency'], p['end_frequency_exclusive'])):
            pair = 2 | int(frequency not in kills[p['shift']])
            payload[index // 4] |= pair << (6 - 2 * (index % 4))
            trials += 1
        expected[case['id']] = payload
    if trials != oracle['sweep']['trials']:
        raise ValueError('Manifest sweep domain differs from the physical oracle')
    results = []
    for case, actual in zip(manifest['cases'], observed):
        group = 'wave' if case['group'] == 'ch3' else 'sweep'
        if case['id'] not in expected or args.group not in ('all', group):
            continue
        want = expected[case['id']]
        got = actual['payload']
        differences = [{'index': i, 'expected': a, 'observed': b}
                       for i, (a, b) in enumerate(zip(want, got)) if a != b]
        results.append({'id': case['id'], 'name': case['name'], 'group': group,
                        'expected': want, 'observed': got, 'length_match': len(want) == len(got),
                        'differences': differences})
    failures = [r for r in results if not r['length_match'] or r['differences']]
    git = lambda *a: subprocess.check_output(['git', '-C', str(source), *a], text=True)
    report = {'status': 'failed' if failures else 'passed', 'group': args.group,
              'sameboy_commit': git('rev-parse', 'HEAD').strip(),
              'sameboy_diff_sha256': hashlib.sha256(git('diff', 'HEAD').encode()).hexdigest(),
              'rom_sha256': digest(rom), 'manifest_sha256': digest(manifest_path),
              'oracle_sha256': digest(oracle_path), 'runner_source_sha256': digest(ROOT / 'benchmark/probe-readbacks.c'),
              'commands': commands + [command], 'execution_seconds': duration,
              'checked_cases': len(results), 'failed_cases': len(failures),
              'different_bytes': sum(len(r['differences']) for r in results), 'results': results}
    (out / 'results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({k: v for k, v in report.items() if k not in ['results', 'commands']}, indent=2))
    print(f'Report: {out / "results.json"}')
    return bool(failures)


if __name__ == '__main__':
    raise SystemExit(main())
