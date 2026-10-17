#!/usr/bin/env python3
"""Check pulse duty phase against the recorded DMG using fixed case alignment.

Locate the strongest negative edge in the second oscillator period after every
length-domain retrigger with enough requested length to cover that period.
The phase-bin width is the pulse's hardware duty step (period / 8), not an
accuracy tolerance. No gain, EQ or local alignment is fitted.
"""
import argparse
import csv
import json
from pathlib import Path

import numpy as np
from scipy.io import wavfile

ROOT = Path(__file__).resolve().parents[1]
BASELINE = ROOT / 'artifacts/torture/renders/render-h1xqcgiz'
COMPARISON = ROOT / 'artifacts/torture/comparison-torture-pula2a6h'
PERIOD = 8 * 4 * (2048 - 0x600) / 4194304
STEP = PERIOD / 8


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--render', type=Path, default=BASELINE)
    parser.add_argument('--out', type=Path)
    args = parser.parse_args()
    metrics = json.loads((COMPARISON / 'metrics.json').read_text())
    oracle = np.load(COMPARISON / 'signals.npz')
    rate, audio = wavfile.read(args.render / 'audio.wav')
    with (args.render / 'markers.csv').open() as stream:
        markers = list(csv.DictReader(stream))
    with (args.render / 'apu.csv').open() as stream:
        writes = list(csv.DictReader(stream))
    results = []
    for case_id, reg in [(70, '0xff14'), (72, '0xff19')]:
        starts = [float(m['run_start_seconds']) for m in markers if int(m['case_id']) == case_id and int(m['event']) == 2]
        if not starts:
            continue
        start = starts[0]
        end = next(float(m['run_start_seconds']) for m in markers if int(m['case_id']) == case_id and int(m['event']) == 3)
        original = next(c for c in metrics['cases'] if c['id'] == case_id)
        original_start = original['reference_interval_s'][0]
        with (BASELINE / 'apu.csv').open() as stream:
            original_writes = list(csv.DictReader(stream))
        def triggers(rows, a, b):
            return [float(w['run_start_seconds'])-a for w in rows
                    if a <= float(w['run_start_seconds']) < b and w['address'] == reg and w['value'] == '0xc6']
        candidate_triggers = triggers(writes,start,end)
        hardware_triggers = triggers(original_writes, original_start, original['reference_interval_s'][1])
        assert len(candidate_triggers) == len(hardware_triggers) == 64
        hardware = oracle[f'case_{case_id}_hardware_calibrated']
        ht = oracle[f'case_{case_id}_time_s']
        rows=[]
        # Register loads 0..61 request at least three length ticks, so the
        # second pulse period exists even under an immediate first length tick.
        for length, (ct, ot) in enumerate(zip(candidate_triggers[:62], hardware_triggers[:62])):
            ci=np.arange(int(np.ceil((start+ct+PERIOD)*rate)),int(np.ceil((start+ct+2*PERIOD)*rate)))
            hi=np.flatnonzero((ht>=ot+PERIOD)&(ht<ot+2*PERIOD))
            ce=(ci[0]+int(np.argmin(np.diff(audio[ci,0].astype(float)))))/rate-start-ct
            he=ht[hi[0]+int(np.argmin(np.diff(hardware[hi])))]-ot
            error_steps=int(round((ce-he)/STEP))
            rows.append({'length_load':length,'reference_edge_s':ce,'hardware_edge_s':he,
                         'edge_error_s':ce-he,'duty_step_error':error_steps})
        results.append({'case_id':case_id,'mismatched_bursts':sum(r['duty_step_error']!=0 for r in rows),
                        'observed_bursts':len(rows),'bursts':rows})
    result={'render':str(args.render),'duty_step_s':STEP,'cases':results,
            'pass':bool(results) and all(c['mismatched_bursts']==0 for c in results)}
    if args.out: args.out.write_text(json.dumps(result,indent=2)+'\n')
    for case in results:
        print(f"case {case['case_id']}: {case['mismatched_bursts']}/{case['observed_bursts']} burst phases disagree with hardware")
    return 0 if result['pass'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
