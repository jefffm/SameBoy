# Focused DMG CH3/CH4 fixes — local implementation and validation

These behavioral fixes are separate from review cleanup commit `82b45cfe`
(`APU: address model-scope and save-state review feedback`). Only `Core/apu.c`
belongs to the focused implementation. This directory is locally excluded
experimental evidence, not a new SameBoy subproject or an upstream test suite.
Nothing has been pushed or posted to PR #790.

## Scope and implementation decisions

Both changes select `GB_MODEL_DMG_B`. Evidence comes from one physical DMG;
its silicon revision has not been established. This is a conservative choice
of emulator model, not confirmation that every DMG-B or another model behaves
identically.

- CH3 uses FF for an inactive sample-clock read while its DAC and triggered
  clock gate remain enabled. It changes the held byte at existing immediate
  or deferred sample-read events, not directly at expiry or retrigger.
  Otherwise the original address-bus-selected RAM read remains in place.
- CH4 DAC disable clears the background-counting gate even after length expiry.
  It retains counter phase; enabling the DAC alone does not restart counting.
  Existing trigger behavior, including triggering with the DAC off, is retained.
  The existing active-channel near-edge increment is not changed.
- No added save-state fields, timer rescheduling, hidden-index/LFSR changes,
  dependencies, or changes to other models are part of this focused patch.

The physical observations concern retriggered held output and retained upstream
phase. They do not directly observe the hidden CH3 index or CH4 LFSR. FF is a
behavioral model of the tested inactive reads, not proof of every SRAM bit or
CPU-bus phase. The CH4 measurement cannot distinguish a stopped counter from
equivalent phase-preserving storage/restoration.

## Red/green checks against physical recordings

The focused ROM contains 62 CH3 cases, 54 CH4 cases, and one calibration case.
The expected output comes from the physical recording, not the implementation.
All comparisons retain the independent acquisition calibration; tested edges
are not individually aligned to make the model match.

| Build | CH3 | CH4 |
|---|---|---|
| Cleanup baseline | 26 of 62 fail; maximum 15.464 sample units | Not needed for the CH3 red step |
| CH3 only | 62 pass | 16 of 54 fail; maximum 1074.822 T |
| Final Clang | 62 pass; maximum 0.528597 sample units | 54 pass; maximum 65.884385 T |
| Final GCC | Same results as Clang | Same results as Clang |
| Final with native save/load before retrigger | 62 pass; maximum 0.528900 | 54 pass; maximum 65.884385 T |

The engineering allowances are 2 sample units for held output and 128 T for
the first three noise edges, comparing both physical audio channels. These
are not statistical confidence intervals or claims of cycle-exact hardware
timing. Noise edge-prefix patterns also match. Results and individual errors
are in each render directory's `regression-*.json`.

The final render also passes comparisons against both earlier recordings:

| Take | CH3 cases / maximum waveform RMSE | CH4 cases / maximum first-edge error |
|---|---|---|
| `xryrmxjl` | 36 / 0.013548 | 48 / 110.644404 T |
| `qw3uz2uv` | 36 / 0.013375 | 48 / 110.793877 T |

Those checks use the earlier recordings' original calibration and thresholds
of 0.03 normalized waveform RMSE and 128 T. They are weaker than the focused
multi-edge/two-channel check and do not replace it. See
`previous-probe/regression.json` and `tests/previous_recordings.py`.

## Compatibility, transport, and review

- Seven existing companion regressions pass with both Clang and GCC:
  mixer/DAC, noise feedback/save compatibility, pulse clock, pulse duty,
  DAC shutdown edge, DAC reconnect, and DAC synthesis timing.
- The upstream sanity script passes all six expected screenshot hashes.
  Its outputs were moved, not deleted, into `after/upstream-output/`.
- Final focused PCM independently decodes on both audio channels into 234
  unique, ordered, checksum-valid packets and 116 readback sets identical to
  the physical capture. CPU-write and marker traces remain byte-identical
  to the frozen reference, including the suite-end event.
- Fifteen non-DMG-B variants produce byte-identical PCM and traces against
  the cleanup baseline on this ROM: `004`, `044`, `084`, `0c4`, `100`, `101`,
  `181`, `200`, `201`, `202`, `203`, `204`, `205`, `207`, and `227`.
- Standards review found no hard violations. Its optional calibration-variable
  naming suggestion was applied to the external regression script.
- Evidence review found a real compatibility regression in the first CH3
  candidate: a deferred read crossing DAC disable could replace 55 with FF.
  Requiring both `enable` and `pulsed` before the new inactive value fixes it.
  Pending counts 1 and 2, interrupted by DAC disable or retrigger, preserve
  the exposed sample on both compiler builds. `tests/pending-wave.c` is
  internal-state scratch validation, not a claim about physical subcycle
  behavior or a new permanent public-interface regression test.

## Limits that remain visible

The 116 pre-retrigger native save/load roundtrips preserve the traces and pass
the recorded-output checks, but PCM is **not** byte-identical with and without
roundtrips. This is also true of the cleanup baseline. Baseline: 32,274 sample
values differ, maximum 3,643 PCM units, RMS 6.1333. Final: 46,674 values differ,
maximum 3,643, RMS 8.4690. This does not establish artifact-free audio state
restoration; the separate baseline comparison prevents misreporting it as
transparent or as a newly isolated regression.

Clang and GCC static libraries and the tester build successfully. GCC reports
its existing mixed-LTO incremental-link warning. Full `make lib` packaging is
not verified: the installed environment lacks `cppp`, so public-header
generation failed even though its pipeline returned success. Tests include
the source headers and link the successfully built archives. Sanitizers were
not freshly rerun; an earlier baseline sanitizer issue in `Core/gb.c:1639`
remains outside this focused check.

No broad frequency/subcycle or cross-device hardware claim is made. The
separate reviewer promise for convenient sweep-table verification remains
outstanding; these CH3/CH4 fixes and recordings do not fulfill it.

## Provenance and reproduction

Paths below are relative to `/home/jeff/Work/SameBoy-review` unless absolute.
Frozen inputs and the original WAV were not modified.

- Final `Core/apu.c` SHA-256:
  `05ec5dd9e162d58254ebfb67b6d617e110a0fd3a80a78b1dd20486448a540368`
- ROM: `.amp/in/artifacts/ch34-focused-probe/rom/ch34-focused-probe.gb`
- ROM SHA-256:
  `35e40243730d7d686721c987ef40ceb6b5dedb31a6051c0c6e4a586a75c498ca`
- Physical WAV:
  `/home/jeff/Documents/ChatGPT/dmg/artifacts/hardware/ch34-clocks-ew_png0d/capture.wav`
- WAV SHA-256:
  `21cbdadda811688f8c432b58358e9e4eeea1d3ac595f6f266edcb9ae7e768c68`
- Acquisition methods and uncertainty:
  `../ch34-focused-hardware-ew_png0d/ch3-analysis/REPORT.md` and
  `../ch34-focused-hardware-ew_png0d/ch4-analysis/REPORT.md`.

Run from the repository root, using the existing audio-lab virtual environment:

```sh
set -eu
root=.amp/in/artifacts/ch34-focused-fixes
kit=.amp/in/artifacts/ch34-focused-probe
python=/home/jeff/Documents/ChatGPT/dmg/.venv/bin/python
export PYTHONDONTWRITEBYTECODE=1
make -j2 CONF=release build/lib/libsameboy.a
mkdir -p "$root/recheck"
clang -std=gnu11 -O2 -I. "$root/tests/capture-model.c" \
    build/lib/libsameboy.a -lm -ldl -o "$root/recheck/capture"
"$root/recheck/capture" --rom "$kit/rom/ch34-focused-probe.gb" \
    --boot build/bin/BootROMs/dmg_boot.bin --until-done --seed 1 \
    --wav "$root/recheck/audio.wav" --screenshot "$root/recheck/screen.ppm" \
    --apu-log "$root/recheck/apu.csv" --markers-log "$root/recheck/markers.csv"
"$python" "$root/tests/recording_regression.py" "$root/recheck"
"$python" "$root/tests/previous_recordings.py"
```

The last command checks the saved final render of the earlier ROM in
`previous-probe/`. To regenerate it, use the same renderer with
`.amp/in/artifacts/ch34-clock-probe/rom/ch34-clock-probe.gb` and corresponding
output paths.
For GCC, build the archive with
`make -j2 CC=gcc CONF=release OBJ=.amp/in/gcc-obj LIBDIR=.amp/in/gcc-lib .amp/in/gcc-lib/libsameboy.a`
and link that archive with GCC. `CH34_MODEL` selects a numeric model and
`CH34_ROUNDTRIP=1` enables the 116 pre-retrigger save/load checks.

## Suggested reviewer explanation — not posted

The recordings support continued retained phase after CH4 length expiry with
the DAC enabled, but disabling the DAC stops that progression until retrigger,
without erasing phase. CH3 shows a delayed held-output change even with wave
RAM untouched; the existing sample-clock timing plus an inactive FF read
matches the tested histories. The fixes are limited to the DMG-B emulation
model, separate from the review cleanup, and checked against all three takes.
They do not claim direct observation of hidden CH3 index or CH4 LFSR motion,
universal hardware behavior, or complete subcycle accuracy.
