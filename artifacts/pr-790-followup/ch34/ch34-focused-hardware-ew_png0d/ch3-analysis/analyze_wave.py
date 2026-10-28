"""Measure CH3 held-output equivalents without fitting tested-waveform timing."""
import argparse
import csv
import json
from pathlib import Path
import sys

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
from scipy import ndimage

RESULTS = Path(__file__).resolve().parent.parent
KIT = RESULTS.parent / 'ch34-focused-probe'
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--out', type=Path, required=True)
OUT = parser.parse_args().out.resolve()
OUT.mkdir(parents=True, exist_ok=False)
sys.path.insert(0, str(KIT / 'benchmark/torture'))
from analyze import read_audio, sample_hardware

CPU = 4194304
manifest = json.loads((KIT / 'rom/manifest.json').read_text())
reports = [json.loads((RESULTS / folder / 'metrics.json').read_text())
           for folder in ('reference-comparison', 'right-channel-check')]
with (KIT / 'reference/markers.csv').open() as stream:
    markers = list(csv.DictReader(stream))
with (KIT / 'reference/apu.csv').open() as stream:
    writes = list(csv.DictReader(stream))
for w in writes:
    w['time'] = int(w['run_start_tick_8mhz']) / (2 * CPU)

rr, reference, _ = read_audio(KIT / 'reference/audio.wav', 0)
_, frozen, _ = read_audio(KIT / 'candidates/freeze-wave/audio.wav', 0)
_, pulled_up, _ = read_audio(RESULTS / 'candidate-pulled-up/audio.wav', 0)
cases = []
for c in manifest['cases']:
    if not c['group'].startswith('ch3-'):
        continue
    c = dict(c)
    start = next(int(m['run_start_tick_8mhz']) / (2 * CPU) for m in markers
                 if int(m['case_id']) == c['id'] and m['event'] == '2')
    end = next(int(m['run_start_tick_8mhz']) / (2 * CPU) for m in markers
               if int(m['case_id']) == c['id'] and m['event'] == '3')
    ww = [w for w in writes if start <= w['time'] < end]
    c['trigger'] = max(w['time'] for w in ww if w['address'] == '0xff1e' and int(w['value'], 16) & 128)
    c['mute'] = next(w['time'] for w in ww if w['address'] == '0xff1c' and w['value'] == '0x00')
    c['final_nibble'] = c['parameters'].get('replacement_byte', c['parameters']['initial_byte']) >> 4
    cases.append(c)

# Renderer-only effective latency from known nonzero-to-zero NR32 endpoints.
delays = []
for c in cases:
    if not c['final_nibble']:
        continue
    center = c['mute'] + 32 / rr
    lo, hi = round((center - .0001) * rr), round((center + .0001) * rr)
    edge = lo + np.argmax(np.diff(reference[lo:hi])) + .5
    delays.append(edge / rr - c['mute'])
renderer_delay = float(np.median(delays))
assert abs(renderer_delay - 32 / rr) < 1 / rr


def step(times, samples, center, inner=.00009, outer=.00022):
    relative = times - center
    before = (relative > -outer) & (relative < -inner)
    after = (relative > inner) & (relative < outer)
    assert before.sum() >= 4 and after.sum() >= 4
    a = np.polyfit(relative[before], samples[before], 1)
    b = np.polyfit(relative[after], samples[after], 1)
    return float(b[1] - a[1])


all_results, signals, quality = [], {}, []
for channel, report in enumerate(reports):
    assert report['packet_coverage']['complete']
    packets = {(p['kind'], p['id']): p for p in report['hardware_packets']}
    hr, hardware, metadata = read_audio(RESULTS.parent / 'hardware/ew_png0d.wav', channel)
    assert hr == rr == 48000
    alignment = report['alignment']
    clock, offset = alignment['clock'], alignment['offset_s']
    gain = alignment['calibration']['gain_hardware_per_reference']
    coefficients = ndimage.spline_filter(hardware, order=3)
    quality.append({'channel': channel, 'metadata': metadata,
                    'peak_dbfs': float(20 * np.log10(np.max(abs(hardware)))),
                    'near_fullscale_samples': int(np.count_nonzero(abs(hardware) >= .999)),
                    'clock': clock, 'offset_s': offset, 'calibration_gain': gain})
    rows = []
    for c in cases:
        assert [v & 15 for v in packets[2, c['id']]['payload']] == c['parameters']['expected_activity']
        indices = np.arange(int((c['trigger'] + renderer_delay - .00045) * rr),
                            int((c['mute'] + renderer_delay + .00045) * rr))
        times = indices / rr - c['trigger'] - renderer_delay
        h = sample_hardware(hardware, (offset + clock * indices / rr) * hr, coefficients) / gain
        r, f = reference[indices], frozen[indices]
        mute_time = c['mute'] - c['trigger']
        fetch_time = (4094 + 6) / CPU
        measured = {}
        for name, x in (('hardware', h), ('reference', r), ('frozen', f),
                        ('pulled_up', pulled_up[indices])):
            measured[name] = {'trigger_step': step(times, x, 0),
                              'fetch_step': step(times, x, fetch_time),
                              'mute_step': step(times, x, mute_time)}
        variants = [step(times, h, fetch_time, inner, outer)
                    for inner, outer in ((.00007, .00020), (.00009, .00022), (.00011, .00025))]
        timing_variants = [step(times, h, fetch_time + shift / rr)
                           for shift in (-1, 0, 1)]
        rows.append({'id': c['id'], 'name': c['name'], 'group': c['group'],
                     'parameters': c['parameters'], 'final_nibble': c['final_nibble'],
                     'trigger_reference_s': c['trigger'], 'mute_after_trigger_s': mute_time,
                     'measurements': measured, 'fetch_baseline_variants': variants,
                     'fetch_timing_variants': timing_variants})
        if channel == 0:
            signals[c['id']] = (times, h, r, f)

    # One scale per channel from all known active-output-to-digital-zero mute
    # steps. No gain chosen to fit any held sample or hypothesis.
    scales = {}
    for source in ('hardware', 'reference', 'frozen', 'pulled_up'):
        known = [(c['final_nibble'], c['measurements'][source]['mute_step'])
                 for c in rows if c['final_nibble']]
        n, v = np.array(known).T
        unit = float(n @ v / (n @ n))
        scales[source] = {'step_per_nibble': unit,
                          'calibration_cases': len(known),
                          'calibration_residual_rms_nibbles': float(np.sqrt(np.mean((v / unit - n) ** 2)))}
    for c in rows:
        for source, measurement in c['measurements'].items():
            unit = scales[source]['step_per_nibble']
            measurement['held_equivalent_from_fetch'] = c['final_nibble'] + measurement['fetch_step'] / unit
            measurement['held_equivalent_from_trigger'] = (
                None if c['parameters']['mode'].startswith('dac-')
                else -measurement['trigger_step'] / unit)
        unit = scales['hardware']['step_per_nibble']
        c['fetch_baseline_spread_nibbles'] = float(np.ptp(c.pop('fetch_baseline_variants')) / unit)
        c['fetch_one_sample_timing_spread_nibbles'] = float(np.ptp(c.pop('fetch_timing_variants')) / unit)
    all_results.append({'channel': channel, 'scales': scales, 'cases': rows})

result = {'recording_quality': quality, 'renderer_delay_s': renderer_delay,
          'reference_mute_delay_range_s': [float(min(delays)), float(max(delays))],
          'method': 'Common packet-calibrated affine time mapping and gain per channel. Local linear pre/post baselines extrapolated at scheduled trigger/fetch/mute times; no tested-edge alignment. One CH3 amplitude scale per channel from all nonzero final-sample NR32 mute steps. Held equivalents inferred from known final nibble plus first-fetch step. Trigger-step estimates are supplementary only: nearby expiry edges contaminate short-gap cases, and DAC transitions invalidate this estimate in DAC-history cases. These are analog output equivalents, not physical buffer reads. Baseline-window and one-native-sample timing sensitivities are reported as spreads, not confidence intervals.',
          'channels': all_results}
(OUT / 'results.json').write_text(json.dumps(result, indent=2) + '\n')
with (OUT / 'held-output.csv').open('w') as stream:
    writer = csv.writer(stream)
    writer.writerow(['channel', 'id', 'name', 'held_from_fetch', 'held_from_trigger', 'reference_held', 'frozen_held', 'pulled_up_held', 'baseline_spread'])
    for channel in all_results:
        for c in channel['cases']:
            mm = c['measurements']
            writer.writerow([channel['channel'], c['id'], c['name'],
                             mm['hardware']['held_equivalent_from_fetch'],
                             mm['hardware']['held_equivalent_from_trigger'],
                             mm['reference']['held_equivalent_from_fetch'],
                             mm['frozen']['held_equivalent_from_fetch'],
                             mm['pulled_up']['held_equivalent_from_fetch'], c['fetch_baseline_spread_nibbles']])

names = ['ch3-hold-00-0', 'ch3-hold-00-4096', 'ch3-hold-55-1024', 'ch3-hold-55-4096',
         'ch3-bus-0-00-idle', 'ch3-bus-0-aa-wave-read', 'ch3-dac-stop-active-55',
         'ch3-never-aa-wave-read']
fig, axes = plt.subplots(4, 2, figsize=(12, 10), sharex=True, sharey=True)
for ax, name in zip(axes.flat, names):
    c = next(c for c in cases if c['name'] == name)
    times, h, r, f = signals[c['id']]
    ax.plot(times * 1000, h, color='#111111', lw=1.7, label='Physical DMG', zorder=3)
    ax.plot(times * 1000, r, color='#0072B2', lw=1.1, label='SameBoy reference', zorder=3)
    ax.plot(times * 1000, f, color='#D55E00', lw=1.1, ls='--', label='Frozen inactive buffer', zorder=3)
    for event in (0, 4100 / CPU * 1000, (c['mute'] - c['trigger']) * 1000):
        ax.axvline(event, color='#dddddd', lw=.6, ls=':', zorder=0)
    ax.set_title(f"#{c['id']}: {name}", fontsize=9)
    ax.grid(alpha=.15)
    ax.set_xlim(-.3, 2.3)
axes[0, 0].legend(fontsize=8)
for ax in axes[-1]:
    ax.set_xlabel('Milliseconds after retrigger (renderer delay removed)')
fig.supylabel('Amplitude / common calibration gain', fontsize=10)
fig.suptitle('Focused CH3 probe: measured output and two emulator alternatives\nVertical lines: retrigger, first new sample, digital mute; no per-case time alignment', fontsize=12)
fig.tight_layout(rect=(.02, 0, 1, .95))
fig.savefig(OUT / 'waveforms.png', dpi=140)
plt.close(fig)

print('Calibration:', json.dumps([r['scales'] for r in all_results]))
for c in all_results[0]['cases']:
    m = c['measurements']
    print(c['name'], 'held hardware/ref/frozen/pulled_up',
          *(round(m[s]['held_equivalent_from_fetch'], 3) for s in ('hardware', 'reference', 'frozen', 'pulled_up')),
          'trigger', m['hardware']['held_equivalent_from_trigger'])
