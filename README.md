# SameBoy DMG audio: companion evidence

This is the separate evidence branch for Jeff Lynn’s seven-commit SameBoy audio PR. It contains test sources, frozen hardware ROMs, physical readbacks, recordings, and existing validation reports. The core changes are on [`jefffm/SameBoy:codex/dmg-audio-fidelity`](https://github.com/jefffm/SameBoy/tree/codex/dmg-audio-fidelity), publication head `8182d8edbaddbac9f6057930f6cebdabd3b83228`, based on upstream `26a56bd6a122a6318a456abd80558408742d5adf`.

The work began with a chiptune recorded from LSDj 8.5.1 on an authentic DMG. Later standalone ROMs isolated noise, pulse, wave-RAM, and sweep behavior. The hardware connection was the stock headphone jack to Scarlett inputs 3 (left) and 4 (right), recorded at 48 kHz, stereo, 24-bit PCM. **The physical CPU/board revision is unconfirmed.** The emulator tests select DMG-B. The right capture channel is inverted, so analysis uses left channel zero; do not average the two. The analog path is not calibrated.

## What supports each change

| Change | Reproduction and evidence | Scope |
|---|---|---|
| DAC disconnection | `benchmark/dac-off-probe.asm`, `benchmark/verify-dac-off.py`; `figures/dac-off-transition.png`, `hardware/lsdj-dac-excerpt.wav`, `results/causal-findings.json` | LSDj rest-tail excess RMS fell from 23.42 to 2.49 dB with the original fixed alignment/gain. Generated DAC-off fixture has 336 silent gaps; that particular fixture was not recorded on hardware. |
| Noise feedback latch | `benchmark/run-noise-feedback-test.sh`; the steady and retrigger ROMs/FLACs; `results/retrigger-hardware-findings.json`, `results/noise-feedback-regression.json` | Falling-edge feedback capture explains the physical 64-phase retrigger pattern; 604 observed cycles, phase-mean timing RMS error 2.329 µs. Native/legacy state and non-DMG controls are documented separately. |
| Retain silent pulse clocks | `benchmark/run-pulse-clock-test.sh`; original torture recording, cases 70 and 72; `results/pulse/` | Whole duty-step phase disagreements fell from 49/62 to 0/62 bursts on each pulse channel. |
| Retain pulse duty output | `benchmark/run-pulse-duty-test.sh`; original torture duty-trigger cases and `results/pulse/duty-oracle.json` | Retriggers and volume writes preserve the duty latch, including zero-volume retention; native and legacy save-state cases are tested. |
| Refresh mixer without changing held DAC samples | `benchmark/run-mixer-dac-test.sh`; original torture, especially case 189 | The internal forced-refresh sentinel must not replace a disabled DAC’s held sample. Focused tests cover DMG-B, CGB-E and AGB behavior; this change is not restricted to DMG. |
| Wave-RAM retrigger majority | `benchmark/wave-majority-repro.asm`, `benchmark/wave-probe-check.py`; v2 recording and committed physical oracle | All 17 v2 cases/272 bytes and the original 60 cases/6,337 bytes match. This is observed RAM corruption, not a claim that the readback probe sounds better. Other DMG specimens may retain the ordinary block-copy variant. |
| Positive-sweep stops | `benchmark/run-sweep-map-test.py`, `benchmark/check-physical-probe.py`, `benchmark/generate-sweep-table.py`; v2 and history recordings/oracles | A guarded 1,792-byte empirical map matches 11,587 measured ordinary positive transitions: 192 stops, 11,395 survivors. Six later direct/second-update pairs also agree. This is not a transistor model or proof for every history/revision. |

`results/pr-head/` contains the final publication-head focused checks: all four C regressions pass; all 57 v2 CPU readback cases match the physical oracle with zero differing bytes; sweep-header regeneration and diff checks pass. The complete 189-case and 58-case WAV comparisons in `results/measured-map/` were captured at the pre-publication head `61d83fe7deeb4c90eef003b7933d0a9dd5fa7f2c`. The publication changes were braces/comment/message cleanup, with no changed executable behavior. The full takes have 337/337 and 116/116 packets respectively; history has 26/26.

The broader historical SameSuite report is preserved under `results/historical-broad/`: existing failures were unchanged from its preserved baseline, not converted into passes. It is not a fresh all-platform run on the publication head. `results/measured-map/regressions/` separately records the final six upstream sanity cases and two documented DMG-B DIV cases. The performance report is one before/after sweep-map run on the same finite 160.121-second ROM with 48 kHz audio: about 5.06/5.05 seconds wall time on the tested Ryzen 5650U. It is a throughput observation, not a statistical performance claim for the whole seven-commit series.

## Integrity and licenses

`manifest.json` lists every published file’s SHA256 and size. `provenance-sources.json` maps archived source/report copies to their originals and describes each portability change. Original manifests and physical-oracle JSON are retained verbatim so their established hashes remain meaningful; historical absolute paths inside them are provenance, not installation requirements.

`hardware/manifest.json` records each original WAV SHA256, FLAC SHA256, original sample format/frame count, and independent decoded-PCM verification. Every complete FLAC reproduces the original interleaved PCM exactly. WAV container metadata is omitted, so decoding does **not** recreate the original container SHA256. Original WAVs were not modified. `hardware/capture-integrity.json` retains only relevant saved/status/format fields; private recording controls and notifications are excluded.

The LSDj excerpt is exactly original left-channel PCM24 frames `[90735, 819663)`, 15.186 seconds, with no resampling, filtering, or gain. Its separate provenance JSON records the full recording’s hash without distributing that full song. No LSDj program/save is present. Recreating the original LSDj playback therefore requires independently obtained inputs; the included excerpt, figure, and historical reports are evidence rather than a self-contained LSDj rerender recipe.

New source code, ROM source, and documentation use the Expat license in `LICENSE`, copyright Jeff Lynn. Existing SameBoy notices are preserved in `licenses/SameBoy-LICENSE.txt`; software-derived portions remain subject to them. Recordings, music excerpts, and audio-derived figures/data are supplied as review evidence, without a blanket software-license grant. No experimental circuit implementation, dmg-sim source, extracted netlist, or research-model CSV is included. The two files under `benchmark/sweep-circuit/research/` are authored CPU test fixtures only. Historical manifest descriptions mentioning circuit predictions describe unconfirmed hypotheses; the measured readbacks are the oracle.

## Reproduce

Use Python 3.11+ with NumPy and SciPy, RGBDS (`rgbasm`, `rgblink`, `rgbfix`), FLAC, a C compiler, GNU make, and ripgrep. Matplotlib is needed only for optional plots. SameBoy’s own build dependencies apply. Existing validation reports record the tested tool versions. Run these commands from this evidence directory, using a separate SameBoy checkout:

```sh
sameboy_checkout=/absolute/path/to/SameBoy
python evidence.py verify
python evidence.py rebuild
python evidence.py decode
```

`rebuild` assembles the three frozen suites and two noise ROMs and requires byte-for-byte identity with the five recorded ROMs. It does not need the old sweep-model CSV. `decode` creates `build/hardware/*.wav` and validates original PCM and format. To decode only one take, use `--recording sweep-history-vvg4l2b6` (or another ID in the hardware manifest). `build/` is generated output, excluded from the evidence manifest and Git. The verifier checks its explicit file list; `.git/` is irrelevant.

Focused code regressions and physical CPU readbacks:

```sh
bash benchmark/run-noise-feedback-test.sh "$sameboy_checkout"
bash benchmark/run-pulse-clock-test.sh "$sameboy_checkout"
bash benchmark/run-pulse-duty-test.sh "$sameboy_checkout"
bash benchmark/run-mixer-dac-test.sh "$sameboy_checkout"
python benchmark/check-physical-probe.py --sameboy "$sameboy_checkout" --out build/cpu-v2
python benchmark/run-sweep-map-test.py --sameboy "$sameboy_checkout" --out build/sweep-boundaries
python benchmark/wave-probe-check.py --sameboy "$sameboy_checkout" --out build/wave-minimal --minimal
python benchmark/wave-probe-check.py --sameboy "$sameboy_checkout" --out build/wave-v2
python benchmark/wave-readback-check.py --sameboy "$sameboy_checkout" --all-wave
python benchmark/generate-sweep-table.py --check "$sameboy_checkout/Core/dmg_sweep.h"
```

The output directories for CPU/map/render commands must be new, protecting prior measurements. `wave-probe-check.py --minimal` uses the reduced standalone wave ROM instead of the acoustic engine. The frozen v2 suite already contains every measured sweep trial; `check-physical-probe.py` reads CPU results directly and does not infer them from sound.

To render the finite v2 suite and independently decode its hardware recording:

```sh
python benchmark/torture/render.py --sameboy "$sameboy_checkout" \
  --manifest artifacts/audio-oracle-probe-v2/manifest.json --out build/v2-render
python benchmark/torture/analyze.py \
  --reference build/v2-render/audio.wav --markers build/v2-render/markers.csv \
  --manifest artifacts/audio-oracle-probe-v2/manifest.json \
  --hardware build/hardware/audio-oracle-probe-v2-erekgdxk.wav --out build/v2-decoded
python check-packets.py --suite v2 --metrics build/v2-decoded/metrics.json
```

This fresh analysis checks packets and fits global calibration against the selected reference. For the **historical frozen clock/offset/gain comparison**, use the preserved baseline report instead:

```sh
python benchmark/torture/compare_candidate.py build/v2-render \
  --baseline artifacts/audio-oracle-probe-v2/comparison-erekgdxk-reference/metrics.json \
  --out build/v2-frozen.json
```

The portable comparator verifies decoded hardware PCM, reuses the validated physical packet decoding, and requires identical ROM and CPU marker timing. It retains the original global calibration for every case. It does not optimize per-case fits or apply an EQ. Waveform/spectral metrics have no invented pass threshold; not every metric improves.

For the original 189-case suite, use `artifacts/torture/full/manifest.json`, `build/hardware/torture-pula2a6h.wav`, and the baseline report `artifacts/torture/comparison-torture-pula2a6h/metrics.json`. The pulse edge measurement can then be reproduced with `python benchmark/pulse-edge-oracle.py --render build/torture-render --out build/pulse-edges.json`. Its included auxiliary NPZ retains only the exact four existing hardware/time arrays used by cases 70/72; its baseline CSV retains only the 128 original trigger rows needed by that checker.

For the short history suite, use `artifacts/sweep-history-probe/manifest.json` and `build/hardware/sweep-history-vvg4l2b6.wav` in the render/analyze commands, then:

```sh
python check-packets.py --suite history --metrics build/history-decoded/metrics.json
python benchmark/sweep-history-probe.py \
  --manifest artifacts/sweep-history-probe/manifest.json \
  --metrics build/history-decoded/metrics.json --report build/history-pairs.json
```

Noise waveform and spectral analysis using the preserved before/after emulator captures:

```sh
python oracle-analysis/steady_probe.py --hardware build/hardware/steady-noise.wav \
  --emulated artifacts/noise-probe/steady-mbc5.wav --out-prefix build/steady
python oracle-analysis/retrigger_hardware.py --hardware build/hardware/retrigger-wqowpq82.wav \
  --emulated artifacts/noise-probe/retrigger-mbc5.wav \
  --trace artifacts/noise-probe/retrigger-mbc5.csv \
  --prototype artifacts/noise-feedback-fixed/retrigger.wav --out-prefix build/retrigger
```

The original 15.186-second analysis duration is retained in `benchmark/baseline.json`. These noise fixtures loop; render them for the same established 18.3-second span if creating new emulator captures with `benchmark/capture.c`. The frozen torture suites instead run to their explicit suite-complete marker.

For the standalone DAC-gap fixture, the suite render above builds a capture executable usable here:

```sh
bash benchmark/run-dac-off-probe.sh build/v2-render/capture artifacts/noise-probe/gate-hold "$sameboy_checkout"
python benchmark/verify-dac-off.py --prefix artifacts/noise-probe/gate-hold \
  --silence-only --out build/dac-gap-results.json
```

`--silence-only` explicitly checks the 336 DAC-off intervals. The historical strict control-identity test is not a final-series claim: the later mixer correction intentionally changes the control’s startup transient. Its recorded failure and successful silent-gap result are preserved in `results/historical-broad/dac.json`. No hardware recording of this minimal DAC fixture is claimed.

`results/packaging-portability.json` records the packaging checks. Existing complete WAV analyses and broad emulator suites were reused, not rerun for packaging.
