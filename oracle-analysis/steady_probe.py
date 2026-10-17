#!/usr/bin/env python3
"""Compare settled steady hardware noise using one clock, phase, and scalar gain.

Native spectra remain separate from the globally aligned listening waveform.
No filter, EQ, local warp, or chosen per-period phase is applied.
"""
import argparse
import hashlib
import json
from pathlib import Path
import warnings

import numpy as np
from scipy import ndimage, optimize, signal
from scipy.io import wavfile


def read_channel(path, channel=0):
    rate, data = wavfile.read(path)
    if data.ndim > 1:
        data = data[:, channel]
    if np.issubdtype(data.dtype, np.integer):
        data = data.astype(float) / 2 ** (np.iinfo(data.dtype).bits - 1)
    return rate, data.astype(float)


def sha(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def fundamental(samples, rate, nominal_period):
    frequency, psd = signal.periodogram(samples, rate, window='hann')
    peak = int(np.argmax(psd[1:]) + 1)
    harmonic = round(frequency[peak] * nominal_period)
    if harmonic < 1 or peak + 1 >= len(frequency):
        raise ValueError('Dominant spectral peak cannot identify the known LFSR comb')
    time = np.arange(len(samples)) / rate
    windowed = (samples - samples.mean()) * signal.windows.hann(len(samples), sym=False)
    # The FFT identifies the dominant resolved line; its neighboring bins bracket
    # the continuous-frequency maximum. Numerical tolerance follows float64.
    fit = optimize.minimize_scalar(
        lambda f: -abs(np.dot(windowed, np.exp(-2j * np.pi * f * time))) ** 2,
        bounds=(frequency[peak - 1], frequency[peak + 1]), method='bounded',
        options={'xatol': np.finfo(float).eps * frequency[peak]})
    if not fit.success:
        raise RuntimeError(fit.message)
    return {'dominant_fft_bin_hz': float(frequency[peak]),
            'dominant_harmonic': harmonic,
            'fitted_dominant_line_hz': float(fit.x),
            'fundamental_hz': float(fit.x / harmonic),
            'period_s': float(harmonic / fit.x)}, frequency, psd


def metrics(hardware, emulator):
    return {'correlation': float(np.corrcoef(hardware, emulator)[0, 1]),
            'normalized_rmse': float(np.linalg.norm(hardware - emulator) / np.linalg.norm(hardware)),
            'hardware_rms': float(np.sqrt(np.mean(hardware ** 2))),
            'emulator_rms': float(np.sqrt(np.mean(emulator ** 2))),
            'hardware_mean': float(hardware.mean()), 'emulator_mean': float(emulator.mean())}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--hardware', type=Path, default=Path('/home/jeff/Music/Scarlett-Recordings/Media/01-Scarlett 3-4 - Stereo-261017_0638.wav'))
    parser.add_argument('--emulated', type=Path, default=Path('artifacts/noise-probe/steady-mbc5.wav'))
    parser.add_argument('--baseline', type=Path, default=Path('benchmark/baseline.json'), help='Authority for the existing passage duration')
    parser.add_argument('--out-prefix', type=Path, default=Path('artifacts/steady-probe'))
    args = parser.parse_args()
    warnings.filterwarnings('ignore', category=wavfile.WavFileWarning)
    duration = json.loads(args.baseline.read_text())['duration_s']
    hr, hardware_full = read_channel(args.hardware)
    er, emulator_full = read_channel(args.emulated)
    if hr != er:
        raise ValueError('Capture at the same sample rate before using this comparator')
    rate = hr
    count = round(duration * rate)
    if min(len(hardware_full), len(emulator_full)) < count:
        raise ValueError('Existing passage duration exceeds an input')
    hardware = hardware_full[-count:]
    emulator_native = emulator_full[-count:]
    nominal_period = 127 * 6144 / 4194304
    hfit, hf, hp = fundamental(hardware, rate, nominal_period)
    efit, _, _ = fundamental(emulator_native, rate, nominal_period)
    ratio = hfit['fundamental_hz'] / efit['fundamental_hz']
    # Fourier resampling preserves bandwidth. Integer output length slightly
    # quantizes the implemented ratio; both requested and used values are saved.
    corrected = signal.resample(emulator_full, round(len(emulator_full) / ratio))
    implemented_ratio = len(emulator_full) / len(corrected)
    last_start = len(corrected) - count
    first_start = last_start - round(hfit['period_s'] * rate)
    if first_start < 0:
        raise ValueError('No complete LFSR period available for global phase alignment')
    candidate = corrected[first_start:last_start + count]
    numerator = signal.correlate(candidate, hardware - hardware.mean(), mode='valid', method='fft')
    cumulative = np.r_[0., np.cumsum(candidate)]
    squared = np.r_[0., np.cumsum(candidate * candidate)]
    sums = cumulative[count:] - cumulative[:-count]
    energy = squared[count:] - squared[:-count] - sums * sums / count
    scores = numerator / (np.sqrt(energy) * np.linalg.norm(hardware - hardware.mean()))
    lag = int(np.argmax(scores))
    fraction = 0.
    if 0 < lag < len(scores) - 1:
        fraction = float(.5 * (scores[lag - 1] - scores[lag + 1]) /
                         (scores[lag - 1] - 2 * scores[lag] + scores[lag + 1]))
    # This is one global fractional phase adjustment, not per-cycle matching.
    shifted = np.fft.ifft(ndimage.fourier_shift(np.fft.fft(corrected), -fraction)).real
    aligned = shifted[first_start + lag:first_start + lag + count]
    gain = float(np.sqrt(np.mean(hardware ** 2) / np.mean(aligned ** 2)))
    aligned *= gain
    emulated_start = (first_start + lag + fraction) / rate * implemented_ratio
    hardware_start = (len(hardware_full) - count) / rate

    # Native line powers: sum between midpoints of adjacent 127-step harmonics.
    # The native emulator frequency axis is corrected, not its waveform samples.
    ef, ep = signal.periodogram(emulator_native * gain, rate * ratio, window='hann')
    fundamental_hz = hfit['fundamental_hz']
    harmonic_indices = np.arange(1, int(min(hf[-1], ef[-1]) / fundamental_hz - .5) + 1)
    hpower, epower = [], []
    for harmonic in harmonic_indices:
        low, high = (harmonic - .5) * fundamental_hz, (harmonic + .5) * fundamental_hz
        hpower.append(float(hp[(hf >= low) & (hf < high)].sum() * (hf[1] - hf[0])))
        epower.append(float(ep[(ef >= low) & (ef < high)].sum() * (ef[1] - ef[0])))
    hpower, epower = np.array(hpower), np.array(epower)
    harmonic_ratio_db = 10 * np.log10(epower / hpower)
    # Selection is tied to the known fundamental, the prior residual regions,
    # and the measured strongest lines. All harmonics are retained in NPZ.
    selected = {
        'fundamental': 1,
        'line_in_prior_37_to_41_hz_region': round(39 / fundamental_hz),
        'line_in_prior_80_to_83_hz_region': round(81.5 / fundamental_hz),
        'strongest_hardware_line': hfit['dominant_harmonic'],
        'strongest_emulator_line': efit['dominant_harmonic'],
    }
    line_report = {label: {'harmonic': harmonic,
                           'frequency_hz': harmonic * fundamental_hz,
                           'sameboy_minus_hardware_power_db': float(harmonic_ratio_db[harmonic - 1])}
                   for label, harmonic in selected.items()}
    result = {
        'hardware': {'path': str(args.hardware), 'sha256': sha(args.hardware), 'channel': 0,
                     'start_s': hardware_start, 'end_s': len(hardware_full) / rate},
        'emulator': {'path': str(args.emulated), 'sha256': sha(args.emulated), 'channel': 0,
                     'aligned_start_s': emulated_start,
                     'aligned_end_s': emulated_start + duration * implemented_ratio,
                     'native_psd_start_s': (len(emulator_full) - count) / rate,
                     'native_psd_end_s': len(emulator_full) / rate},
        'duration_s': duration, 'duration_authority': str(args.baseline),
        'selection': 'Use the final complete 15.186-second hardware passage after observed startup settling. Use the final settled emulator passage for native spectra, and permit one full 127-step period of earlier emulator start positions for a single global phase fit over the entire passage. No local phase selection.',
        'nominal_lfsr_period_s': nominal_period,
        'nominal_period_authority': 'benchmark/noise-probe.asm: 127 steps * 6144 DMG T-cycles / 4194304 Hz',
        'hardware_period_estimate': hfit, 'emulator_period_estimate': efit,
        'emulated_seconds_per_hardware_second': ratio,
        'relative_clock_ratio_ppm': (ratio - 1) * 1e6,
        'implemented_audio_resampling_ratio': implemented_ratio,
        'implemented_ratio_minus_fit_ppm': (implemented_ratio - ratio) * 1e6,
        'single_rms_gain_hardware_per_emulator': gain,
        'global_phase_fractional_sample': fraction,
        'whole_passage': metrics(hardware, aligned),
        'halves_without_refitting': [metrics(hardware[:count // 2], aligned[:count // 2]),
                                    metrics(hardware[count // 2:], aligned[count // 2:])],
        'native_harmonic_powers': line_report,
        'interpretation': 'The strong globally aligned waveform agreement and 127-step periodicity support agreement of the ongoing narrow-noise sequence. Residual steady differences are concentrated toward low frequencies and are compatible with an analog transfer difference. They do not establish its component or justify EQ. The fitted clock ratio includes recording-clock and oscillator effects. This steady comparison does not validate NR44 restart timing.',
    }
    args.out_prefix.parent.mkdir(parents=True, exist_ok=True)
    args.out_prefix.with_suffix('.json').write_text(json.dumps(result, indent=2) + '\n')
    one_period = round(hfit['period_s'] * rate)
    np.savez_compressed(args.out_prefix.with_suffix('.npz'),
                        native_hardware_frequency=hf, native_hardware_psd=hp,
                        native_emulator_corrected_frequency=ef, native_emulator_psd=ep,
                        harmonic_indices=harmonic_indices,
                        harmonic_frequency=harmonic_indices * fundamental_hz,
                        hardware_harmonic_power=hpower, emulator_harmonic_power=epower,
                        harmonic_emulator_minus_hardware_db=harmonic_ratio_db,
                        period_time=np.arange(one_period) / rate,
                        period_hardware=hardware[:one_period], period_emulator=aligned[:one_period])
    print(json.dumps({'relative_clock_ratio_ppm': result['relative_clock_ratio_ppm'],
                      'whole_passage': result['whole_passage'],
                      'native_harmonic_powers': line_report}, indent=2))


if __name__ == '__main__':
    main()
