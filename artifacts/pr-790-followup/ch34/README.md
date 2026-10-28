# PR #790: CH3/CH4 recording evidence

This package contains both hardware-tested ROMs, all three original lossless
recordings, frozen reference PCM/traces, analysis, and the current external
regressions. It is a local release candidate, not an already published update.
It contains no programmer commands and does not flash hardware.

## Results and limits

The focused recording supports two behavioral changes on Jeff's single DMG:

- CH3's retriggered held output changes after an inactive interval with the
  DAC enabled, including with untouched wave RAM. Clocked inactive FF reads
  reproduce the tested output. This does not establish the physical low
  nibble, all CPU/bus phases, or continued hidden playback-index motion.
- CH4 retains upstream phase after length expiry. DAC disable stops its
  progression; enabling the DAC alone does not resume it before retrigger.
  Phase-preserving storage/restoration is observationally equivalent to a
  stopped physical counter here. Hidden LFSR continuation is not measured.

The physical CPU revision is **unconfirmed**. DMG-B is the selected emulator
model, not an identified revision of the recorded device. The focused changes
remain DMG-B-only. Separate pre-CGB review cleanup follows the reviewer's
requested assumption, not new MGB/SGB measurements.

The final model matches all 62 focused CH3 cases and 54 CH4 cases: maximum
errors 0.528597 sample units and 65.884385 T, respectively. The fixed
engineering allowances are 2 sample units and 128 T, not confidence intervals
or claims of cycle-exact analog accuracy. Both earlier recordings also pass
the weaker original-recording regression. The measurements retain their
original independent timing/amplitude calibration; no tested edge is shifted
individually to match a model.

The final CH3 condition includes the DAC and triggered-clock gates. A pending
read that crosses DAC disable must not inherit the rejected first candidate's
unconditional inactive FF behavior. The included `pending-wave.c` is scratch
compatibility validation of this boundary, not new physical evidence.

## Which ROM to run

| ROM | Cases | Typical time from boot | Existing recordings |
| --- | ---: | ---: | --- |
| `ch34-focused-probe/rom/ch34-focused-probe.gb` | 117: calibration + 62 CH3 + 54 CH4 | 45.08 s | `hardware/ew_png0d.wav` |
| `ch34-clock-probe/rom/ch34-clock-probe.gb` | 85: calibration + 36 CH3 + 48 CH4 | 33.26 s | `hardware/xryrmxjl.wav`, `hardware/qw3uz2uv.wav` |

Both run automatically once, leave the LCD off, emit acoustic readback packets,
then become silent. They have **no on-screen pass/fail result**. Start a stereo
headphone recording before powering on; allow 60 seconds for the focused ROM
or 45 seconds for the initial ROM after power-on. Avoid clipping, resampling,
automatic gain control, and summing the channels. Save lossless PCM, preferably
48 kHz/24-bit. Recording duration alone does not establish completeness; the
decoder must receive the final packet.

Both images use MBC5+RAM+battery headers for the tested cartridge setup but
never access cartridge SRAM. Cartridge programming is outside this package.

ROM SHA-256:

- Focused: `35e40243730d7d686721c987ef40ceb6b5dedb31a6051c0c6e4a586a75c498ca`
- Initial: `e68b3184162eb5f848e5428e12b2d89bc9139154bce776299976197e190e8882`

The focused recording's acquisition metadata names the preceding ROM. It is
preserved unchanged as historical metadata, **not** used to select the ROM.
Decoding against the focused manifest identifies the complete 234-packet run.

## Portable reproduction

Use Python 3.11 or newer. Analysis needs NumPy, SciPy, and Matplotlib; ROM
rebuilding needs RGBDS 1.0.0 on PATH. Core regression additionally needs GNU
Make, Clang or GCC, and a SameBoy checkout. No compiled host executables or
third-party music ROMs are included. The supplied boot image is SameBoy's
open-source replacement, not Nintendo's boot ROM; see `SameBoy-LICENSE`.

Run these commands from this package directory, without Python's `-O` option
(checks use assertions). Output directories must be new and outside the
package; none of these commands overwrites archived results.

```sh
export PYTHONDONTWRITEBYTECODE=1 OPENBLAS_NUM_THREADS=1
python3 evidence.py verify
python3 evidence.py rebuild --out ../ch34-rebuilt
python3 evidence.py decode --out ../ch34-decoded
python3 evidence.py measure --out ../ch34-measured
python3 evidence.py regress --source /path/to/SameBoy --cc clang --out ../ch34-clang
python3 evidence.py regress --source /path/to/SameBoy --cc gcc --out ../ch34-gcc
```

- `verify` checks every entry in `SHA256SUMS`.
- `rebuild` reproduces both exact ROMs and frozen case bodies.
- `decode` independently decodes all three packaged WAVs on both channels and
  checks packet/readback identity and calibration against the archived results.
- `measure` recomputes focused CH3/CH4 measurements and all 14 CH4 sensitivity
  variants, checking equality apart from relocated file provenance.
- `regress` builds an isolated static library from the supplied checkout,
  renders both ROMs, compares against all three recordings, and runs seven
  companion regressions plus the pending-wave boundary check. It does not
  run upstream packaging, sanitizers, or the six upstream screenshot tests.

The tested core is local commit
`e31144f18194ff8dc9f9e648a7cac7be412d3aec`; see `provenance.json` for source
hashes and tool versions. `patches/local-followups.mbox` preserves the three
unpublished commits after published PR head
`2d6ea110275b17de06d9d1c1a621adf625973cac`, with review cleanup separate from
the focused fixes. The script never applies patches or changes Git history.
Until the code is published, a checkout of only the published head is not
expected to pass the new hardware regression.

For a new hardware recording, run the packaged `benchmark/torture/analyze.py`
with that recording's path and the selected ROM's reference, markers, and
manifest, as shown by `--help`. The root `decode` command deliberately checks
the archived recordings, not arbitrary hardware. New devices need measured
comparisons rather than a universal pass/fail verdict against Jeff's unit.

## Evidence map and historical reports

- `ch34-focused-hardware-ew_png0d/ch3-analysis/REPORT.md` and
  `ch4-analysis/REPORT.md`: acquisition, calibration, measured values,
  uncertainty, rejected hypotheses, and raw analysis tables.
- `ch34-clock-probe/hardware-*/`: original and repeat recording analyses.
  Their CH4 onset tables are archived acquisition results; the earlier-take
  regression consumes these tables rather than refitting them.
- `ch34-focused-fixes/`: archived red/green results and implementation report.
- `companion-tests/`: current tests, including helper headers, scope checks,
  and the simplified old-save-state contract.
- `validation/`: fresh package reproduction results; `scope-cleanup/` retains
  prior compiler and upstream-sanity evidence.

Nested reports are historical snapshots. Their absolute lab paths and phrases
such as “no Core change” or “sweep promise outstanding” describe their original
stage, not this package's current status. This README and the fresh validation
report take precedence for runnable commands and delivery state. Portability
edits only redirect input/output paths and add CLI arguments; the measurement
math and frozen ROM sources are unchanged. The archived `candidate-pulled-up`
and `freeze-wave` PCM are rejected/intermediate experimental alternatives,
**not** the final model. Their patches are retained for provenance.

Earlier native save/load checks passed the recorded-output thresholds but
were not PCM-byte-identical; the baseline also differs around save/load.
Earlier full library packaging lacked `cppp`; a pre-existing initialization
sanitizer issue was not resolved here. Neither artifact-free state restoration,
full packaging success, nor a sanitizer-clean build is claimed.
