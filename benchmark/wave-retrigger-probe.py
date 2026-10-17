#!/usr/bin/env python3
"""Build a finite physical probe separating CH3 SRAM-corruption hypotheses."""
import hashlib
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parent.parent
HERE = ROOT / 'benchmark/torture'
sys.path.insert(0, str(HERE))
from cases import Case, WAVE, catalog


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def probe(name, wave, *, wait=208, nops=False, delay_off=0, safe=False):
    c = Case(name, 'ch3', 'Physical hypothesis discriminator; no assumed pass result.')
    c.parameters = {'wave_bytes': wave, 'wait_T': wait, 'wait_instruction': 'NOP' if nops else 'original counted wait',
                    'post_trigger_extra_T': delay_off, 'safe_retrigger': safe}
    c.reset()
    c.wave(frequency=0x7f8)
    # Preserve the original setup instruction sequence and bus-write timing.
    for address, value in enumerate(wave):
        old = f'    write_reg ${0xff30 + address:04x}, ${WAVE[address]:02x}'
        c.lines[c.lines.index(old)] = f'    write_reg ${0xff30 + address:04x}, ${value:02x}'
    if nops:
        c.emit(*['    nop'] * (wait // 4))
    else:
        c.wait(wait)
    if safe:
        c.write(0x1a, 0)
        c.write(0x1a, 0x80)
    c.write(0x1e, 0x87)
    c.wait(delay_off)
    c.write(0x1a, 0)
    c.ram('result')
    c.finish()
    return c


def probe_cases():
    cases = [catalog()[0], probe('original-pattern', WAVE)]
    # Complete three-input Boolean domain, broadcast to all eight data bits.
    # Bytes0,4,7 are destination, aligned source, and selected source respectively.
    for inputs in range(8):
        wave = WAVE.copy()
        for bit, address in enumerate((0, 4, 7)):
            wave[address] = 255 if inputs & (1 << bit) else 0
        c = probe(f'operand-truth-{inputs:03b}', wave)
        c.parameters['boolean_inputs_a_b_c'] = [(inputs >> bit) & 1 for bit in range(3)]
        cases.append(c)
    cases += [probe('original-pattern-equal-time-NOP', WAVE, nops=True),
              # At7F8 each sample lasts16T; delay the DAC-off write by one
              # full wave-byte interval to separate it from trigger startup.
              probe('original-pattern-delayed-DAC-off', WAVE, delay_off=32),
              probe('original-pattern-safe-control', WAVE, safe=True)]
    return cases


def additional_probe_cases():
    """Append only truth rows missing in the other recorded source blocks.

    Block4 already receives the complete Boolean domain in probe_cases().
    The original WAVE pattern covers five rows in block8 and six in block12;
    their three and two missing rows test the rule's address generalization.
    """
    cases = []
    for base in range(8, 16, 4):
        seen = set()
        for selected in range(base, base + 4):
            a, b, c = WAVE[0], WAVE[base], WAVE[selected]
            for bit in range(8):
                seen.add((((a >> bit) & 1) << 2) |
                         (((b >> bit) & 1) << 1) | ((c >> bit) & 1))
        selected = base + 3
        # In the existing timer interpretation, this is the first 16T-aligned
        # delay whose next sample selects the final byte in this source block.
        wait = selected * 32 - 16
        for truth in sorted(set(range(8)) - seen):
            wave = WAVE.copy()
            inputs = [(truth >> bit) & 1 for bit in (2, 1, 0)]
            for address, value in zip((0, base, selected), inputs):
                wave[address] = 255 if value else 0
            case = probe(f'operand-block{base:02x}-truth-{truth:03b}', wave, wait=wait)
            case.parameters.update(boolean_inputs_a_b_c=inputs,
                                   selected_offset_hypothesis=selected,
                                   missing_in_original_block_data=True)
            cases.append(case)
    return cases


def main():
    out = ROOT / 'artifacts/wave-retrigger/discriminator'
    out.mkdir(parents=True, exist_ok=True)
    cases = probe_cases() + additional_probe_cases()
    table, assembly = [], []
    for i, c in enumerate(cases, 1):
        label = f'Case_{i:04x}'
        table.append(f'db BANK({label})\ndw {label}\ndw {i}')
        assembly += [f'SECTION "{c.name}", ROMX', label + ':', *c.lines]
    (out / 'case_table.inc').write_text('\n'.join(table) + '\ndb 0\n')
    (out / 'cases.inc').write_text('\n'.join(assembly) + '\n')
    rom = out / 'wave-retrigger-probe.gb'
    for command in [
        ['rgbasm', '-I', str(HERE) + '/', '-I', str(out) + '/', '-o', str(out / 'engine.o'), str(HERE / 'engine.asm')],
        ['rgblink', '-m', str(out / 'wave-retrigger-probe.map'), '-n', str(out / 'wave-retrigger-probe.sym'), '-o', str(rom), str(out / 'engine.o')],
        ['rgbfix', '-v', '-p', '0', '-t', 'WAVE RAM PROBE', '-m', '0x1b', '-r', '0x04', str(rom)],
    ]:
        subprocess.run(command, check=True)
    base = json.loads((ROOT / 'artifacts/torture/renders/render-h1xqcgiz/manifest.json').read_text())
    manifest = {k: base[k] for k in ['format', 'clock_T_hz', 'protocol', 'markers']}
    manifest.update({'rom': str(rom), 'rom_sha256': digest(rom), 'rom_bytes': rom.stat().st_size,
                     'purpose': 'Characterize this physical DMG before selecting a corruption formula or model scope.',
                     'behavior': 'Automatic one pass, acoustic readbacks and final packet; no SRAM access.',
                     'cases': [{'id': i, 'name': c.name, 'group': c.group, 'description': c.description,
                                'parameters': c.parameters, 'readback_labels': c.readback_labels,
                                'readback_length': len(c.readback_labels)} for i, c in enumerate(cases, 1)],
                     'source_sha256': {str(p.relative_to(ROOT)): digest(p) for p in
                                       [Path(__file__).resolve(), HERE / 'engine.asm', HERE / 'macros.inc', HERE / 'cases.py']},
                     'limits': ['A single DMG characterizes an instance, not all DMG revisions.',
                                'The majority formula is a hypothesis; the probe intentionally does not declare it an expectation.']})
    (out / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(f'{rom}\n{len(cases)} cases; SHA256 {digest(rom)}')


if __name__ == '__main__':
    main()
