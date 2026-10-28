#!/usr/bin/env python3
"""Encode the complete measured ordinary positive-sweep domain, without fitting.

This is an empirical transition map for the recorded DMG unit. It is not
generated from the estimated-delay circuit or from a list of model mismatches.
"""
import argparse
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ORACLE = ROOT / 'inputs/dmg-probe-v2-erekgdxk.json'
ORACLE_SHA256 = 'e2251d1ccdd6cf9558ea95a9da574d6d60b973f541708b67daff8b6a00786277'


def generate():
    raw = ORACLE.read_bytes()
    if hashlib.sha256(raw).hexdigest() != ORACLE_SHA256:
        raise ValueError('Physical oracle differs from the recorded source')
    oracle = json.loads(raw)
    coverage = oracle['packet_coverage']
    if not coverage['complete'] or not coverage['suite_end_received'] or not oracle['sweep']['all_initial_active']:
        raise ValueError('Incomplete physical sweep observations')
    rows, trials, stops = [], 0, 0
    for shift in range(1, 8):  # NR10's nonzero three-bit shift field.
        domain = {f for f in range(0x800)
                  if (new := f + (f >> shift)) < 0x800 and new + (new >> shift) < 0x800}
        measured = oracle['sweep']['killed_old_frequencies_by_shift'][str(shift)]
        killed = set(measured)
        if len(killed) != len(measured) or not killed <= domain:
            raise ValueError('Duplicate or out-of-domain physical observation')
        row = [0] * (0x800 // 8)  # One bit for each 11-bit frequency.
        for frequency in killed:
            row[frequency >> 3] |= 1 << (frequency & 7)
        rows.append(row)
        trials += len(domain)
        stops += len(killed)
    if trials != oracle['sweep']['trials']:
        raise ValueError('Measured domain differs from the register domain')
    header = '''/* Measured ordinary positive-sweep outcomes for DMG-B emulation.
 * Each row corresponds to NR10 shift 1..7; bit f marks old frequency f.
 * All 11,587 eligible states were measured: 192 stopped, 11,395 survived.
 * Ineligible states are zero-filled and must be excluded by the caller.
 *
 * This is an empirical model of the recorded unit, not a gate-delay model.
 * The physical DMG's CPU/board revision has not been identified.
 * Source: companion audio lab benchmark/generate-sweep-table.py and
 * benchmark/oracles/dmg-probe-v2-erekgdxk.json, SHA256
 * e2251d1ccdd6cf9558ea95a9da574d6d60b973f541708b67daff8b6a00786277.
 * Hardware WAV SHA256:
 * bf1a40db5ef5a883182c1121d4da9d569c9d0b4191b47e8dbf8b8f9acd736a93.
 * No values come from the separately licensed circuit research model.
 */
static const uint8_t dmg_sweep_carry_hazards[7][0x100] = {
'''
    for shift, row in enumerate(rows, 1):
        header += f'    {{ /* Shift {shift} */\n'
        for offset in range(0, len(row), 16):
            header += '        ' + ', '.join(f'0x{x:02X}' for x in row[offset:offset + 16]) + ',\n'
        header += '    },\n'
    header += '};\n'
    return header, trials, stops


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    target = parser.add_mutually_exclusive_group(required=True)
    target.add_argument('--out', type=Path, help='Write a new include/header')
    target.add_argument('--check', type=Path, help='Verify an existing .inc or legacy .h byte-for-byte')
    target.add_argument('--sameboy', '--core', dest='sameboy', type=Path,
                        help='Check Core/dmg_sweep.inc, falling back to legacy Core/dmg_sweep.h')
    args = parser.parse_args()
    header, trials, stops = generate()
    if args.sameboy:
        args.check = args.sameboy / 'Core/dmg_sweep.inc'
        if not args.check.is_file():
            args.check = args.sameboy / 'Core/dmg_sweep.h'
    if args.check:
        if args.check.read_bytes() != header.encode():
            raise ValueError('Header differs from the complete measured truth table')
        print(f'PASS: {args.check}')
    else:
        with args.out.open('x') as output:
            output.write(header)
    print(f'{trials} measured transitions: {stops} stops, {trials - stops} survivors; 1792 table bytes')


if __name__ == '__main__':
    main()
