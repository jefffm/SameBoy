"""Check both earlier takes with their original timing and gain calibration."""
import argparse
import importlib.util
import json
from pathlib import Path
import sys

import numpy as np
from scipy import ndimage

ROOT = Path(__file__).resolve().parents[1]
OLD = ROOT.parent / 'ch34-clock-probe'
DATA = ROOT.parent / 'ch34-focused-hardware-ew_png0d'
sys.path.insert(0, str(OLD / 'benchmark/torture'))
from analyze import read_audio, sample_hardware

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('render', type=Path)
render = parser.parse_args().render.resolve()
for name in ('apu.csv', 'markers.csv'):
    assert (render / name).read_bytes() == (OLD / 'reference' / name).read_bytes(), name
rate, audio, _ = read_audio(render / 'audio.wav', 0)
spec = importlib.util.spec_from_file_location('phase', DATA / 'ch4-analysis/analyze_ch4.py')
phase = importlib.util.module_from_spec(spec)
spec.loader.exec_module(phase)
first = json.loads((OLD / 'hardware-xryrmxjl/ch4-analysis/results.json').read_text())
repeat = json.loads((OLD / 'hardware-qw3uz2uv/repeat-comparison.json').read_text())
delay = first['reference_effective_write_to_PCM_delay_s']
model_onsets = {}
for c in first['cases']:
    origin = c['retrigger_log_s'] + delay
    events = phase.edges(audio, rate, origin + .001, origin + .0154, .04)
    assert len(events) >= 3
    model_onsets[c['id']] = (events[0]['time_s'] - origin) * 4194304

result = []
for take, noise in [('xryrmxjl', first['cases']), ('qw3uz2uv', repeat['ch4'][0]['cases'])]:
    folder = OLD / f'hardware-{take}'
    report = json.loads((folder / 'reference-comparison/metrics.json').read_text())
    wave = json.loads((folder / 'wave-measurements.json').read_text())
    hr, hardware, _ = read_audio(ROOT.parent / 'hardware' / f'{take}.wav', 0)
    assert rate == hr == 48000
    mapping = report['alignment']
    gain = mapping['calibration']['gain_hardware_per_reference']
    coefficients = ndimage.spline_filter(hardware, order=3)
    waves = []
    for c in wave['ch3']:
        origin = c['trigger_reference_s'] + wave['renderer_delay_s']
        indices = np.arange(int((origin + .00008) * rate) + 1, int((origin + .00185) * rate))
        h = sample_hardware(hardware, (mapping['offset_s'] + mapping['clock'] * indices / rate) * hr,
                            coefficients) / gain
        error = float(np.sqrt(np.mean((h - audio[indices]) ** 2)))
        waves.append({'id': c['id'], 'name': c['name'], 'waveform_rmse': error})
    noises = [{'id': c['id'], 'name': c['name'],
               'onset_error_T': model_onsets[c['id']] - c['hardware_onset_T']} for c in noise]
    largest_wave = max(c['waveform_rmse'] for c in waves)
    largest_noise = max(abs(c['onset_error_T']) for c in noises)
    result.append({'take': take, 'wave': waves, 'noise': noises,
                   'max_waveform_rmse': largest_wave, 'max_noise_onset_error_T': largest_noise})
    print(take, len(waves), 'wave cases', largest_wave, len(noises), 'noise cases', largest_noise)

(render / 'regression.json').write_text(json.dumps(result, indent=2) + '\n')
# Fixed engineering allowances: observed analog waveform mismatch in unchanged
# controls is about .014; the old discrete held-output errors were about .16.
assert all(r['max_waveform_rmse'] < .03 and r['max_noise_onset_error_T'] < 128 for r in result)
print('PASS: both earlier recordings')
