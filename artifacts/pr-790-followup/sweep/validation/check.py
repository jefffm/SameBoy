"""Portable emulator/reporting regression check; never physical hardware evidence."""
import argparse
import csv
import hashlib
import json
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor

HERE = Path(__file__).resolve().parent
KIT = HERE.parent
LIMITS = [911, 1312, 1619, 1815, 1926, 1986, 2018]
KEYS = [(s, f) for s in range(1, 8) for f in range(LIMITS[s - 1])]
oracle = json.loads((KIT / 'inputs/dmg-probe-v2-erekgdxk.json').read_text())
STOP = {(int(s), f) for s, values in oracle['sweep']['killed_old_frequencies_by_shift'].items() for f in values}
reference = [(1, int(key not in STOP)) for key in KEYS]
mixed = list(reference)
overrides = {(1, 0): (1, 0), (1, 683): (1, 1), (1, 910): (0, 1),
             (2, 1020): (0, 0), (3, 1618): (1, 0), (4, 0): (1, 0),
             (4, 1814): (1, 0), (7, 2017): (0, 1)}
for key, pair in overrides.items():
    mixed[KEYS.index(key)] = pair
fixtures = {'mixed': mixed, 'opposite': [(1, 1 - p[1]) for p in reference],
            'invalid': [(0, f % 2) for s, f in KEYS]}


def read_rows(path):
    with path.open() as stream:
        return [{k: int(v) for k, v in row.items()} for row in csv.DictReader(stream)]


def run(name, model, fixture, variant, output, work, bootroms):
    out = output / name
    out.mkdir(exist_ok=True)
    rom = KIT / 'rom' / variant
    boot = 'mgb' if model == '0x100' else 'sgb' if model == '0x004' else 'sgb2' if model == '0x101' else 'dmg'
    command = [str(work / 'observe'), str(rom), str(bootroms / f'{boot}_boot.bin'),
               str(out), model, str(work / f'{fixture}.bin') if fixture else '-', '1']
    result = subprocess.run(command, capture_output=True, text=True, timeout=300)
    (out / 'run.log').write_text(result.stdout + result.stderr)
    result.check_returncode()
    print(name, next(line for line in result.stdout.splitlines() if line.startswith('model=')), flush=True)
    rows = read_rows(out / 'trials.csv')
    assert [(r['shift'], r['frequency']) for r in rows] == KEYS
    assert [r['trial'] for r in rows] == list(range(11587))
    observed = [(r['pre'], r['post']) for r in rows]
    if fixture:
        assert observed == fixtures[fixture]
    elif model == '0x002':
        assert observed == reference
    ram = (out / 'ram.bin').read_bytes()
    assert len(ram) == 8192
    for (shift, frequency), (pre, post) in zip(KEYS, observed):
        byte = ram[(shift - 1) * 512 + frequency // 4]
        assert (byte >> (6 - 2 * (frequency % 4))) & 3 == pre * 2 + post
    summary = (out / 'summary.txt').read_text().splitlines()
    problems = {}
    totals = [0, 0, 0]
    for shift in range(1, 8):
        cases = [(key, pair) for key, pair in zip(KEYS, observed) if key[0] == shift]
        stops = sum(pre and not post for key, (pre, post) in cases)
        differences = sum(pre and (bool(post) != (key not in STOP)) for key, (pre, post) in cases)
        invalid = sum(not pre for key, (pre, post) in cases)
        counts = [stops, differences, invalid]
        for i in range(3):
            offset = 0xf00 + (shift - 1) * 6 + 2 * i
            assert int.from_bytes(ram[offset:offset + 2], 'little') == counts[i], (name, shift, counts)
            totals[i] += counts[i]
        ref = sum(key[0] == shift for key in STOP)
        assert summary[shift + 3].split() == [str(shift), f'{stops:04}', f'{ref:03}', f'{differences:04}', f'{invalid:04}']
        problems[shift] = [(key, pair) for key, pair in cases if not pair[0] or bool(pair[1]) != (key not in STOP)]
    status = 'INVALID INITIAL DATA' if totals[2] else 'DIFFERS FROM JEFF' if totals[1] else "MATCHES JEFF'S UNIT"
    assert summary[13].strip() == status
    assert summary[14].strip() == 'NOT A HARDWARE PASS'
    for page in ('summary-return', 'summary-wrap'):
        assert (out / f'{page}.txt').read_text().splitlines() == summary

    def detail(name, shift, index):
        lines = (out / f'{name}.txt').read_text().splitlines()
        assert lines[1].startswith(f'SHIFT {shift}')
        if not problems[shift]:
            assert lines[7].strip() == 'NO DIFFERENCES'
            return
        key, (pre, post) = problems[shift][index % len(problems[shift])]
        assert lines[7].strip() == f'FREQ ${key[1]:03X}', (name, lines[7], key)
        assert lines[9].split() == ['NR52', 'PRE', str(pre), 'POST', str(post)]
        assert lines[11].strip() == 'JEFF: ' + ('STOP' if key in STOP else 'LIVE')
        assert lines[12].strip() == 'UNIT: ' + ('INVALID INIT' if not pre else 'LIVE' if post else 'STOP')
    detail('detail-first', 1, 0)
    detail('detail-last', 1, -1)
    detail('detail-wrap', 1, 0)
    detail('detail-next', 1, 1)
    detail('detail-shift2', 2, 0)
    detail('detail-shift7', 7, 0)
    print(name, 'PASS', dict(zip(('stops', 'differences', 'invalid'), totals)), flush=True)
    return rows


def main():
    if not __debug__:
        raise SystemExit('Checks require assertions: do not use python -O or PYTHONOPTIMIZE.')
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--core', required=True, type=Path, help='SameBoy source checkout (read-only)')
    parser.add_argument('--bootroms', required=True, type=Path, help='directory of SameBoy dmg/mgb/sgb/sgb2_boot.bin files')
    parser.add_argument('--out', required=True, type=Path, help='new results directory (never overwrite evidence)')
    parser.add_argument('--cc', default='cc', help='C compiler executable')
    args = parser.parse_args()
    core, bootroms, output = args.core.resolve(), args.bootroms.resolve(), args.out.resolve()
    assert (core / 'Core/gb.h').is_file(), 'Expected SameBoy checkout with Core/gb.h'
    boots = [bootroms / f'{model}_boot.bin' for model in ('dmg', 'mgb', 'sgb', 'sgb2')]
    assert all(path.is_file() for path in boots), 'Missing boot ROMs; see README.md'
    subprocess.run([sys.executable, str(KIT / 'build.py'), '--check-only'], check=True)
    output.mkdir(parents=True, exist_ok=False)
    source_files = sorted(path for path in (core / 'Core').rglob('*') if path.is_file())
    source_hashes = {str(path.relative_to(core)): hashlib.sha256(path.read_bytes()).hexdigest()
                     for path in source_files}
    jobs = [('native-dmg', '0x002', None), ('native-mgb', '0x100', None),
            ('native-sgb', '0x004', None), ('native-sgb2', '0x101', None),
            ('mixed', '0x002', 'mixed'), ('opposite', '0x002', 'opposite'),
            ('invalid', '0x002', 'invalid'), ('mbc5-dmg', '0x002', None)]
    with tempfile.TemporaryDirectory(prefix='.work-', dir=output) as temporary:
        work = Path(temporary)
        # Compile the actual checkout, not a potentially stale prebuilt library.
        # Assertions stay enabled in both harness and core; no make/checkout writes.
        command = [args.cc, '-std=gnu11', '-O2', '-D_GNU_SOURCE', '-DGB_INTERNAL',
                   '-DGB_VERSION="sweep-evidence-check"', '-I', str(core), str(HERE / 'observe.c'),
                   *map(str, sorted((core / 'Core').glob('*.c'))), '-lm', '-o', str(work / 'observe')]
        result = subprocess.run(command, capture_output=True, text=True)
        (output / 'compile.log').write_text(shlex.join(command) + '\n' + result.stdout + result.stderr)
        result.check_returncode()
        for name, pairs in fixtures.items():
            (work / f'{name}.bin').write_bytes(bytes(bit for pair in pairs for bit in pair))
        with ThreadPoolExecutor(max_workers=2) as pool:
            results = list(pool.map(lambda job: run(*job,
                'sweep-verify-mbc5.gb' if job[0] == 'mbc5-dmg' else 'sweep-verify.gb',
                output, work, bootroms), jobs))
    # The original audio ROM is not bundled or rerun. Compare to its frozen
    # archival per-trial observation AND relative instruction-boundary trace.
    original = KIT / 'archive/validation/original/trials.csv'
    assert results[0] == read_rows(original), 'Archived original schedule/readbacks differ'
    assert results[0] == results[-1], 'ROM-only and MBC5 trial traces differ'
    for path in (output / 'native-dmg').iterdir():
        if path.suffix in ('.txt', '.ppm', '.bin'):
            assert path.read_bytes() == (output / 'mbc5-dmg' / path.name).read_bytes(), path.name
    for name, before in source_hashes.items():
        assert hashlib.sha256((core / name).read_bytes()).hexdigest() == before, f'{name} changed during validation'
    revision = subprocess.run(['git', '-C', str(core), 'rev-parse', 'HEAD'], capture_output=True, text=True)
    report = {'all_passed': True, 'trials_per_run': 11587,
              'native_models': ['DMG-B', 'MGB', 'SGB', 'SGB2'],
              'synthetic': ['mixed missing/extra stops and both invalid bit patterns',
                            'all results inverted', 'all initial controls invalid'],
              'archived_original_schedule_and_readbacks_equal': True,
              'original_audio_rom_rerun': False, 'variants_trace_ram_and_screens_equal': True,
              'physical_hardware_exercised_by_this_check': False,
              'core_revision': revision.stdout.strip() if revision.returncode == 0 else None,
              'core_sources_sha256': source_hashes,
              'bootroms_sha256': {path.name: hashlib.sha256(path.read_bytes()).hexdigest() for path in boots},
              'validation_sources_sha256': {path.name: hashlib.sha256(path.read_bytes()).hexdigest()
                                            for path in (HERE / 'observe.c', Path(__file__))},
              'compiler': subprocess.check_output([args.cc, '--version'], text=True).splitlines()[0],
              'command': sys.argv,
              'path_note': 'Invocation and compile.log paths are execution provenance, not portable dependencies.'}
    (output / 'checks.json').write_text(json.dumps(report, indent=2) + '\n')
    print('PASS: archived original trace, both variants, 4 models, 3 injected patterns, UI navigation and full retained results')


if __name__ == '__main__':
    main()
