# Package verification — 2026-10-28

These are new runs of the packaged reproduction tools, not just copied claims
from the original experiment. Host binaries and build objects are deliberately
excluded; the logs and machine-readable results are included.

## Rebuild and recording checks

- RGBDS 1.0.0 rebuilt both ROMs byte-for-byte, including generated case bodies,
  case tables, and manifest case metadata: initial 65,536 bytes; focused
  131,072 bytes. Output paths in generated manifests naturally differ.
- All three raw WAVs decoded independently on both channels: 170 packets for
  each initial-ROM take/channel and 234 for the focused take/channel. Complete,
  ordered, checksum-valid coverage and all readbacks matched the archive.
- The calibration's largest numerical difference was 6.9389e-17. A first
  package check used exact JSON float equality and failed on three last-bit
  reduction differences, despite identical clock/offset and decoded packets.
  The comparison now permits 1e-12 absolute/relative rounding for calibration
  numbers only. It still rejects a 1 microsecond offset change; packet and
  readback comparisons remain exact. See `decode-summary.json`.
- Recomputed focused CH3 and CH4 measurements and all 14 sensitivity variants
  matched the archived numeric results exactly. Only relocated input paths
  and adapted-script hashes were excluded from provenance comparisons.

## Current-core regression

Clang 23.1.4 and GCC 16.2.1 separately built the current static library and
rendered both ROMs. Both compilers produced byte-identical PCM, APU traces,
and marker traces, and both passed:

| Recording | CH3 | CH4 |
| --- | --- | --- |
| Focused `ew_png0d` | 62 cases; max error 0.528597 sample units | 54 cases; max error 65.884385 T |
| Initial `xryrmxjl` | 36 cases; max waveform RMSE 0.013548 | 48 cases; max onset error 110.644404 T |
| Repeat `qw3uz2uv` | 36 cases; max waveform RMSE 0.013375 | 48 cases; max onset error 110.793877 T |

The seven current companion regressions pass with both compilers. The four
pending-wave boundary combinations also pass, preserving exposed sample 5.
Compiler-specific logs include build provenance and individual errors.

The package was additionally copied to a different directory and tested
against a fresh `git archive` of the current source, with no lab artifacts in
that source tree. ROM rebuild, focused measurements, and the complete Clang
regression passed there too; see `relocated/`. Original lab paths survive in
archived metadata only and are not required to load the packaged inputs.

## Scope

This package test does not rerun sanitizers, native state roundtrips, upstream
screenshot sanity, or full library packaging. Historical results and their
limitations remain in the nested reports. The separate prior integrated
six-test upstream sanity result is retained under `../scope-cleanup/`.

Tool versions: Python 3.14.4, NumPy 2.5.4, SciPy 1.18.1, Matplotlib 3.11.2,
RGBDS 1.0.0. Exact numerical archive comparison was exercised with this
environment; other numerical-library versions have not been validated here.
