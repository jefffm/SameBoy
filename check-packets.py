#!/usr/bin/env python3
"""Verify freshly decoded physical readbacks against the committed physical oracle.

This checks observation integrity, not the correctness of an emulator prediction.
"""
import argparse
import json
from pathlib import Path
ROOT = Path(__file__).resolve().parent
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--suite', choices=['v2', 'history'], required=True)
p.add_argument('--metrics', type=Path, required=True)
a = p.parse_args()
metrics = json.loads(a.metrics.read_text())
if not metrics['packet_coverage']['complete'] or not metrics['packet_coverage']['suite_end_received']:
    raise ValueError('Incomplete or ambiguous physical packet coverage')
if a.suite == 'v2':
    oracle = json.loads((ROOT / 'benchmark/oracles/dmg-probe-v2-erekgdxk.json').read_text())
    manifest = json.loads((ROOT / 'artifacts/audio-oracle-probe-v2/manifest.json').read_text())
    expected = {r['case']: [r['NR52']] for r in oracle['prefix_readbacks']}
    expected.update({r['case']: r['readback'] for r in oracle['wave']['cases']})
    kills = {int(s): set(v) for s, v in oracle['sweep']['killed_old_frequencies_by_shift'].items()}
    assert oracle['sweep']['all_initial_active']
    trials = 0
    for case in manifest['cases']:
        if case['group'] != 'ch1-sweep-hazard': continue
        parameters = case['parameters']; payload = [0] * case['readback_length']
        for index, frequency in enumerate(range(parameters['first_frequency'], parameters['end_frequency_exclusive'])):
            pair = 2 | int(frequency not in kills[parameters['shift']])
            payload[index // 4] |= pair << (6 - 2 * (index % 4))
            trials += 1
        expected[case['id']] = payload
    assert trials == oracle['sweep']['trials']
else:
    oracle = json.loads((ROOT / 'benchmark/oracles/dmg-sweep-history-vvg4l2b6.json').read_text())
    expected = {r['case']: r['NR52'] for r in oracle['readbacks']}
packets = [r for r in metrics['hardware_packets'] if r['kind'] == 2]
assert len(packets) == len(expected)
for packet in packets:
    assert packet['status'] == 'valid' and packet['checksum'] == packet['expected_checksum']
    assert packet['payload'] == expected.pop(packet['id'])
assert not expected
print(f"Verified {a.suite}: all {metrics['packet_coverage']['expected']} packets complete; every physical readback matches the committed observation")
print(f"Rejected decoder candidates: {len(metrics['hardware_invalid_candidates'])}; these are not accepted packets")
