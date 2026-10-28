# Standalone CH1 sweep comparison ROM

This 32 KiB Game Boy program displays observations on the console; no audio
recording, decoder, cartridge RAM, or save battery is required.

**Hardware result, 2026-10-28:** the exact `rom/sweep-verify-mbc5.gb` variant
ran on Jeff's same DMG: **11587 trials, 192 stops, zero differences, zero
invalid initial controls**. See [the photo and hardware report](hardware/README.md).
The CPU revision is unknown. This is same-unit confirmation of the earlier
audio-derived reference, **not cross-device evidence or a universal PASS**.

`rom/sweep-verify.gb` is the ROM-only distribution image. It has the same
program and reference bytes but has **not separately been physically
boot-tested**. Both variants have emulator evidence; their only differences
are the three header bytes documented below. This packaging work did not
flash hardware, change Core, publish, push, or commit anything.

## Run it

1. Use `rom/sweep-verify.gb` with a ROM-only-compatible loader, or the exact
   hardware-tested `rom/sweep-verify-mbc5.gb` with suitable MBC5 hardware.
   Cartridge programming is a separate, user-controlled operation; no
   programmer tools or commands are included. Power on the prepared cartridge.
2. Press **Start** at the intro. The LCD intentionally goes blank and the
   program runs silently for about **95 seconds** (allow 100 seconds).
   Do not power off during this interval. The LCD and interrupts stay off to
   preserve the original measurement conditions; there is no progress display.
3. Photograph the results summary, along with the console model and known
   board/CPU markings. If the hardware revision is unknown, say so.
4. Press **Left/Right** to cycle through the summary and shifts 1–7. On a
   shift page, **Up/Down** selects the previous/next differing or invalid
   frequency, wrapping at the ends. **Start** returns to the summary.
   Photograph differing or invalid detail pages as well. Allow a brief pause
   for scanning when moving between pages.
5. Results remain visible until power off. Power-cycle to repeat. Results
   exist only in WRAM; the ROM does not save or modify cartridge SRAM.

No recording needs to be cued. The results screen is the deliverable from a
physical run. Testing another console is useful even if it differs from the
reference: **a difference is not a hardware failure**.

## Read the results

| Field | Meaning |
| --- | --- |
| S | Positive-sweep shift, 1–7 |
| STOP | Initially active trials that became inactive after the first update |
| REF | Stop count from Jeff's one recorded DMG |
| DIFF | Valid observations that differ from that unit, compared by frequency rather than count alone |
| BAD | Initially inactive trials; excluded from STOP and DIFF |

On detail pages, `FREQ $xxx` is the original frequency, in hexadecimal.
`NR52 PRE` and `POST` are channel-1 active bits read before/after the first
sweep update. `JEFF` shows the recorded reference; `UNIT` shows the current
run. `00` and `01` are both invalid initial states, not evidence of a sweep
stop. `NO DIFFERENCES` means neither differing nor invalid trials for that
shift. If any initial state is invalid, the summary says `INVALID INITIAL
DATA`, even if other valid trials also differ.

`MATCHES JEFF'S UNIT` means every measured pair matches within this specific
experiment. It is **not** a universal hardware PASS or proof of a circuit
model. `DIFFERS FROM JEFF` is a useful observation to investigate.

| Shift | Trials | Reference stops |
| --- | ---: | ---: |
| 1 | 911 | 127 |
| 2 | 1312 | 54 |
| 3 | 1619 | 7 |
| 4 | 1815 | 0 |
| 5 | 1926 | 2 |
| 6 | 1986 | 1 |
| 7 | 2018 | 1 |
| Total | 11587 | 192 |

## Measurement boundary

For each shift `s = 1..7`, include original 11-bit frequencies `f` for which
`n = f + (f >> s)` and `n + (n >> s) < 2048`. These exclude ordinary overflow
at the checked updates. Each trial resets APU and DIV independently, sets
positive sweep pace 1, duty `$80`, envelope `$F0`, loads the frequency, and
triggers CH1 with routing off. It then waits 8192 T-cycles, samples NR52 bit
0, waits 24576 more T-cycles, and samples again. Instruction overhead is
additional; the frozen assembly, not these rounded descriptions, defines
the exact schedule.

All 13 measurement batch bodies, including packing, resets, loops, waits,
and padding, are copied unchanged from the earlier sweep-only probe. They
also match the batches in the physically recorded audio-oracle-v2 ROM
apart from labels. Only dispatch, result retention, and presentation change.
Four trials fit a byte, in `pre0 post0 pre1 post1 pre2 post2 pre3 post3`
order. Full pairs are retained at `$C000`, with 512 bytes per shift;
unmeasured padding is excluded from comparisons.

This is exhaustive for this settled, positive-sweep, first-update domain,
not every sweep history, timing phase, negative-sweep mode, or hardware
revision. It does not settle CH3/CH4 behavior or the other PR changes.

## Provenance and rebuilding

The comparison data is `inputs/dmg-probe-v2-erekgdxk.json`, decoded from one
physical DMG recording. Its CPU revision is unconfirmed; the emulator model
name DMG-B is not a physical revision identification. The JSON contains
recording, source-ROM, and source-manifest hashes and packet coverage.

The published frozen evidence is at:
https://github.com/jefffm/SameBoy/tree/6a39117b4a771a4838b156441b09176c12a56c85/artifacts/audio-oracle-probe-v2

The included `inputs/sweep-cases.inc` and `inputs/sweep-manifest.json` come
from the corresponding sweep-only probe. Historical paths in these inputs
are provenance, not build dependencies. The packaged inputs are sufficient
to rebuild without the original lab or a SameBoy checkout.

Requirements: Python 3.8+ (standard library only) and RGBDS **1.0.0**.
From this directory, using either an isolated RGBDS directory or tools on PATH:

```sh
python3 build.py --rgbds /path/to/rgbds-1.0.0 --out rebuilt
# Alternatively, with RGBDS on PATH: python3 build.py --out rebuilt
# Check the bundled artifacts without any assembler or SameBoy installation:
python3 build.py --check-only
```

`rebuilt` must not already exist. The script builds from the bundled assembly,
font, and frozen observations, then checks **both complete output hashes**, all
frozen source/input hashes, domain coverage, reference counts, the 32 KiB size,
title/type/ROM/RAM header fields, and both checksums. It verifies exactly these
differences (ROM-only → MBC5), not merely a list of allowed offsets:

| Offset | ROM-only | MBC5 | Meaning |
| --- | --- | --- | --- |
| `$147` | `$00` | `$1B` | ROM-only → MBC5+RAM+battery cartridge declaration |
| `$149` | `$00` | `$04` | No RAM → 128 KiB RAM declaration |
| `$14D` | `$EB` | `$CC` | Header checksum |

The global checksum stays identical: the byte changes cancel in the sum.
The MBC5 RAM/battery declarations match the tested cartridge; the program
still never accesses cartridge RAM and requires no battery. All non-header
bytes, including every measurement instruction, are identical. RGBFIX may
warn that it overwrites the existing header checksum when making the variant;
that is intentional and the final checksum is independently verified.

| Image | SHA-256 |
| --- | --- |
| `sweep-verify.gb` | `cfb29be1b03c7cb6ebc76a63e2260455b745e09aea4966e26de8fafbf4217d09` |
| `sweep-verify-mbc5.gb` | `784825ebfa76fcb33f45aeb12275d34cfdf373a4e9f176f2bea6452e3d507b03` |

Rebuilt outputs and a `build-checks.json` report go only into the new output
directory; temporary object/generated-input files are removed automatically.
Do not use `python -O` or `PYTHONOPTIMIZE`: the scripts reject disabled assertions.
`archive/build-manifest.json` is the unchanged historical build record, not a
manifest for this repackaged tree. Its `build.py` hash identifies the original
wrapper; this adapted wrapper deliberately differs. The assembly, macros and
all frozen inputs remain byte-identical. The font is
derived from SameBoy's SDL font; `SameBoy-LICENSE` accompanies it and the
other reused SameBoy material.

### Empirical map reproduction and focused regression

The [portable companion sweep-map path](map/README.md) is included with its
original minimal CPU/assembly fixtures. It accepts final `Core/dmg_sweep.inc`
and legacy `Core/dmg_sweep.h`; no lab paths or circuit models are needed:

```sh
python3 map/generate-sweep-table.py --sameboy /path/to/SameBoy
python3 map/run-sweep-map-test.py --sameboy /path/to/SameBoy \
  --rgbds /path/to/rgbds-1.0.0 --out new-map-results
```

This reproduces the full 1,792-byte include from the frozen oracle and executes
the existing eight stop/survivor boundary cases against freshly compiled Core.
The table and assertions share a physical capture: passing checks verify
encoding/integration, not independent predictions of other hardware.

## Validation and limitations

The local validation harness uses public SameBoy CPU-memory, input, and
pixel callbacks. It checked all 11587 pairs and the per-trial DIV-to-trigger,
trigger-to-first-read, and first-to-second-read intervals against the old
sweep-only audio ROM. Every row matched on the preserved DMG-B baseline.
The complete standalone run also matched the final release build in trial
trace, retained WRAM, and all captured screens.

Boot, summary, detail, previous/next wrapping, shift navigation, and Start
return were exercised on emulated DMG-B, MGB, SGB, and SGB2. The DMG-B run
matched all 192 reference stops. The other emulator models lack the
DMG-B-only empirical table and reported 192 differences, not hardware
failures. These checks establish emulator execution, **not physical
cross-model confirmation**. Measurement/reporting took 94.098 seconds on
emulated DMG-B, about 94.100 on MGB/SGB2, and 91.884 on SGB's faster clock.

Injected NR52 observations separately exercised missing/extra stops,
invalid `00`/`01` pairs, all 11587 results inverted, and all 11587 initial
controls invalid. Retained bits, all counts, detail text, and wraparound
were checked independently of the ROM's analysis. These inputs test the
reporter only; they are never hardware evidence. No cartridge SRAM reads
or writes occurred, and the LCD was off at every sampled observation.

`archive/validation/` contains unchanged historical reports, logs, traces,
screen text and retained WRAM. Its `checks.json` says `hardware_tested: false`
because it is a pre-hardware **emulator-validation record**, not the current
hardware status. `archive/variant.json` and `archive/flash-readback.json` are
unchanged variant and prior flash/readback provenance. Absolute paths in
archival JSON and frozen inputs are historical metadata, never dependencies.
See [the archive index](archive/README.md).

Screenshots are actual emulator renders;
files ending in `synthetic` use injected reporting inputs. Composite images
only place two unchanged viewports side by side with a separating gutter.

### Portable emulator/reporting check

Requires a GCC/Clang-compatible C compiler on a POSIX host, Python 3.8+, a
SameBoy source checkout, and the open-source SameBoy replacement boot ROMs
`dmg_boot.bin`, `mgb_boot.bin`, `sgb_boot.bin`, `sgb2_boot.bin` (not Nintendo
boot-ROM dumps). These are explicit external inputs; no lab directory is used.
If necessary, build replacement boot ROMs in your own disposable SameBoy
checkout using that project's `make bootroms` target and its RGBDS requirements.

```sh
python3 validation/check.py \
  --core /path/to/SameBoy \
  --bootroms /path/to/SameBoy/build/bin/BootROMs \
  --out emulator-results
```

The output directory must be new. This compiles the specified Core sources
directly with assertions enabled (`-O2`, no stale library), uses the unchanged
public-callback harness, and writes nothing to that checkout. It checks four
emulator models, three synthetic reporter patterns, navigation, retained bits,
counts, detail text, and both variants on DMG-B. The variants must have identical
trial traces, retained WRAM, screen text and pixel renders. DMG-B observations
must match the frozen reference; this is a regression check for the reviewed
core, not an expectation that arbitrary upstream versions implement the table.

The original audio ROM is **not rerun**. Its bundled archival trace is compared
row-for-row, including the three timing intervals, to the fresh ROM-only DMG-B
run. Harness executables and injected fixtures are temporary and deleted.
Results retain compiler output, source/boot hashes, run logs and traces.
Compiler/invocation paths in generated reports are execution provenance only.

See [verification/README.md](verification/README.md) for the actual fresh-copy
build and current-core check performed during packaging. These remain emulator
checks, not new hardware observations. Outstanding physical work is additional
identified units/revisions and a separate ROM-only boot check. The existing
photo is only a summary: no per-trial hardware dump or hardware timing/run-duration
measurement is claimed. This bundle does not settle CH3/CH4 or the rest of PR #790.
