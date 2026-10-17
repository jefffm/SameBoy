#!/usr/bin/env python3
"""Compare every complete cycle in a settled NR44-only hardware probe passage.

Cadence and reset phase come independently from each recording. Native derivative
extrema measure pulse widths. No local waveform alignment, amplitude threshold,
phase selection, EQ, resampling, or rejection of individual cycles is used.
"""
import argparse
import csv
import hashlib
import json
from math import gcd
from pathlib import Path
import warnings

import numpy as np
from scipy.io import wavfile

from steady_probe import fundamental, read_channel

CPU_HZ = 4194304
RESTART_T = 105336
LFSR_T = 6144
INITIAL_BOUNDS_T = (36916, 43012)
BASE_INITIAL_T = 52
BASE_PERIOD_T = 48
COUNTER_ADVANCE = (RESTART_T - BASE_INITIAL_T) // BASE_PERIOD_T + 1
COUNTER_MODULUS = 128
PHASE_ADVANCE = COUNTER_ADVANCE % COUNTER_MODULUS
PHASE_CYCLE = COUNTER_MODULUS // gcd(PHASE_ADVANCE, COUNTER_MODULUS)


def sha(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def summary(values):
    values = np.asarray(values)
    return {'minimum': float(values.min()), 'median': float(np.median(values)),
            'mean': float(values.mean()), 'maximum': float(values.max())}


def inspect(path, duration):
    rate, complete = read_channel(path)
    count = round(duration * rate)
    if count > len(complete):
        raise ValueError('Input is shorter than the established comparison duration')
    samples = complete[-count:]
    start_s = (len(complete) - count) / rate
    spectral, _, _ = fundamental(samples, rate, RESTART_T / CPU_HZ)
    initial_period = spectral['period_s']
    initial_step = initial_period * LFSR_T / RESTART_T
    derivative = np.diff(samples)
    time = (np.arange(len(derivative)) + .5) / rate
    # One display bin per native sample of the measured cadence. Folding every
    # derivative sample identifies the sharp repeated reset edge independently
    # of the free-running LFSR phase; its later transitions move across the fold.
    bins = round(initial_period * rate)
    index = np.floor((time / initial_period % 1) * bins).astype(int)
    coverage = np.bincount(index, minlength=bins)
    folded_derivative = np.bincount(index, weights=derivative, minlength=bins) / coverage
    reset_phase = (int(np.argmax(folded_derivative)) + .5) / bins * initial_period
    cycles = np.arange(np.ceil((initial_step / 2 - reset_phase) / initial_period),
                       np.floor((time[-1] - initial_step / 2 - reset_phase) / initial_period) + 1,
                       dtype=int)
    predicted = reset_phase + cycles * initial_period
    resets = []
    for reset in predicted:
        # One LFSR-clock-wide neighborhood centered on the observed reset phase.
        # The half-clock boundaries isolate this repeated edge; no amplitude gate.
        low = int(np.ceil((reset - initial_step / 2) * rate - .5))
        high = int(np.floor((reset + initial_step / 2) * rate - .5)) + 1
        resets.append(low + int(np.argmax(derivative[low:high])))
    resets = np.array(resets)
    reset_times = (resets + .5) / rate
    cadence, phase = np.polyfit(cycles, reset_times, 1)
    step = float(cadence * LFSR_T / RESTART_T)
    cadence_residual = reset_times - (phase + cadence * cycles)
    observations = []
    for reset, following_reset in zip(resets, resets[1:]):
        # With the baseline zero-seeded narrow LFSR, changes occur at steps 7,
        # 13,14. Search the
        # first falling edge before the earliest step13 edge, then the subsequent
        # opposite edges. Every full cycle is retained, including all extrema.
        before_step13 = np.arange(reset + 1, min(following_reset, reset + round(12 * step * rate)))
        falling = int(before_step13[np.argmin(derivative[before_step13])])
        after_falling = np.arange(falling + 1, following_reset)
        rising = int(after_falling[np.argmax(derivative[after_falling])])
        after_rising = np.arange(rising + 1, following_reset)
        if not len(after_rising):
            raise ValueError('Complete probe cycle does not contain the expected subsequent falling edge')
        following_fall = int(after_rising[np.argmin(derivative[after_rising])])
        observations.append([
            start_s + (reset + .5) / rate,
            start_s + (falling + .5) / rate,
            start_s + (rising + .5) / rate,
            start_s + (following_fall + .5) / rate,
            (falling - reset) / rate,
            (rising - falling) / rate,
            (following_fall - rising) / rate,
            derivative[reset], derivative[falling], derivative[rising], derivative[following_fall],
        ])
    observations = np.array(observations)
    first_widths = observations[:, 4]
    widths_clocks = first_widths / step
    # Derivation: 2194 base-counter ticks advance bit6 phase by 18 mod128;
    # 128/gcd(18,128)=64. All observed positions receive equal aggregate weight,
    # independent of whether this passage ends partway through the phase cycle.
    phase_means = np.array([np.mean(widths_clocks[position::PHASE_CYCLE]) for position in range(PHASE_CYCLE)])
    phase_counts = np.array([len(widths_clocks[position::PHASE_CYCLE]) for position in range(PHASE_CYCLE)])
    phase_spans = np.array([np.ptp(widths_clocks[position::PHASE_CYCLE]) for position in range(PHASE_CYCLE)])
    repeat_difference = first_widths[PHASE_CYCLE:] - first_widths[:-PHASE_CYCLE]
    wrapped_increments = (np.diff(widths_clocks) + .5) % 1 - .5
    upper_bound = INITIAL_BOUNDS_T[1] / LFSR_T * step
    report = {
        'path': str(path), 'sha256': sha(path), 'sample_rate': rate, 'channel': 0,
        'full_file_duration_s': len(complete) / rate,
        'selected_interval_s': [start_s, len(complete) / rate],
        'selected_rms_by_half': [float(np.sqrt(np.mean(part ** 2))) for part in np.array_split(samples, 2)],
        'complete_cycles': len(observations),
        'spectral_cadence_estimate': spectral,
        'edge_fitted_cadence_s': float(cadence),
        'edge_cadence_fit_residual_s': summary(cadence_residual),
        'inferred_lfsr_clock_s': step,
        'initial_pulse_width_s': summary(first_widths),
        'initial_pulse_width_lfsr_clocks': summary(widths_clocks),
        'following_six_clock_run_s': summary(observations[:, 5]),
        'following_one_clock_run_s': summary(observations[:, 6]),
        'initial_reset_rise_to_first_fall_derivative_ratio': summary(observations[:, 7] / abs(observations[:, 8])),
        'baseline_model_initial_upper_bound_s': upper_bound,
        'raw_count_above_baseline_model_upper_bound': int(np.count_nonzero(first_widths > upper_bound)),
        'maximum_excess_over_baseline_model_bound_s': float(first_widths.max() - upper_bound),
        'native_sample_interval_s': 1 / rate,
        'same_phase_width_difference_after_64_resets_s': summary(repeat_difference),
        'wrapped_width_change_per_reset_clocks': summary(wrapped_increments),
        'phase_balanced_initial_width_clocks': float(phase_means.mean()),
        'phase_mean_width_range_clocks': [float(phase_means.min()), float(phase_means.max())],
        'maximum_within_phase_width_span_clocks': float(phase_spans.max()),
        'phase_counts_range': [int(phase_counts.min()), int(phase_counts.max())],
    }
    arrays = {'observations': observations, 'phase_means_clocks': phase_means,
              'phase_counts': phase_counts, 'phase_spans_clocks': phase_spans,
              'reset_times_s': start_s + reset_times,
              'reset_cadence_residual_s': cadence_residual,
              'folded_derivative': folded_derivative,
              'folded_time_s': (np.arange(bins) + .5) / bins * initial_period,
              'folded_coverage': coverage}
    return report, arrays


def sequence_comparison(hardware, ha, candidate, ca):
    """Choose a single cyclic phase, then retain the entire observed sequence."""
    hw = ha['observations'][:, 4] / hardware['inferred_lfsr_clock_s']
    em = ca['observations'][:, 4] / candidate['inferred_lfsr_clock_s']
    hw_mean, em_mean = ha['phase_means_clocks'], ca['phase_means_clocks']
    mean_scores = np.array([np.sqrt(np.mean((hw_mean - np.roll(em_mean, shift)) ** 2))
                            for shift in range(PHASE_CYCLE)])
    shift = int(np.argmin(mean_scores))
    template = np.roll(em_mean, shift)
    phase_difference = hw_mean - template
    full_difference = hw - template[np.arange(len(hw)) % PHASE_CYCLE]
    # The held-out boundary is one complete source-derived phase pattern. Fit
    # only the first such pattern, then repeat it without refitting any event.
    first_scores = np.array([np.sqrt(np.mean((hw[:PHASE_CYCLE] - np.roll(em[:PHASE_CYCLE], s)) ** 2))
                             for s in range(PHASE_CYCLE)])
    first_shift = int(np.argmin(first_scores))
    first_template = np.roll(em[:PHASE_CYCLE], first_shift)
    later_difference = hw[PHASE_CYCLE:] - first_template[np.arange(PHASE_CYCLE, len(hw)) % PHASE_CYCLE]
    scale = hardware['inferred_lfsr_clock_s']

    def error(values):
        return {'lfsr_clocks': summary(values),
                'rms_lfsr_clocks': float(np.sqrt(np.mean(values ** 2))),
                'rms_hardware_s': float(np.sqrt(np.mean(values ** 2)) * scale)}

    report = {
        'method': 'One cyclic offset over all 64 chronological phase means minimizes RMS width error. Each file uses its independently measured cadence. No width offset, gain, or event-specific phase is fitted. A separate check fits the offset using only the first complete 64-cycle pattern and repeats that unchanged template for every later hardware cycle.',
        'candidate_seconds_per_hardware_second': candidate['edge_fitted_cadence_s'] / hardware['edge_fitted_cadence_s'],
        'global_cyclic_offset': shift,
        'phase_mean_error': error(phase_difference),
        'all_native_hardware_cycles_against_phase_mean_template': error(full_difference),
        'first_pattern_check': {
            'training_cycles': PHASE_CYCLE, 'global_cyclic_offset': first_shift,
            'training_rms_clocks': float(first_scores[first_shift]),
            'held_out_cycles': len(later_difference), 'held_out_error': error(later_difference),
        },
    }
    arrays = {'cyclic_phase_mean_rms_clocks': mean_scores,
              'aligned_phase_mean_template_clocks': template,
              'phase_mean_error_clocks': phase_difference,
              'all_native_cycle_error_clocks': full_difference,
              'first_pattern_cyclic_rms_clocks': first_scores,
              'held_out_error_clocks': later_difference}
    return report, arrays


def plot_comparison(path, hardware, ha, ea, comparison, candidate_label):
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt

    figure, axes = plt.subplots(2, 1, figsize=(12, 8), constrained_layout=True)
    positions = np.arange(PHASE_CYCLE)
    baseline_shift = int(np.argmin([np.mean((ha['phase_means_clocks'] - np.roll(ea['phase_means_clocks'], shift)) ** 2)
                                   for shift in range(PHASE_CYCLE)]))
    axes[0].plot(positions, np.roll(ea['phase_means_clocks'], baseline_shift), '.-',
                 color='#bd5427', label='Baseline SameBoy (one cyclic offset)')
    axes[0].plot(positions, comparison['aligned_phase_mean_template_clocks'], 'o',
                 markerfacecolor='none', color='#3266b3', label=candidate_label + ' (one cyclic offset)')
    axes[0].plot(positions, ha['phase_means_clocks'], '.', color='#182c22', label='DMG hardware')
    axes[0].set(xlabel='Chronological reset position in the 64-reset phase pattern',
                ylabel='Initial high pulse (LFSR clocks)',
                title='All 64 restart phases: candidate reproduces the measured width sequence')
    axes[0].legend(loc='lower right')
    scale = hardware['inferred_lfsr_clock_s'] * 1e6
    axes[1].plot(np.arange(len(ha['observations'])), comparison['all_native_cycle_error_clocks'] * scale,
                 '.', color='#3266b3', label='Every hardware cycle minus the single candidate phase template')
    axes[1].axhline(0, color='black', linewidth=.7)
    sample_us = hardware['native_sample_interval_s'] * 1e6
    axes[1].axhline(sample_us, color='#777777', linestyle=':', label='One native hardware sample (20.83 µs)')
    axes[1].axhline(-sample_us, color='#777777', linestyle=':')
    axes[1].set(xlabel='Chronological complete hardware cycle (none rejected)',
                ylabel='Initial width difference (µs)',
                title='Full sequence after one global cyclic offset; no per-event correction')
    axes[1].legend(loc='lower right')
    for axis in axes:
        axis.grid(alpha=.2)
    path.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(path, dpi=160)
    plt.close(figure)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--hardware', type=Path, default=Path('artifacts/hardware/retrigger-wqowpq82/capture.wav'))
    parser.add_argument('--emulated', type=Path, default=Path('artifacts/noise-probe/retrigger-mbc5.wav'))
    parser.add_argument('--trace', type=Path, default=Path('artifacts/noise-probe/retrigger-mbc5.csv'))
    parser.add_argument('--baseline', type=Path, default=Path('benchmark/baseline.json'))
    parser.add_argument('--prototype', type=Path, help='Optional candidate rendering of the identical probe')
    parser.add_argument('--candidate-label', default='Feedback latch candidate', help='Candidate display name in the figure')
    parser.add_argument('--plot', type=Path, help='Optional comparison figure (requires --prototype and matplotlib)')
    parser.add_argument('--out-prefix', type=Path, default=Path('artifacts/retrigger-hardware'))
    args = parser.parse_args()
    warnings.filterwarnings('ignore', category=wavfile.WavFileWarning)
    duration = json.loads(args.baseline.read_text())['duration_s']
    with args.trace.open() as stream:
        trace = list(csv.DictReader(stream))
    noise_writes = [row for row in trace if int(row['address'], 16) in (0xff21, 0xff22, 0xff23)]
    controls = [row for row in noise_writes if int(row['address'], 16) != 0xff23]
    if [(int(row['address'], 16), int(row['value'], 16)) for row in controls] != [(0xff21, 0x98), (0xff22, 0x6e)]:
        raise ValueError('Trace is not the specified fixed-NR42/NR43 probe')
    trigger_ticks = np.array([int(row['run_start_tick_8mhz']) for row in noise_writes if int(row['address'], 16) == 0xff23])
    if not np.all(np.diff(trigger_ticks) == 2 * RESTART_T):
        raise ValueError('Trace does not have the source-defined constant NR44 cadence')
    hardware, ha = inspect(args.hardware, duration)
    emulator, ea = inspect(args.emulated, duration)
    sorted_difference = np.sort(ha['phase_means_clocks']) - np.sort(ea['phase_means_clocks'])
    balanced_difference = hardware['phase_balanced_initial_width_clocks'] - emulator['phase_balanced_initial_width_clocks']
    report = {
        'method': __doc__, 'duration_s': duration, 'duration_authority': str(args.baseline),
        'selection': 'Final complete passage of the baseline-defined duration in each file. Hardware startup settling is earlier than this passage; the button marker is not used as an acoustic onset. Hardware cadence and reset phase are measured from audio independently of the emulator.',
        'trace': {'path': str(args.trace), 'sha256': sha(args.trace),
                  'fixed_controls': {'NR42': '0x98', 'NR43': '0x6e'},
                  'constant_nr44_cadence_t_cycles': RESTART_T,
                  'observed_triggers': len(trigger_ticks)},
        'model': {'cpu_hz': CPU_HZ, 'lfsr_clock_t_cycles': LFSR_T,
                  'initial_high_run_t_cycle_bounds': list(INITIAL_BOUNDS_T),
                  'following_run_clock_lengths': [6, 1],
                  'base_counter_initial_t_cycles': BASE_INITIAL_T,
                  'base_counter_subsequent_t_cycles': BASE_PERIOD_T,
                  'base_counter_ticks_per_restart': COUNTER_ADVANCE,
                  'counter_phase_advance_mod128': PHASE_ADVANCE,
                  'phase_pattern_restarts': PHASE_CYCLE,
                  'authority': 'benchmark/noise-probe.asm cadence 105336T. Baseline SameBoy Core/apu.c zero-seed narrow step_lfsr changes at steps 7/13/14; restart initial countdown 52T then 48T. 2194 base ticks advance selected-bit6 phase 18 mod128; 128/gcd(18,128)=64.'},
        'hardware': hardware, 'emulator': emulator,
        'comparison': {
            'emulated_seconds_per_hardware_second': emulator['edge_fitted_cadence_s'] / hardware['edge_fitted_cadence_s'],
            'relative_clock_ratio_ppm': (emulator['edge_fitted_cadence_s'] / hardware['edge_fitted_cadence_s'] - 1) * 1e6,
            'phase_balanced_initial_width_difference_clocks': balanced_difference,
            'phase_balanced_difference_nominal_s': balanced_difference * LFSR_T / CPU_HZ,
            'sorted_all_64_phase_mean_differences_clocks': summary(sorted_difference),
        },
        'observations_columns': ['reset_rise_file_s', 'first_fall_file_s', 'following_rise_file_s',
                                'following_fall_file_s', 'initial_high_width_s', 'following_low_width_s',
                                'following_high_width_s', 'reset_rise_derivative', 'first_fall_derivative',
                                'following_rise_derivative', 'following_fall_derivative'],
        'interpretation': 'The minimal NR44-only hardware probe reproduces an initial pulse extended by approximately half an LFSR clock across the complete 64-phase distribution, while subsequent 6-clock/1-clock runs and 64-reset phase evolution agree. Counts above a digital bound are descriptive; the native sampling grid can put an emulator measurement slightly above it. Global latency, clock-rate error, or a uniform rising-versus-falling edge delay cannot explain an extension confined to the initial pulse. This establishes the minimal output mismatch. Candidate agreement on this fixed-NR43 probe does not establish correctness for NR43 changes, other noise divisors, or other models.',
    }
    arrays = {**{'hardware_' + key: value for key, value in ha.items()},
              **{'emulator_' + key: value for key, value in ea.items()},
              'sorted_phase_mean_difference_clocks': sorted_difference}
    if args.prototype:
        prototype, pa = inspect(args.prototype, duration)
        prototype_trace = args.prototype.with_suffix('.csv')
        if prototype_trace.read_bytes() != args.trace.read_bytes():
            raise ValueError('Candidate APU trace differs from the baseline probe trace')
        comparison, ca = sequence_comparison(hardware, ha, prototype, pa)
        report['prototype'] = prototype
        report['candidate_label'] = args.candidate_label
        report['prototype_trace'] = {'path': str(prototype_trace), 'sha256': sha(prototype_trace),
                                     'identical_to_baseline_trace': True}
        report['prototype_comparison'] = comparison
        arrays.update({'prototype_' + key: value for key, value in pa.items()})
        arrays.update({'prototype_comparison_' + key: value for key, value in ca.items()})
        if args.plot:
            plot_comparison(args.plot, hardware, ha, ea, ca, args.candidate_label)
            report['figure'] = str(args.plot)
    elif args.plot:
        parser.error('--plot requires --prototype')
    args.out_prefix.parent.mkdir(parents=True, exist_ok=True)
    args.out_prefix.with_suffix('.json').write_text(json.dumps(report, indent=2) + '\n')
    np.savez_compressed(args.out_prefix.with_suffix('.npz'), **arrays)
    print(json.dumps({'hardware_cycles': hardware['complete_cycles'],
                      'emulator_cycles': emulator['complete_cycles'],
                      'hardware_initial_width_s': hardware['initial_pulse_width_s'],
                      'emulator_initial_width_s': emulator['initial_pulse_width_s'],
                      'comparison': report['comparison'],
                      'prototype_comparison': report.get('prototype_comparison')}, indent=2))


if __name__ == '__main__':
    main()
