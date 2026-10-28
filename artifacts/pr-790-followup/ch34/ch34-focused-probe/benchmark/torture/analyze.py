#!/usr/bin/env python3
"""Decode DMG torture packets and compare audio using one global clock and gain.

Packet checks establish identity/coverage, not analog accuracy. Numerical audio
metrics have no pass threshold. Native spectra use each entire stimulus segment.
"""
from __future__ import annotations

import argparse
import csv
import hashlib
import html
import json
import math
import warnings
from pathlib import Path

import numpy as np
from scipy import ndimage, optimize, signal
from scipy.io import wavfile

CPU_HZ = 4194304
BIT_T = 8192
BIT_S = BIT_T / CPU_HZ
FRAME_S = 70224 / CPU_HZ
FREQUENCIES = (2048, 4096)
SYNC = np.unpackbits(np.array([0xD3, 0x91], dtype=np.uint8))


def digest(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def read_audio(path, channel):
    with warnings.catch_warnings():
        warnings.simplefilter('ignore', wavfile.WavFileWarning)
        rate, data = wavfile.read(path)
    channels = data.shape[1] if data.ndim == 2 else 1
    if channel >= channels:
        raise ValueError(f'{path}: channel {channel} does not exist')
    source_dtype = str(data.dtype)
    data = data[:, channel] if data.ndim == 2 else data
    if np.issubdtype(data.dtype, np.integer):
        if data.dtype == np.uint8:
            data = (data.astype(float) - 128) / 128
        else:
            data = data.astype(float) / 2 ** (np.iinfo(data.dtype).bits - 1)
    else:
        data = data.astype(float)
    if not np.isfinite(data).all():
        raise ValueError(f'{path}: non-finite audio samples')
    return rate, data, {'path': str(path), 'sha256': digest(path), 'sample_rate': rate,
                        'channels': channels, 'selected_channel': channel,
                        'sample_format': source_dtype, 'samples': len(data),
                        'duration_s': len(data) / rate}


def tone_evidence(samples, rate, clock):
    """Rectangular coherent energy over one protocol bit, at every native sample."""
    length = round(BIT_S * clock * rate)
    if length < 1 or len(samples) < length:
        raise ValueError('Audio cannot contain one protocol bit')
    time = np.arange(len(samples)) / rate
    energies = []
    for frequency in FREQUENCIES:
        running = np.r_[0j, np.cumsum(samples * np.exp(-2j * np.pi * frequency / clock * time))]
        energies.append(abs(running[length:] - running[:-length]) ** 2)
    total = energies[0] + energies[1]
    evidence = np.divide(energies[1] - energies[0], total,
                         out=np.zeros_like(total), where=total > 0)
    return evidence


def protocol_lengths(cases):
    lengths = {(1, int(case['id'])): 0 for case in cases}
    for case in cases:
        labels = case.get('readback_labels', [])
        length = case.get('readback_length', len(labels))
        if length:
            lengths[(2, int(case['id']))] = int(length)
    lengths[(3, 0xFFFF)] = 0
    return lengths


def packet_at(evidence, position, bit_samples, lengths):
    def get_bytes(count):
        locations = position + np.arange(count * 8) * bit_samples
        available = int(math.floor((len(evidence) - 1 - position) / bit_samples)) + 1
        if available < count * 8:
            return None
        left = np.floor(locations).astype(int)
        fraction = locations-left
        values = evidence[left]*(1-fraction) + evidence[np.minimum(left+1,len(evidence)-1)]*fraction
        bits = values > 0
        return np.packbits(bits).tolist()

    header = get_bytes(6)
    if header is None:
        return {'status': 'partial_header'}
    if header[:2] != [0xD3,0x91]:
        return {'status':'invalid_sync_at_decoded_bit_phase','sync_bytes':header[:2]}
    kind, identifier, length = header[2], header[3] | (header[4] << 8), header[5]
    packet = {'kind': kind, 'id': identifier, 'payload_length': length}
    expected = lengths.get((kind, identifier))
    if expected is None:
        return dict(packet, status='unknown_kind_or_id')
    if length != expected:
        return dict(packet, status='unexpected_payload_length', expected_payload_length=expected)
    data = get_bytes(7 + length)
    if data is None:
        return dict(packet, status='partial_packet')
    checksum = int(np.bitwise_xor.reduce(np.array(data[2:-1], dtype=np.uint8)))
    packet.update(payload=data[6:-1], checksum=data[-1], expected_checksum=checksum,
                  bytes=data, status='valid' if data[-1] == checksum else 'invalid_checksum')
    return packet


def decode_packets(samples, rate, lengths, clock=1.0):
    evidence = tone_evidence(samples, rate, clock)
    bit_samples = BIT_S * clock * rate
    available = len(evidence) - math.ceil((len(SYNC) - 1) * bit_samples)
    if available <= 0:
        return [], [], {'clock_used': clock, 'candidate_regions': 0}
    starts = np.arange(available)
    matches = np.ones(available, dtype=bool)
    score = np.zeros(available)
    # Exact agreement of all 16 sync-bit decisions is the protocol preamble
    # condition. No signal amplitude or correlation threshold is introduced.
    for index, bit in enumerate(SYNC):
        location=index*bit_samples
        whole=math.floor(location)
        fraction=location-whole
        positions=starts+whole
        values=evidence[positions] if fraction==0 else evidence[positions]*(1-fraction)+evidence[positions+1]*fraction
        matches &= (values > 0) == bool(bit)
        score += values * (1 if bit else -1)
    indices = np.flatnonzero(matches)
    groups = np.split(indices, np.flatnonzero(np.diff(indices) > 1) + 1) if len(indices) else []
    valid, invalid = [], []
    for group in groups:
        # Decode every preamble-consistent native start in this contiguous region.
        # Checksum and manifest identity select complete packets; the strongest
        # full preamble selects the timestamp among equally valid bit phases.
        candidates = []
        for position in group[np.argsort(-score[group])]:
            packet = packet_at(evidence, float(position), bit_samples, lengths)
            packet.update(start_s=float(position / rate), preamble_score=float(score[position]),
                          candidate_region_s=[float(group[0] / rate), float(group[-1] / rate)])
            candidates.append(packet)
            if packet['status'] == 'valid':
                break
        selected = next((p for p in candidates if p['status'] == 'valid'), candidates[0])
        if selected['status'] == 'valid':
            selected['end_s'] = selected['start_s'] + len(selected['bytes']) * 8 * BIT_S * clock
            valid.append(selected)
        else:
            invalid.append(selected)
    # A single acoustic packet can produce disconnected preamble-consistent
    # phase regions within its first bit. These are candidate starts, not two
    # transmissions. Keep the strongest checksum-valid phase and record every
    # alternative; genuinely separate repeated packets remain duplicates.
    selected_packets = []
    for packet in sorted(valid, key=lambda p: -p['preamble_score']):
        same = next((p for p in selected_packets
                     if abs(p['start_s']-packet['start_s']) < BIT_S*clock), None)
        if same is None:
            selected_packets.append(packet)
        else:
            same.setdefault('alternative_preamble_phases', []).append({
                key:packet[key] for key in ('start_s','preamble_score','candidate_region_s','kind','id','payload','checksum')})
    valid = sorted(selected_packets, key=lambda p:p['start_s'])
    for candidate in invalid:
        overlaps = [p for p in valid if abs(p['start_s']-candidate['start_s']) < BIT_S*clock]
        candidate['within_valid_packet_first_bit'] = bool(overlaps)
    return valid, invalid, {'clock_used': clock, 'bit_duration_s': BIT_S * clock,
                            'candidate_regions': len(groups), 'valid_packets': len(valid),
                            'invalid_or_partial_candidates': len(invalid)}


def decode_reference(samples, rate, markers, lengths):
    packets, invalid = [], []
    events = {1:1,5:2,6:3}
    for marker in markers:
        if marker['event'] not in events:
            continue
        key = events[marker['event']],marker['id']
        if key not in lengths:
            raise ValueError(f'Reference marker {key} disagrees with manifest packet lengths')
        # The callback identifies the packet writer; allow the one-bit phase
        # ambiguity of the coherent decoder around that source-defined anchor.
        first=max(0,math.floor((marker['time_s']-BIT_S)*rate))
        end=min(len(samples),math.ceil((marker['time_s']+(7+lengths[key])*8*BIT_S+BIT_S)*rate))
        found,bad,_=decode_packets(samples[first:end],rate,{key:lengths[key]})
        for packet in found+bad:
            packet['start_s']=(round(packet['start_s']*rate)+first)/rate
            packet['candidate_region_s']=[t+first/rate for t in packet['candidate_region_s']]
            if 'end_s' in packet: packet['end_s']+=first/rate
            for phase in packet.get('alternative_preamble_phases',[]):
                phase['start_s']+=first/rate
                phase['candidate_region_s']=[t+first/rate for t in phase['candidate_region_s']]
        packets.extend(found);invalid.extend(bad)
    return packets,invalid,{'method':'Decode actual audio around reference packet-writer markers; each search includes one bit of phase ambiguity before/after the specified packet.',
                            'valid_packets':len(packets),'invalid_or_partial_candidates':len(invalid)}


def unique_packets(packets):
    grouped = {}
    for packet in packets:
        grouped.setdefault((packet['kind'], packet['id']), []).append(packet)
    return {key: values[0] for key, values in grouped.items() if len(values) == 1}, {
        f'{key[0]}:{key[1]}': values for key, values in grouped.items() if len(values) > 1}


def affine_fit(reference, hardware, fixed_clock=None, fixed_offset=None):
    ru, _ = unique_packets(reference)
    hu, _ = unique_packets(hardware)
    keys = sorted(ru.keys() & hu.keys())
    x = np.array([ru[key]['start_s'] for key in keys])
    y = np.array([hu[key]['start_s'] for key in keys])
    if fixed_clock is None:
        if len(x) < 2 or np.ptp(x) == 0:
            raise ValueError('Two distinct matched packets are required to estimate clock; provide --clock and --offset as a fallback')
        xc, yc = x - x.mean(), y - y.mean()
        clock = float(np.dot(xc, yc) / np.dot(xc, xc)) if fixed_offset is None else float(np.dot(x, y-fixed_offset) / np.dot(x,x))
    else:
        clock = fixed_clock
    if clock <= 0:
        raise ValueError('Packet timing gives a nonpositive clock ratio')
    if fixed_offset is None:
        if not len(x):
            raise ValueError('A matched packet is required to estimate offset; provide --offset')
        offset = float(np.mean(y - clock * x))
    else:
        offset = fixed_offset
    residual = y - (offset + clock * x)
    return clock, offset, {'matched_keys': [[int(k), int(i)] for k, i in keys],
                           'matched_reference_s': x.tolist(), 'matched_hardware_s': y.tolist(),
                           'packet_timing_residual_s': residual.tolist(),
                           'packet_timing_rms_s': float(np.sqrt(np.mean(residual ** 2))) if len(residual) else None}


def sample_hardware(samples, positions, coefficients=None):
    if np.array_equal(positions, np.rint(positions)):
        return samples[positions.astype(int)]
    if coefficients is None:
        coefficients = ndimage.spline_filter(samples, order=3)
    return ndimage.map_coordinates(coefficients, [positions], order=3, prefilter=False, mode='constant', cval=np.nan)


def calibration(reference, rr, hardware, hr, packets, clock, offset, refine=True):
    # Every packet's common 16-bit preamble is the calibration signal. This
    # source-defined region has identical frequency/volume content across cases
    # and does not depend on the readback or stimulus under investigation.
    indices = []
    for packet in packets:
        indices.append(np.arange(math.ceil(packet['start_s'] * rr),
                                 min(len(reference), math.floor((packet['start_s']+len(SYNC)*BIT_S)*rr))))
    if not indices:
        raise ValueError('No valid reference packets for calibration')
    indices = np.unique(np.concatenate(indices))
    positions = offset * hr + clock * (hr / rr) * indices
    keep = (positions >= 0) & (positions < len(hardware) - 1)
    indices = indices[keep]
    if not len(indices):
        raise ValueError('Calibration packets do not overlap hardware audio')
    r = reference[indices]
    coefficients = ndimage.spline_filter(hardware, order=3)
    # Audio refinement is one global two-parameter mapping. Initial uncertainty
    # is bounded by one protocol-bit period, the native preamble phase ambiguity.
    # The sampling grid supplies the offset-search spacing; no correlation gate.
    center = float(indices.mean() / rr)
    centered = indices / rr - center
    origin = offset + clock * center
    energy = float(np.dot(r, r))
    if energy == 0:
        raise ValueError('Calibration packets have zero reference energy')

    def sampled(delta_s=0.0, drift=0.0):
        return sample_hardware(hardware, (origin + delta_s + (clock + drift) * centered) * hr, coefficients)

    def objective(parameters):
        h = sampled(parameters[0] / hr, parameters[1] / hr / np.ptp(centered))
        if not np.isfinite(h).all():
            return np.inf
        g = np.dot(r, h) / energy
        return float(np.dot(h-g*r, h-g*r) / np.dot(h,h)) if np.dot(h,h) else np.inf

    refinement = {'enabled': bool(refine)}
    initial_h = sample_hardware(hardware, offset*hr + clock*(hr/rr)*indices, coefficients)
    exact_initial_match = np.array_equal(initial_h, r)
    if refine and not exact_initial_match:
        offsets = np.arange(-math.floor(BIT_S * clock * hr / 2), math.ceil(BIT_S * clock * hr / 2) + 1)
        scores = np.array([objective([delta, 0]) for delta in offsets])
        seed = float(offsets[np.argmin(scores)])
        # Parameter2 is additional end-to-end drift in native hardware samples.
        # Bounds follow the same one-bit timing ambiguity, not an accuracy target.
        bound = BIT_S * clock * hr
        fit = optimize.minimize(objective, [seed, 0.0], method='Nelder-Mead',
                                bounds=[(-bound/2,bound/2),(-bound,bound)],
                                options={'xatol': np.sqrt(np.finfo(float).eps),
                                         'fatol': np.finfo(float).eps})
        if not fit.success:
            raise ValueError(f'Global calibration alignment did not converge: {fit.message}')
        delta_s, drift = fit.x[0] / hr, fit.x[1] / hr / np.ptp(centered)
        clock += float(drift)
        offset = float(origin + delta_s - clock * center)
        refinement.update(offset_adjustment_at_center_s=float(delta_s), clock_adjustment=float(drift),
                          objective=float(fit.fun), center_reference_s=center)
    h = sample_hardware(hardware, offset * hr + clock * (hr / rr) * indices, coefficients)
    gain = float(np.dot(r, h) / energy)
    if gain == 0 or not np.isfinite(gain):
        raise ValueError('Calibration gain is zero or non-finite')
    return clock, offset, gain, coefficients, {
        'packet_samples': len(indices), 'gain_hardware_per_reference': gain,
        'polarity': 'inverted' if gain < 0 else 'same',
        'rms_after_calibration': float(np.sqrt(np.mean((h/gain-r)**2))),
        'alignment_refinement': refinement,
        'gain_method': 'One signed least-squares scalar on every covered packet 16-bit preamble; no DC offset or per-case gain fit.'}


def finite_ratio(numerator, denominator):
    return float(numerator / denominator) if denominator else None


def calibration_case_gain(cases, markers, reference, rr, hardware, hr, coefficients, clock, offset, packet_gain):
    candidates=[case for case in cases if case.get('group')=='calibration']
    if len(candidates)!=1:
        return packet_gain, {'source':'packet_preambles','reason':'Manifest does not specify exactly one calibration-group case.'}
    case=candidates[0]; identifier=int(case['id'])
    starts=[m['time_s'] for m in markers if m['event']==2 and m['id']==identifier]
    ends=[m['time_s'] for m in markers if m['event']==3 and m['id']==identifier]
    if len(starts)!=1 or len(ends)!=1:
        raise ValueError('Calibration case needs one stimulus start and end marker')
    indices=np.arange(math.ceil(starts[0]*rr),math.ceil(ends[0]*rr))
    positions=offset*hr+clock*(hr/rr)*indices
    if not len(indices) or indices[-1]>=len(reference) or positions[0]<0 or positions[-1]>=len(hardware):
        return packet_gain, {'source':'packet_preambles','reason':'Manifest calibration case is not fully covered by the recording.'}
    r=reference[indices]
    h=sample_hardware(hardware,positions,coefficients)
    power_r=float(np.mean(r*r));power_h=float(np.mean(h*h))
    if power_r==0 or power_h==0:
        raise ValueError('The specified calibration stimulus has zero signal energy')
    gain=math.copysign(math.sqrt(power_h/power_r),packet_gain)
    return gain, {'source':'manifest_calibration_case','case_id':identifier,'case_name':case['name'],
                   'reference_interval_s':[starts[0],ends[0]], 'reference_rms':math.sqrt(power_r),
                   'hardware_rms':math.sqrt(power_h),
                   'method':'One whole-calibration-case RMS ratio; sign comes from packet preamble correlation. No DC removal, per-case gain, or EQ.'}


def audio_metrics(reference, hardware):
    difference = hardware-reference
    rc, hc = reference-reference.mean(), hardware-hardware.mean()
    return {'samples': len(reference), 'rmse': float(np.sqrt(np.mean(difference**2))),
            'normalized_rmse_reference': finite_ratio(np.linalg.norm(difference), np.linalg.norm(reference)),
            'correlation': finite_ratio(np.dot(rc,hc),np.linalg.norm(rc)*np.linalg.norm(hc)),
            'reference_rms': float(np.sqrt(np.mean(reference**2))),
            'hardware_calibrated_rms': float(np.sqrt(np.mean(hardware**2))),
            'mean_difference': float(difference.mean()), 'peak_absolute_difference': float(abs(difference).max())}


def case_metrics(case, start, end, reference, rr, hardware, hr, coefficients, clock, offset, gain):
    first, last = math.ceil(start*rr), math.ceil(end*rr)
    hstart, hend = offset+clock*start, offset+clock*end
    report = {'id': int(case['id']), 'name': case['name'], 'group': case.get('group',''),
              'description': case.get('description',''), 'reference_interval_s': [start,end],
              'mapped_hardware_interval_s': [hstart,hend]}
    if first < 0 or last > len(reference) or hstart < 0 or hend > len(hardware)/hr:
        report['waveform_status'] = 'partial_or_missing_audio'
        return report, {}
    indices = np.arange(first,last)
    if not len(indices):
        report['waveform_status'] = 'empty_stimulus'
        return report, {}
    r = reference[indices]
    h = sample_hardware(hardware, offset*hr + clock*(hr/rr)*indices, coefficients)/gain
    report['waveform_status'] = 'complete'
    report['waveform'] = audio_metrics(r,h)
    # One GB video frame is a source-defined time scale; the final partial frame
    # remains present. No smoothing window is selected to favor a result.
    boundaries = np.unique(np.r_[0, np.ceil(np.arange(FRAME_S,end-start,FRAME_S)*rr).astype(int),len(r)])
    boundaries = boundaries[(boundaries>=0)&(boundaries<=len(r))]
    envelope_time, re, he = [], [], []
    for a,b in zip(boundaries,boundaries[1:]):
        if b>a:
            envelope_time.append((a+b)/2/rr)
            re.append(np.sqrt(np.mean(r[a:b]**2))); he.append(np.sqrt(np.mean(h[a:b]**2)))
    re,he=np.array(re),np.array(he)
    report['frame_rms_envelope']={'frame_duration_reference_s':FRAME_S,'frames':len(re),
                                   'rmse':float(np.sqrt(np.mean((he-re)**2)))}
    # Spectra are estimated on native audio. Correct only the physical frequency
    # axis and density units using the one clock/gain; do not resample for PSD.
    native = hardware[math.ceil(hstart*hr):math.ceil(hend*hr)]/gain
    arrays={'time_s':indices/rr-start,'reference':r,'hardware_calibrated':h,
            'envelope_time_s':np.array(envelope_time),'reference_frame_rms':re,'hardware_frame_rms':he}
    if min(len(r), len(native)) < 2:
        report['native_spectrum']={'status':'fewer_than_two_native_samples'}
        return report,arrays
    rf,rp=signal.periodogram(r,rr,window='hann',detrend=False)
    hf,hp=signal.periodogram(native,hr*clock,window='hann',detrend=False)
    common=rf<=hf[-1]
    interpolated=np.interp(rf[common],hf,hp)
    spectral_l1=float(np.trapezoid(abs(interpolated-rp[common]),rf[common]))
    reference_power=float(np.trapezoid(rp[common],rf[common]))
    report['native_spectrum']={'window':'whole-segment Hann, no detrending',
      'reference_resolution_hz':float(rf[1]-rf[0]), 'hardware_resolution_reference_hz':float(hf[1]-hf[0]),
      'integrated_absolute_psd_difference':spectral_l1,
      'relative_integrated_absolute_psd_difference':finite_ratio(spectral_l1,reference_power),
      'reference_power':reference_power,'hardware_power':float(np.trapezoid(hp,hf))}
    arrays.update(reference_frequency_hz=rf,reference_psd=rp,
                  hardware_frequency_reference_hz=hf,hardware_psd=hp)
    return report,arrays


def load_markers(path):
    with path.open() as stream:
        rows=list(csv.DictReader(stream))
    return [{'event':int(r['event']), 'id':int(r['case_id']), 'time_s':float(r['run_start_seconds'])} for r in rows]


def packet_coverage(reference, hardware, lengths, cases):
    ru,rduplicates=unique_packets(reference);hu,hduplicates=unique_packets(hardware)
    expected=set(lengths)
    order=[]
    for case in cases:
        identifier=int(case['id'])
        order.append([1,identifier])
        if (2,identifier) in lengths: order.append([2,identifier])
    order.append([3,0xffff])
    observed=[[p['kind'],p['id']] for p in sorted(hardware,key=lambda p:p['start_s'])]
    return {'expected':len(expected),'reference_valid_unique':len(ru),'hardware_valid_unique':len(hu),
      'missing_reference':[[k,i] for k,i in sorted(expected-ru.keys())],
      'missing_hardware':[[k,i] for k,i in sorted(expected-hu.keys())],
      'reference_duplicates':rduplicates,'hardware_duplicates':hduplicates,
      'order_matches_manifest':observed==order,
      'complete':expected<=ru.keys() and expected<=hu.keys() and not rduplicates and not hduplicates and observed==order,
      'suite_end_received':(3,0xffff) in hu}


def make_report(args):
    manifest=json.loads(args.manifest.read_text())
    cases=manifest['cases']
    lengths=protocol_lengths(cases)
    rr,reference,rmeta=read_audio(args.reference,args.channel)
    if args.hardware.resolve()==args.reference.resolve():
        hr,hardware,hmeta=rr,reference,dict(rmeta,path=str(args.hardware))
    else:
        hr,hardware,hmeta=read_audio(args.hardware,args.channel)
    markers=load_markers(args.markers)
    rp,ri,rd=decode_reference(reference,rr,markers,lengths)
    hp,hi,hd=decode_packets(hardware,hr,lengths,args.clock or 1.0)
    report={'reference':rmeta,'hardware':hmeta,'manifest':{'path':str(args.manifest),'sha256':digest(args.manifest)},
      'markers':{'path':str(args.markers),'sha256':digest(args.markers)},
      'protocol':{'sync_hex':'d391','bit_t_cycles':BIT_T,'frequencies_hz':list(FREQUENCIES),
                  'checksum':'XOR kind/id_lo/id_hi/length/payload; excludes sync'},
      'reference_packets':rp,'reference_invalid_candidates':ri,'hardware_packets':hp,'hardware_invalid_candidates':hi,
      'decoder':{'reference':rd,'hardware_initial':hd},
      'packet_coverage':packet_coverage(rp,hp,lengths,cases), 'cases':[],
      'limitations':['Packet validity/coverage is not an analog accuracy pass.',
        'Waveform interpolation uses a cubic spline under one global affine time mapping; native PSD does not use resampled audio.',
        'One signed calibration gain applies to every stimulus. Frequency-dependent analog transfer remains in the residual.',
        'An invalid preamble candidate may be stimulus audio resembling sync; it is retained, not silently assigned to a case.']}
    try:
        clock,offset,anchors=affine_fit(rp,hp,args.clock,args.offset)
        identity_input=rmeta['sha256']==hmeta['sha256'] and rr==hr
        if identity_input and args.clock is None and args.offset is None:
            # Identical files have a known identity mapping. Avoid introducing
            # numerical packet-fit drift into an exact-null control; retain the
            # independently decoded anchor residuals in the report.
            clock,offset=1.0,0.0
        # The first fit supplies the measured bit clock for a second protocol
        # decode. This corrects accumulated phase within readback packets.
        if args.clock is None and clock != 1:
            hp,hi,hd=decode_packets(hardware,hr,lengths,clock)
            report.update(hardware_packets=hp,hardware_invalid_candidates=hi)
            report['decoder']['hardware_clock_corrected']=hd
            clock,offset,anchors=affine_fit(rp,hp,args.clock,args.offset)
        ru,rduplicates=unique_packets(rp); hu,hduplicates=unique_packets(hp)
        calibration_packets=[ru[k] for k in ru.keys()&hu.keys()]
        if not calibration_packets and args.clock is not None and args.offset is not None:
            calibration_packets=list(ru.values())
            report['limitations'].append('Explicit mapping fallback calibrates mapped reference packet regions without decoded hardware identities; inspect packet coverage before interpreting measurements.')
        clock,offset,gain,coefficients,cal=calibration(reference,rr,hardware,hr,calibration_packets,clock,offset,
                                                     refine=not args.no_refine and args.offset is None and args.clock is None)
        packet_gain=gain
        gain,gain_source=calibration_case_gain(cases,markers,reference,rr,hardware,hr,coefficients,clock,offset,packet_gain)
        cal['preamble_fit_gain_hardware_per_reference']=packet_gain
        cal['preamble_rms_after_its_fitted_gain']=cal.pop('rms_after_calibration')
        cal['gain_hardware_per_reference']=gain
        cal['gain_source']=gain_source
        cal['gain_method']=gain_source.get('method',cal['gain_method'])
        report['alignment']={'hardware_s_equals':'offset_s + clock * reference_s', 'clock':clock,'offset_s':offset,
                             'clock_difference_ppm':(clock-1)*1e6,'packet_anchor_fit':anchors,'calibration':cal,
                             'explicit_clock':args.clock,'explicit_offset_s':args.offset,
                             'identical_file_identity_mapping':identity_input and args.clock is None and args.offset is None}
    except ValueError as error:
        report['alignment']={'status':'unavailable','reason':str(error)}
        report['complete_hardware']=False
        report['cases']=[{'id':int(case['id']),'name':case['name'],'waveform_status':'alignment_unavailable'} for case in cases]
        return report,{}
    report['packet_coverage']=packet_coverage(rp,hp,lengths,cases)
    arrays={}
    for case in cases:
        identifier=int(case['id'])
        starts=[m['time_s'] for m in markers if m['event']==2 and m['id']==identifier]
        ends=[m['time_s'] for m in markers if m['event']==3 and m['id']==identifier]
        if len(starts)!=1 or len(ends)!=1 or ends[0]<=starts[0]:
            result={'id':identifier,'name':case['name'],'waveform_status':'invalid_reference_marker_interval'}
            data={}
        else:
            result,data=case_metrics(case,starts[0],ends[0],reference,rr,hardware,hr,coefficients,clock,offset,gain)
        result['case_marker_received']=(1,identifier) in hu
        rread,hread=ru.get((2,identifier)),hu.get((2,identifier))
        if (2,identifier) not in lengths:
            result['readback']={'status':'not_emitted_by_this_case'}
        elif rread is None or hread is None:
            result['readback']={'status':'missing_or_ambiguous_packet','reference_present':rread is not None,'hardware_present':hread is not None}
        else:
            labels=case.get('readback_labels',[])
            mismatch=[{'index':i,'label':labels[i] if i<len(labels) else str(i),'reference':r,'hardware':h}
                      for i,(r,h) in enumerate(zip(rread['payload'],hread['payload'])) if r!=h]
            result['readback']={'status':'different' if mismatch else 'identical','reference':rread['payload'],
                                'hardware':hread['payload'],'differences':mismatch}
        report['cases'].append(result)
        arrays.update({f'case_{identifier}_{key}':value for key,value in data.items()})
    report['complete_hardware']=report['packet_coverage']['complete'] and all(
        case['waveform_status']=='complete' for case in report['cases'])
    return report,arrays


def save_report(report,arrays,out):
    out.mkdir(parents=True,exist_ok=True)
    (out/'metrics.json').write_text(json.dumps(report,indent=2,allow_nan=False)+'\n')
    np.savez_compressed(out/'signals.npz',**arrays)
    columns=['id','name','group','waveform_status','case_marker_received','readback_status','correlation',
             'normalized_rmse_reference','rmse','mean_difference','envelope_rmse','relative_psd_difference']
    with (out/'cases.csv').open('w',newline='') as stream:
        writer=csv.DictWriter(stream,fieldnames=columns);writer.writeheader()
        for case in report['cases']:
            row={key:case.get(key) for key in columns if key in case}
            row.update(readback_status=case.get('readback',{}).get('status'))
            row.update({key:case.get('waveform',{}).get(key) for key in ['correlation','normalized_rmse_reference','rmse','mean_difference']})
            row['envelope_rmse']=case.get('frame_rms_envelope',{}).get('rmse')
            row['relative_psd_difference']=case.get('native_spectrum',{}).get('relative_integrated_absolute_psd_difference')
            writer.writerow(row)
    rows=[]
    for case in report['cases']:
        waveform=case.get('waveform',{})
        rows.append('<tr>'+''.join('<td>'+html.escape(str(value))+'</td>' for value in [case['id'],case['name'],case.get('waveform_status'),case.get('readback',{}).get('status'),waveform.get('correlation'),waveform.get('normalized_rmse_reference')])+'</tr>')
    (out/'index.html').write_text('<!doctype html><meta charset="utf-8"><title>DMG torture audio comparison</title>'
        '<style>body{font:16px system-ui;max-width:1200px;margin:40px auto;padding:0 20px}td,th{padding:8px;border-bottom:1px solid #ddd;text-align:left}pre{white-space:pre-wrap}</style>'
        '<h1>DMG torture audio comparison</h1><p>Numerical measurements have no accuracy pass threshold. Readback identity and packet coverage are reported separately.</p>'
        '<p><a href="metrics.json">Full measurements and packet diagnostics</a> · <a href="cases.csv">Case CSV</a> · <a href="signals.npz">Native spectra and aligned signals</a></p>'
        '<h2>Alignment and calibration</h2><pre>'+html.escape(json.dumps(report.get('alignment'),indent=2))+'</pre>'
        '<h2>Packet coverage</h2><p>Complete hardware capture: '+str(report.get('complete_hardware',False))+'</p><pre>'+html.escape(json.dumps(report.get('packet_coverage'),indent=2))+'</pre>'
        '<table><tr><th>ID</th><th>Case</th><th>Audio coverage</th><th>Readback</th><th>Correlation</th><th>Relative RMSE</th></tr>'+''.join(rows)+'</table>'
        '<h2>Interpretation limits</h2><ul>'+''.join('<li>'+html.escape(x)+'</li>' for x in report['limitations'])+'</ul>')


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--reference',type=Path,required=True)
    parser.add_argument('--markers',type=Path,required=True)
    parser.add_argument('--manifest',type=Path,required=True)
    parser.add_argument('--hardware',type=Path,required=True)
    parser.add_argument('--out',type=Path,required=True)
    parser.add_argument('--channel',type=int,default=0,help='Zero-based channel; default left (0)')
    parser.add_argument('--clock',type=float,help='Explicit hardware seconds per reference second fallback')
    parser.add_argument('--offset',type=float,help='Explicit hardware time at reference time zero, seconds')
    parser.add_argument('--no-refine',action='store_true',help='Use packet-timestamp affine fit without global waveform refinement')
    args=parser.parse_args()
    report,arrays=make_report(args)
    save_report(report,arrays,args.out)
    alignment=report.get('alignment',{})
    brief_alignment={key:alignment.get(key) for key in ('clock','offset_s','clock_difference_ppm','status','reason') if key in alignment}
    brief_alignment['gain_hardware_per_reference']=alignment.get('calibration',{}).get('gain_hardware_per_reference')
    print(json.dumps({'alignment':brief_alignment,'packet_coverage':report.get('packet_coverage'),
                      'complete_hardware':report.get('complete_hardware',False),
                      'cases':len(report['cases']),'report':str(args.out/'index.html')},indent=2))
    if report.get('alignment',{}).get('status')=='unavailable':
        raise SystemExit(2)


if __name__=='__main__':
    main()
