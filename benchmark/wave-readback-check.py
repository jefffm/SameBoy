#!/usr/bin/env python3
"""Replay standalone CH3 cases against readbacks decoded from the physical WAV."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'benchmark/torture'))
from cases import catalog

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--sameboy', type=Path, default=ROOT / 'SameBoy')
    p.add_argument('--all-wave', action='store_true')
    p.add_argument('--baseline', action='store_true', help='Assert the recorded SameBoy baseline, instead of the hardware target')
    args = p.parse_args()
    source = args.sameboy.resolve()
    out = ROOT / 'artifacts/wave-retrigger'
    out.mkdir(exist_ok=True)
    subprocess.run(['make', '-C', str(source), 'CONF=release', 'build/lib/libsameboy.a', 'bootroms'], check=True, stdout=subprocess.DEVNULL)
    exe = out / 'readback'
    subprocess.run(['cc', '-std=gnu11', '-O2', '-Wall', '-Wextra', '-Werror', '-I', str(source),
                    str(ROOT / 'benchmark/wave-readback.c'), str(source / 'build/lib/libsameboy.a'), '-lm', '-ldl', '-o', str(exe)], check=True)
    oracle = json.loads((ROOT / 'benchmark/wave-readback-oracle.json').read_text())
    cases = [c for c in oracle['cases'] if c['id'] in (125, 126, 133, 134) or args.all_wave]
    stimuli = {c.name: c for c in catalog()}
    results = []
    for case in cases:
        destination = out / case['name']
        stimulus = stimuli[case['name']]
        if hashlib.sha256('\n'.join(stimulus.lines).encode()).hexdigest() != case['stimulus_sha256']:
            raise ValueError(f"Stimulus changed since hardware capture: {case['name']}")
        subprocess.run([sys.executable, str(ROOT / 'benchmark/torture/build.py'), '--case', case['name'], '--out', str(destination)], check=True, stdout=subprocess.DEVNULL)
        symbols = {name: address for address, name in re.findall(r'^00:([0-9a-fA-F]+) (wReadback(?:Size)?)$', (destination / 'audio-torture.sym').read_text(), re.M)}
        command = [str(exe), str(destination / 'audio-torture.gb'), str(source / 'build/bin/BootROMs/dmg_boot.bin'), symbols['wReadbackSize'], symbols['wReadback']]
        observed = json.loads(subprocess.check_output(command, text=True).splitlines()[-1])
        want = case['baseline' if args.baseline else 'hardware']
        mismatches = [{'index': i, 'label': stimulus.readback_labels[i], 'expected': a, 'sameboy': b} for i, (a, b) in enumerate(zip(want, observed)) if a != b]
        result = {'id': case['id'], 'case': case['name'], 'length_match': len(want) == len(observed), 'mismatches': mismatches, 'observed': observed}
        results.append(result)
        print(f"{case['id']} {case['name']}: {len(mismatches)} differing bytes" + (f" {mismatches}" if mismatches else ''), flush=True)
    (out / 'latest-readbacks.json').write_text(json.dumps(results, indent=2) + '\n')
    return int(any(not r['length_match'] or r['mismatches'] for r in results))

if __name__ == '__main__':
    raise SystemExit(main())
