"""Check public PCM output against the focused physical recording.

This extends the existing external audio-lab test workflow, not SameBoy's
Tester subproject. Expected values come from hardware, not an emulator snapshot.
"""
import argparse
import importlib.util
import json
from pathlib import Path
import sys

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT.parent / 'ch34-focused-hardware-ew_png0d'
KIT = ROOT.parent / 'ch34-focused-probe'
sys.path.insert(0, str(KIT / 'benchmark/torture'))
from analyze import read_audio

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('render', type=Path)
parser.add_argument('--channel', choices=('wave', 'noise', 'both'), default='both')
args = parser.parse_args()
render = args.render.resolve()
for name in ('apu.csv', 'markers.csv'):
    assert (render / name).read_bytes() == (KIT / 'reference' / name).read_bytes(), name
rate, audio, _ = read_audio(render / 'audio.wav', 0)
assert rate == 48000
result = {}
failures = []


def step(times, samples, center):
    relative = times - center
    before = (relative > -.00022) & (relative < -.00009)
    after = (relative > .00009) & (relative < .00022)
    a = np.polyfit(relative[before], samples[before], 1)
    b = np.polyfit(relative[after], samples[after], 1)
    return float(b[1] - a[1])


if args.channel in ('wave', 'both'):
    recorded = json.loads((DATA / 'ch3-analysis/results.json').read_text())
    cases = recorded['channels'][0]['cases']
    delay = recorded['renderer_delay_s']
    measurements = []
    for c in cases:
        origin = c['trigger_reference_s'] + delay
        indices = np.arange(int((origin - .00045) * rate),
                            int((origin + c['mute_after_trigger_s'] + .00045) * rate))
        times = indices / rate - origin
        measurements.append({'id': c['id'], 'name': c['name'],
                             'final': c['final_nibble'],
                             'fetch_step': step(times, audio[indices], 4100 / 4194304),
                             'mute_step': step(times, audio[indices], c['mute_after_trigger_s'])})
    known = [m for m in measurements if m['final']]
    final_nibbles = np.array([m['final'] for m in known])
    mute_steps = np.array([m['mute_step'] for m in known])
    amplitude_per_nibble = float(final_nibbles @ mute_steps / (final_nibbles @ final_nibbles))
    assert amplitude_per_nibble > 0
    for i, m in enumerate(measurements):
        m['model_held'] = m['final'] + m['fetch_step'] / amplitude_per_nibble
        m['physical_held'] = [ch['cases'][i]['measurements']['hardware']['held_equivalent_from_fetch']
                              for ch in recorded['channels']]
        m['errors'] = [m['model_held'] - h for h in m['physical_held']]
        # Two sample units exceed the measured baseline/window sensitivities;
        # a frozen or RAM-refill regression differs by 5, 10, or 15 units.
        if max(abs(e) for e in m['errors']) > 2:
            failures.append(m['name'])
    result['wave'] = {'cases': measurements, 'tolerance_sample_units': 2,
                      'max_error': max(abs(e) for m in measurements for e in m['errors'])}

if args.channel in ('noise', 'both'):
    spec = importlib.util.spec_from_file_location('phase', DATA / 'ch4-analysis/analyze_ch4.py')
    phase = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(phase)
    cases, audios, metrics = phase.load()
    audios['reference'] = (rate, audio)
    rows, calibration = phase.measure(cases, audios, metrics)
    measurements = []
    for row in rows:
        model = row['audio']['reference']['edges'][:3]
        assert len(model) == 3
        errors = [model[i]['latency_T'] - row['audio'][channel]['edges'][i]['latency_T']
                  for channel in ('left', 'right') for i in range(3)]
        measurements.append({'id': row['id'], 'name': row['name'], 'edge_errors_T': errors})
        # Engineering tolerance from the acquisition analysis, not an assertion
        # of sub-sample absolute accuracy. Competing phase models miss by 384+ T.
        if max(abs(e) for e in errors) > 128:
            failures.append(row['name'])
    patterns = phase.pattern_checks(rows, metrics)
    for channel in ('left', 'right'):
        if patterns[channel]['common_prefix_pattern_mismatches']:
            failures.append(f'noise-{channel}-edge-pattern')
    result['noise'] = {'cases': measurements, 'calibration': calibration,
                       'patterns': patterns, 'tolerance_T': 128,
                       'max_error_T': max(abs(e) for m in measurements for e in m['edge_errors_T'])}

result['failures'] = failures
(render / f'regression-{args.channel}.json').write_text(json.dumps(result, indent=2) + '\n')
for channel in ('wave', 'noise'):
    if channel in result:
        print(channel, len(result[channel]['cases']), 'cases; maximum error',
              result[channel].get('max_error', result[channel].get('max_error_T')))
print('FAIL' if failures else 'PASS', len(failures), failures)
raise SystemExit(bool(failures))
