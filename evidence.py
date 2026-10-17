#!/usr/bin/env python3
"""Portable integrity, FLAC decode and byte-identical frozen-ROM rebuilding."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile
import wave

ROOT = Path(__file__).resolve().parent
SUITES = {'torture': 'artifacts/torture/full', 'v2': 'artifacts/audio-oracle-probe-v2',
          'history': 'artifacts/sweep-history-probe'}


def sha(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def verify_pcm(path, record):
    with wave.open(str(path), 'rb') as wav:
        expected = tuple(record[k] for k in ['sample_rate', 'channels', 'sample_width_bytes', 'frames'])
        assert (wav.getframerate(), wav.getnchannels(), wav.getsampwidth(), wav.getnframes()) == expected
        h = hashlib.sha256()
        while chunk := wav.readframes(wav.getframerate()):
            h.update(chunk)
    assert h.hexdigest() == record['pcm_sha256'], 'Decoded PCM differs from original WAV'


def decode(recording):
    records = json.loads((ROOT / 'hardware/manifest.json').read_text())['recordings']
    selected = [r for r in records if recording is None or r['id'] == recording]
    if not selected:
        raise ValueError('Unknown recording ID')
    destination = ROOT / 'build/hardware'
    destination.mkdir(parents=True, exist_ok=True)
    for record in selected:
        source = ROOT / record['flac']
        assert sha(source) == record['flac_sha256']
        target = destination / (record['id'] + '.wav')
        if not target.exists():
            subprocess.run(['flac', '--silent', '--decode', '--output-name=' + str(target), str(source)], check=True)
        verify_pcm(target, record)
        print(f'Original PCM verified: {target}')


def rebuild():
    results = []
    engine = ROOT / 'benchmark/torture'
    with tempfile.TemporaryDirectory(prefix='dmg-frozen-roms-') as temporary:
        out = Path(temporary)
        for name, directory in SUITES.items():
            source = ROOT / directory
            manifest = json.loads((source / 'manifest.json').read_text())
            for filename in ['engine.asm', 'macros.inc']:
                assert sha(engine / filename) == manifest['source_sha256']['benchmark/torture/' + filename]
            for filename, digest in manifest['generated_sha256'].items():
                assert sha(source / filename) == digest
            obj, rom = out / (name + '.o'), out / (name + '.gb')
            commands = [
                ['rgbasm', '-I', str(engine) + '/', '-I', str(source) + '/', '-o', str(obj), str(engine / 'engine.asm')],
                ['rgblink', '-o', str(rom), str(obj)],
                ['rgbfix', '-v', '-p', '0', '-t', 'DMG AUDIO LAB', '-m', '0x1b', '-r', '0x04', str(rom)],
            ]
            for command in commands:
                subprocess.run(command, check=True)
            assert rom.read_bytes() == (source / 'audio-torture.gb').read_bytes()
            assert sha(rom) == manifest['rom_sha256']
            results.append({'suite': name, 'rom_sha256': sha(rom), 'byte_identical': True})
        for mode, retrigger in [('steady', 0), ('retrigger', 1)]:
            obj, rom = out / (mode + '.o'), out / (mode + '.gb')
            for command in [
                ['rgbasm', '-D', f'RETRIGGER={retrigger}', '-o', str(obj), str(ROOT / 'benchmark/noise-probe.asm')],
                ['rgblink', '-o', str(rom), str(obj)],
                ['rgbfix', '-v', '-p', '0', '-t', 'NOISE PROBE', '-m', '0x1b', '-r', '0x04', str(rom)],
            ]:
                subprocess.run(command, check=True)
            assert rom.read_bytes() == (ROOT / f'artifacts/noise-probe/{mode}-mbc5.gb').read_bytes()
            results.append({'suite': mode, 'rom_sha256': sha(rom), 'byte_identical': True})
    print(json.dumps(results, indent=2))


def verify():
    manifest = json.loads((ROOT / 'manifest.json').read_text())
    for record in manifest['files']:
        path = ROOT / record['path']
        assert path.stat().st_size == record['bytes'], str(path)
        assert sha(path) == record['sha256'], str(path)
    print(f"Verified {len(manifest['files'])} listed files (SHA256 and size)")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=['verify', 'decode', 'rebuild'])
    parser.add_argument('--recording', help='One hardware recording ID; otherwise decode all five')
    args = parser.parse_args()
    if args.action == 'verify': verify()
    elif args.action == 'decode': decode(args.recording)
    else: rebuild()


if __name__ == '__main__':
    main()
