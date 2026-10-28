# Packaging verification — 2026-10-28

These are new local software checks of the copied package, not new physical
measurements. Original artifacts and Core were read-only. No upload, flash,
commit, PR post, top-level package manifest or archive was created.

## Environment and results

- Linux x86-64; Python 3.15.1; GCC `16.2.1 20260810`.
- RGBDS 1.0.0 was used from the isolated local `.amp/in/tools/rgbds-1.0.0/`,
  not a system installation. RGBDS executables are not bundled.
- SameBoy checkout HEAD: `e31144f18194ff8dc9f9e648a7cac7be412d3aec`.
  Core file hashes were recorded before compilation and checked again after
  execution. The existing library was not used. Replacement boot ROM hashes
  are recorded in `current-core/checks.json`.
- Fresh-copy rebuild reproduced both exact 32 KiB ROMs and both header
  checksums. The only differences were `$147:00→1B`, `$149:00→04`,
  `$14D:EB→CC`. All measurement instruction bytes were preserved.
- Eight full standalone emulator runs passed: four native models, three
  synthetic reporter cases, and the hardware-header variant on DMG-B.
- The full DMG-B trial trace, including all three relative timing columns,
  matched the bundled original audio-ROM trace. That original ROM was not rerun.
- Both variants had identical full trial traces, retained WRAM, all captured
  screen text and PPM renders on DMG-B.
- The map generator reproduced current `Core/dmg_sweep.inc` byte-for-byte:
  SHA-256 `22f7eda92ecc27b3fabc03179dbf82f7e0c9ccd3543fc9fe50e01d3509a7dac7`.
  The eight-case CPU boundary regression passed in both the package and a
  fresh copy. Legacy `.h` table lookup also passed using a temporary fixture;
  no legacy core revision was rebuilt.

| Emulator/reporting run | Stops | Differences | Invalid initial controls |
| --- | ---: | ---: | ---: |
| ROM-only, DMG-B | 192 | 0 | 0 |
| ROM-only, MGB | 0 | 192 | 0 |
| ROM-only, SGB | 0 | 192 | 0 |
| ROM-only, SGB2 | 0 | 192 | 0 |
| Synthetic mixed | 194 | 5 | 3 |
| Synthetic opposite | 11395 | 11587 | 0 |
| Synthetic invalid | 0 | 0 | 11587 |
| MBC5, DMG-B | 192 | 0 | 0 |

Each run covers 11,587 pairs. Synthetic rows exercise the reporter only.
Other emulated models do not use the DMG-B empirical table; their reported
differences are not physical failures. The observer asserted LCD-off at every
sample and no cartridge SRAM accesses through completion. DMG-B measurement
and reporting time was 94.098009 **emulated** seconds, not a hardware duration.

## Exact main commands executed

From `/home/jeff/Work/SameBoy-review` (the variables below abbreviate the
literal paths used):

```sh
P=.amp/in/artifacts/pr-790-evidence/sweep
CORE=/home/jeff/Work/SameBoy-review
RGBDS=/home/jeff/Work/SameBoy-review/.amp/in/tools/rgbds-1.0.0

python3 "$P/build.py" --check-only
python3 "$P/build.py" --rgbds .amp/in/tools/rgbds-1.0.0 --out "$P/.test-build"
python3 "$P/validation/check.py" --core "$CORE" \
  --bootroms "$CORE/build/bin/BootROMs" --out "$P/verification/current-core"
python3 "$P/map/generate-sweep-table.py" --sameboy "$CORE"
python3 "$P/map/run-sweep-map-test.py" --sameboy "$CORE" \
  --rgbds .amp/in/tools/rgbds-1.0.0 --out "$P/verification/sweep-map-current-core"
```

All completed successfully. Main stdout/stderr is retained in
`current-core-run.log` and `sweep-map-current-core.log`; detailed compiler
commands and warnings are in their output subdirectories. The standalone
compiler emitted Core's existing multi-character-constant warnings; the map
compiler emitted the fixture's redundant `GB_INTERNAL` define warning. Neither
prevented compilation. RGBFIX's expected header-checksum overwrite warning is
retained in the fresh-build log. No failing check was bypassed or weakened.

## Fresh-copy test

A new `.fresh-copy/` directory under the owned sweep tree received only the
package's source/input/ROM/evidence directories and scripts, not original lab
directories, compiled tools or output directories. From that fresh directory:

```sh
python3 build.py \
  --rgbds /home/jeff/Work/SameBoy-review/.amp/in/tools/rgbds-1.0.0 --out rebuilt
cmp rebuilt/sweep-verify.gb rom/sweep-verify.gb
cmp rebuilt/sweep-verify-mbc5.gb rom/sweep-verify-mbc5.gb
python3 map/run-sweep-map-test.py \
  --sameboy /home/jeff/Work/SameBoy-review \
  --rgbds /home/jeff/Work/SameBoy-review/.amp/in/tools/rgbds-1.0.0 \
  --out ../verification/sweep-map-fresh-copy
python3 map/generate-sweep-table.py --sameboy ../.legacy-path-check
```

The last command used a temporary `Core/dmg_sweep.h` containing the unchanged
final table and no `.inc`, exercising the fallback. Results are retained in
`fresh-copy-build.log`, `fresh-copy-build-checks.json`,
`sweep-map-fresh-copy.log`, `sweep-map-fresh-copy/`, and
`legacy-header-path.log`. The temporary fresh copy, test-build directory and
legacy fixture were removed after verification; no executable/object remains.
Fresh-copy testing, not extraction from a ZIP, was used as requested.

## Reading the records

Absolute paths in logs and JSON are **execution provenance**, not dependencies
of the copied scripts. Use the parameterized commands in the main README on
another host. Paths into temporary directories deliberately no longer exist;
the retained scripts regenerate those products. The report hashes identify
what was actually built and checked; these are not a top-level package manifest.

`../archive/` is a separate historical evidence set. Its pre-run
`hardware_tested: false` flag must not override the later photograph and prior
flash/readback record in `../hardware/` and `../archive/flash-readback.json`.
The existing physical result is from Jeff's same DMG, CPU revision unknown,
using only the MBC5 variant. No cross-device conclusion or universal PASS is
supported; the ROM-only image remains separately hardware-untested.

The packaging checks do not rerun the earlier raw-audio decoder, waveform,
multi-update, or CH3/CH4 suites. Other operating systems, compiler versions
and Python 3.8 itself were not exercised here. Publication, package manifests
and final archive creation remain the integrating owner's responsibility.
