#!/usr/bin/env python3
"""Compare a new full-suite render using the first hardware run's fixed mapping.

Reuse the already validated hardware packets. Decode only the candidate's
acoustic readbacks, and require identical ROM and CPU marker timing before
applying the frozen clock, offset and calibration gain to every case.
"""
import argparse
import json
from pathlib import Path
import hashlib
import wave

ROOT = Path(__file__).resolve().parents[2]

def archived_path(value):
    value = str(value)
    if '/artifacts/' in value:
        value = 'artifacts/' + value.split('/artifacts/', 1)[1]
    path = Path(value)
    return path if path.is_absolute() else ROOT / path

def physical_record(sha256):
    manifest = json.loads((ROOT / 'hardware/manifest.json').read_text())
    return next(r for r in manifest['recordings'] if r['original_wav_sha256'] == sha256)

def verify_pcm(path, record):
    with wave.open(str(path), 'rb') as wav:
        actual = (wav.getframerate(), wav.getnchannels(), wav.getsampwidth(), wav.getnframes())
        expected = tuple(record[k] for k in ['sample_rate', 'channels', 'sample_width_bytes', 'frames'])
        if actual != expected:
            raise ValueError('Decoded hardware audio format differs from the original')
        h = hashlib.sha256()
        while chunk := wav.readframes(wav.getframerate()):
            h.update(chunk)
    if h.hexdigest() != record['pcm_sha256']:
        raise ValueError('Decoded hardware PCM differs from the original')

from scipy import ndimage

from analyze import (case_metrics, decode_reference, digest, load_markers,
                     packet_coverage, protocol_lengths, read_audio, unique_packets)


def compare(baseline_path, render, identifiers=None, hardware_override=None):
    baseline = json.loads(baseline_path.read_text())
    manifest_path = render / 'manifest.json'
    manifest = json.loads(manifest_path.read_text())
    original = json.loads(archived_path(baseline['manifest']['path']).read_text())
    if manifest['rom_sha256'] != original['rom_sha256']:
        raise ValueError('Candidate ROM differs from the recorded hardware ROM')
    if [(c['id'], c['name'], c['readback_length']) for c in manifest['cases']] != [
            (c['id'], c['name'], c['readback_length']) for c in original['cases']]:
        raise ValueError('Candidate case identities differ from hardware manifest')
    markers = load_markers(render / 'markers.csv')
    if markers != load_markers(archived_path(baseline['markers']['path'])):
        raise ValueError('Candidate CPU event timing differs; frozen mapping is invalid')
    record = physical_record(baseline['hardware']['sha256'])
    hardware_path = hardware_override or ROOT / 'build/hardware' / (record['id'] + '.wav')
    verify_pcm(hardware_path, record)
    hr, hardware, hmeta = read_audio(hardware_path, baseline['hardware']['selected_channel'])
    hmeta['original_wav_sha256'] = record['original_wav_sha256']
    hmeta['original_pcm_verified'] = True
    rr, reference, rmeta = read_audio(render / 'audio.wav', baseline['reference']['selected_channel'])
    lengths = protocol_lengths(original['cases'])
    packets, invalid, decoder = decode_reference(reference, rr, markers, lengths)
    ru, _ = unique_packets(packets)
    hu, _ = unique_packets(baseline['hardware_packets'])
    mapping = baseline['alignment']
    clock, offset = mapping['clock'], mapping['offset_s']
    gain = mapping['calibration']['gain_hardware_per_reference']
    coefficients = ndimage.spline_filter(hardware, order=3)
    selected = [c for c in original['cases'] if identifiers is None or c['id'] in identifiers]
    if identifiers is not None and set(identifiers) != {c['id'] for c in selected}:
        raise ValueError('Unknown requested case ID')
    results = []
    for case in selected:
        identifier = case['id']
        start = next(m['time_s'] for m in markers if m['id'] == identifier and m['event'] == 2)
        end = next(m['time_s'] for m in markers if m['id'] == identifier and m['event'] == 3)
        result, _ = case_metrics(case, start, end, reference, rr, hardware, hr,
                                 coefficients, clock, offset, gain)
        old = next(c for c in baseline['cases'] if c['id'] == identifier)
        result['baseline_waveform'] = old['waveform']
        result['baseline_native_spectrum'] = old['native_spectrum']
        key = 2, identifier
        if key not in lengths:
            result['readback'] = {'status': 'not_emitted_by_this_case'}
        elif key not in ru or key not in hu:
            result['readback'] = {'status': 'missing_or_ambiguous_packet'}
        else:
            differences = [{'index': i, 'label': case['readback_labels'][i],
                            'reference': r, 'hardware': h}
                           for i, (r, h) in enumerate(zip(ru[key]['payload'], hu[key]['payload'])) if r != h]
            result['readback'] = {'status': 'different' if differences else 'identical',
                                  'differences': differences}
        results.append(result)
    return {
        'baseline_report': str(baseline_path), 'baseline_report_sha256': digest(baseline_path),
        'reference': rmeta, 'hardware': hmeta,
        'manifest_sha256': digest(manifest_path), 'markers_sha256': digest(render / 'markers.csv'),
        'candidate_provenance': json.loads((render / 'provenance.json').read_text()),
        'frozen_mapping': {'clock': clock, 'offset_s': offset, 'gain': gain},
        'cpu_marker_timing_identical': True,
        'packet_coverage': packet_coverage(packets, baseline['hardware_packets'], lengths, original['cases']),
        'candidate_decoder': decoder, 'candidate_invalid_packets': invalid,
        'cases': results,
        'scope': 'Same hardware samples and original global mapping/gain for every candidate; no new clock, gain, EQ or per-case fit. Hardware packets reuse the validated baseline report.',
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('render', type=Path)
    parser.add_argument('--baseline', type=Path, default=Path('artifacts/torture/comparison-torture-pula2a6h/metrics.json'))
    parser.add_argument('--case', type=int, action='append', help='Restrict reported cases; retain complete packet coverage')
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--hardware', type=Path, help='Decoded package FLAC; exact original PCM verified before reuse of archived packet decoding')
    args = parser.parse_args()
    report = compare(args.baseline, args.render, args.case, args.hardware)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    with args.out.open('x') as output:
        json.dump(report, output, indent=2, allow_nan=False)
        output.write('\n')
    print(json.dumps({'report': str(args.out), 'cases': len(report['cases']),
                      'packet_coverage': report['packet_coverage'],
                      'readback_differences': sum(len(c['readback'].get('differences', [])) for c in report['cases'])}, indent=2))


if __name__ == '__main__':
    main()
