#!/usr/bin/env python3
"""Render an existing torture ROM until its explicit suite-complete marker."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest', type=Path, default=ROOT / 'artifacts/torture/full/manifest.json')
    parser.add_argument('--sameboy', type=Path, default=ROOT / 'SameBoy')
    parser.add_argument('--out', type=Path, help='New output directory; existing output is not overwritten')
    args = parser.parse_args()
    manifest = json.loads(args.manifest.read_text())
    rom = args.manifest.resolve().parent / Path(manifest['rom']).name
    if sha(rom) != manifest['rom_sha256']:
        raise RuntimeError('ROM no longer matches manifest')
    base = ROOT / 'artifacts/torture/renders'
    base.mkdir(parents=True, exist_ok=True)
    if args.out:
        out = args.out.resolve()
        out.mkdir(parents=True, exist_ok=False)
    else:
        out = Path(tempfile.mkdtemp(prefix='render-', dir=base))
    source = args.sameboy.resolve()
    commands = [
        ['make', '-C', str(source), 'CONF=release', 'build/lib/libsameboy.a', 'bootroms'],
        ['cc', '-std=gnu11', '-O2', '-Wall', '-Wextra', '-Werror', '-I', str(source),
         str(ROOT / 'benchmark/capture.c'), str(source / 'build/lib/libsameboy.a'), '-lm', '-ldl', '-o', str(out / 'capture')],
        [str(out / 'capture'), '--rom', str(rom), '--boot', str(source / 'build/bin/BootROMs/dmg_boot.bin'),
         '--until-done', '--seed', '1', '--wav', str(out / 'audio.wav'), '--screenshot', str(out / 'screen.ppm'),
         '--apu-log', str(out / 'apu.csv'), '--markers-log', str(out / 'markers.csv')],
    ]
    git = lambda *args: subprocess.check_output(['git', '-C', str(source), *args], text=True)
    shutil.copyfile(args.manifest, out / 'manifest.json')
    provenance = {'manifest': str(out / 'manifest.json'), 'input_manifest': str(args.manifest.resolve()),
                  'manifest_sha256': sha(out / 'manifest.json'),
                  'rom_sha256': sha(rom), 'sameboy_commit': git('rev-parse', 'HEAD').strip(),
                  'sameboy_diff_sha256': hashlib.sha256(git('diff', 'HEAD').encode()).hexdigest(),
                  'capture_source_sha256': sha(ROOT / 'benchmark/capture.c'), 'commands': commands,
                  'completion_authority': 'ROM emits HRAM FF82=4 after its finite suite/end packet; no wall-clock timeout.'}
    (out / 'provenance.json').write_text(json.dumps(provenance, indent=2) + '\n')
    print(out, flush=True)
    with (out / 'render.log').open('w') as log:
        for command in commands:
            subprocess.run(command, stdout=log, stderr=subprocess.STDOUT, check=True)
    provenance['outputs_sha256'] = {name: sha(out / name) for name in ['audio.wav', 'screen.ppm', 'apu.csv', 'markers.csv']}
    provenance['status'] = 'completed'
    (out / 'provenance.json').write_text(json.dumps(provenance, indent=2) + '\n')
    print(f'Completed: {out / "audio.wav"}', flush=True)


if __name__ == '__main__':
    main()
