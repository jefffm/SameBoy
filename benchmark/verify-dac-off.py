#!/usr/bin/env python3
"""Regression: disconnected DMG DAC output is silent; an unrouted DAC prevents it."""
import argparse
import csv
import hashlib
import json
from pathlib import Path

import numpy as np
from scipy.io import wavfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--prefix', default='artifacts/noise-probe/gate-hold')
parser.add_argument('--control-reference', default='results/dac-edge/before-dac-control.wav')
parser.add_argument('--out', type=Path, default=Path('artifacts/noise-probe/dac-off-verification.json'))
parser.add_argument('--silence-only', action='store_true', help='Assert DAC-off gaps only; skip continuous enabled-DAC control comparison')
args = parser.parse_args()
rate, audio = wavfile.read(args.prefix + '-dac-off.wav')
rows = list(csv.DictReader(open(args.prefix + '-dac-off.csv')))
off_tick = None
intervals = []
for row in rows:
    if int(row['address'], 16) != 0xff21:
        continue
    tick = int(row['run_start_tick_8mhz'])
    if int(row['value'], 16) == 0:
        off_tick = tick
    elif off_tick is not None:
        # Trace times precede the 12-T-cycle LDH instruction. Start at the
        # first sample after its completion: every DAC is now disabled, so
        # the mixer must disconnect without waiting for the DAC fade.
        first = int(np.ceil((off_tick + 24) * rate / 8388608))
        last = int(np.floor(tick * rate / 8388608))
        assert last > first
        intervals.append({'samples': [first, last],
                          'nonzero_stereo_values': int(np.count_nonzero(audio[first:last]))})
        off_tick = None
assert intervals, 'No complete DAC-off intervals captured'
control_identical = None
if not args.silence_only:
    control_identical = Path(args.prefix + '-dac-control.wav').read_bytes() == Path(args.control_reference).read_bytes()
passed = all(x['nonzero_stereo_values'] == 0 for x in intervals) and (args.silence_only or control_identical)
result = {
    'passed': passed,
    'prefix': args.prefix,
    'closed_dac_off_intervals': len(intervals),
    'nonzero_stereo_values_while_disconnected': sum(x['nonzero_stereo_values'] for x in intervals),
    'inactive_unrouted_dac_control_byte_identical': control_identical,
    'control_reference': args.control_reference,
    'wav_sha256': hashlib.sha256(Path(args.prefix + '-dac-off.wav').read_bytes()).hexdigest(),
    'settling_authority': '12 T-cycle LDH bus instruction; rounded up to the first capture sample after completion. No DAC-discharge allowance.',
    'scope': 'Output regression against documented disconnection, plus unchanged active-DAC control. These generated ROMs have not yet been recorded on hardware.',
}
args.out.parent.mkdir(parents=True, exist_ok=True)
args.out.write_text(json.dumps(result, indent=2) + '\n')
print(json.dumps(result, indent=2))
raise SystemExit(0 if passed else 1)
