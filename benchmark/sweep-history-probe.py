#!/usr/bin/env python3
"""Probe whether the same positive-sweep transition depends on prior updates.

Build mode prepares paired first/second-update cases at the existing circuit
counterexamples. Inspect mode compares a finalized hardware capture's pairs.
The chained results are unmeasured predictions, never hardware expectations.
"""
import argparse
import hashlib
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'benchmark/torture'))
import build
from cases import Case, FRAME_STEP, SWEEP_TICK, catalog as torture_catalog

ORACLE = ROOT / 'benchmark/oracles/dmg-probe-v2-erekgdxk.json'
ORACLE_SHA256 = 'e2251d1ccdd6cf9558ea95a9da574d6d60b973f541708b67daff8b6a00786277'
# Existing missing-stop and false-stop controls, including the shift-1 timing
# contradiction. Include every one with a surviving ordinary predecessor.
BOUNDARIES = [(2, 0x4cd), (2, 0x51a), (2, 0x51b), (2, 0x51c),
              (2, 0x51d), (2, 0x51e), (2, 0x51f), (3, 0x64f), (1, 0x2ab)]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def catalog():
    if digest(ORACLE) != ORACLE_SHA256:
        raise ValueError('Physical oracle changed')
    oracle = json.loads(ORACLE.read_text())
    if not oracle['sweep']['all_initial_active']:
        raise ValueError('Probe requires the measured initial-active controls')
    killed = {int(s): set(values) for s, values in
              oracle['sweep']['killed_old_frequencies_by_shift'].items()}
    result = [torture_catalog()[0]]
    for shift, target in BOUNDARIES:
        predecessors = [p for p in range(2048) if p + (p >> shift) == target]
        if not predecessors:
            continue
        if len(predecessors) != 1:
            raise ValueError('Positive sweep should have a unique predecessor')
        predecessor = predecessors[0]
        if predecessor in killed[shift]:
            continue
        following = target + (target >> shift)
        if following + (following >> shift) >= 2048:
            raise ValueError('Target would have ordinary arithmetic overflow')
        for mode, initial in [('direct', target), ('chained', predecessor)]:
            prediction = [0xf1] + ([0xf1] if mode == 'chained' else [])
            prediction.append(0xf0 | int(target not in killed[shift]))
            case = Case(f'sweep-history-s{shift}-{target:03x}-{mode}', 'ch1-sweep-history',
                        'Read the same target transition after a fresh trigger or one surviving ordinary sweep update.',
                        parameters={'shift': shift, 'target_frequency': target,
                                    'predecessor_frequency': predecessor,
                                    'initial_frequency': initial, 'mode': mode,
                                    'prediction_NR52': prediction,
                                    'prediction_authority': 'History-independent extension of the recorded first-update map; pending this hardware measurement.',
                                    'first_update_target_active_recorded': target not in killed[shift],
                                    'initial_wait_T': FRAME_STEP,
                                    'following_wait_T': 3 * FRAME_STEP,
                                    'second_update_wait_T': SWEEP_TICK if mode == 'chained' else 0})
            # Match the packed probe's reset/trigger sequence, including the
            # frequency write instructions. NR51 remains zero during trials.
            case.emit(f'    ld de, ${initial:04x}')
            case.reset()
            for reg, value in [(0x10, 0x10 | shift), (0x11, 0x80), (0x12, 0xf0)]:
                case.write(reg, value)
            case.emit('    ld a, e', '    ldh [$ff13], a',
                      '    ld a, d', '    or $80', '    ldh [$ff14], a')
            case.wait(FRAME_STEP)
            case.read(0x26, 'NR52-before-first-update')
            case.wait(3 * FRAME_STEP)
            case.read(0x26, 'NR52-after-first-update')
            if mode == 'chained':
                case.wait(SWEEP_TICK)
                case.read(0x26, 'NR52-after-second-update')
            case.finish()
            result.append(case)
    return result


def inspect(manifest, metrics):
    observed, failures, pairs = {}, [], {}
    for case in manifest['cases']:
        if case['group'] != 'ch1-sweep-history':
            continue
        packets = [p for p in metrics['hardware_packets'] if p['kind'] == 2 and p['id'] == case['id']]
        if len(packets) != 1:
            failures.append({'case': case['id'], 'reason': 'missing_or_duplicate_packet'})
            continue
        packet = packets[0]
        if (packet['status'] != 'valid' or packet['checksum'] != packet['expected_checksum']
                or len(packet['payload']) != case['readback_length']):
            failures.append({'case': case['id'], 'reason': 'invalid_packet'})
            continue
        observed[case['id']] = packet['payload']
        p = case['parameters']
        pairs.setdefault((p['shift'], p['target_frequency']), {})[p['mode']] = case
    results = []
    for (shift, target), modes in sorted(pairs.items()):
        if set(modes) != {'direct', 'chained'}:
            continue
        direct = observed[modes['direct']['id']]
        chained = observed[modes['chained']['id']]
        controls_valid = direct[0] == 0xf1 and chained[:2] == [0xf1, 0xf1]
        controls_valid &= direct[-1] in (0xf0, 0xf1) and chained[-1] in (0xf0, 0xf1)
        recorded_active = modes['direct']['parameters']['first_update_target_active_recorded']
        results.append({'shift': shift, 'target_frequency': target,
                        'direct_NR52': direct, 'chained_NR52': chained,
                        'controls_valid': controls_valid,
                        'direct_repeats_previous_take': direct[-1] == 0xf0 | int(recorded_active),
                        'different_after_target': direct[-1] != chained[-1] if controls_valid else None})
    expected = sum(c['group'] == 'ch1-sweep-history' for c in manifest['cases']) // 2
    complete = bool(metrics.get('complete_hardware')) and not failures and len(results) == expected
    return {'complete': complete, 'packet_failures': failures, 'pairs': results,
            'interpretation': 'A valid pair difference challenges the history-independent map. Agreement only supports these histories on this unit; it does not prove a transistor mechanism or all histories.'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=ROOT / 'artifacts/sweep-history-probe')
    parser.add_argument('--manifest', type=Path)
    parser.add_argument('--metrics', type=Path)
    parser.add_argument('--report', type=Path)
    args = parser.parse_args()
    if args.metrics:
        if not args.manifest or not args.report:
            parser.error('--metrics requires --manifest and --report')
        manifest = json.loads(args.manifest.read_text())
        metrics = json.loads(args.metrics.read_text())
        if metrics['manifest']['sha256'] != digest(args.manifest):
            raise ValueError('Metrics use a different immutable manifest')
        report = inspect(manifest, metrics)
        report.update(manifest_sha256=digest(args.manifest), metrics_sha256=digest(args.metrics))
        with args.report.open('x') as file:
            json.dump(report, file, indent=2)
            file.write('\n')
        print(json.dumps(report, indent=2))
        return
    args.out.mkdir(parents=True, exist_ok=False)
    build.catalog = catalog
    sys.argv = [str(Path(__file__).resolve()), '--out', str(args.out)]
    build.main()
    path = args.out / 'manifest.json'
    manifest = json.loads(path.read_text())
    manifest['source_sha256']['benchmark/sweep-history-probe.py'] = digest(Path(__file__))
    manifest['oracle_sha256'] = ORACLE_SHA256
    manifest['purpose'] = 'Unmeasured paired-history discriminator. All prediction_NR52 values are hypotheses, not observations from this ROM.'
    path.write_text(json.dumps(manifest, indent=2) + '\n')


if __name__ == '__main__':
    main()
