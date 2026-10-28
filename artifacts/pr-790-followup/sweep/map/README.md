# Reproduce and regress the empirical sweep map

This is the companion lab's existing `generate-sweep-table.py` and
`run-sweep-map-test.py` path, adapted to this portable tree. Both accept an
explicit SameBoy checkout and prefer **`Core/dmg_sweep.inc`**, falling back to
legacy **`Core/dmg_sweep.h`**. No original lab directory is required.

## Commands

Table reproduction needs only Python 3.8+ and the bundled, hash-pinned physical
oracle. It compares the complete generated include byte-for-byte:

```sh
# From the sweep/ directory:
python3 map/generate-sweep-table.py --sameboy /path/to/SameBoy
# Explicit paths work for either spelling:
python3 map/generate-sweep-table.py --check /path/to/SameBoy/Core/dmg_sweep.inc
# Optional: generate a new file, refusing to replace an existing one:
python3 map/generate-sweep-table.py --out reproduced-dmg_sweep.inc
```

The focused CPU regression additionally needs RGBDS **1.0.0** and a
GCC/Clang-compatible C compiler on a POSIX host:

```sh
python3 map/run-sweep-map-test.py \
  --sameboy /path/to/SameBoy \
  --rgbds /path/to/rgbds-1.0.0 \
  --out new-map-results
```

`--core` is an alias for `--sameboy`. Omit `--rgbds` if RGBDS is on PATH;
use `--cc clang` to select another compiler. The output directory must be new.
The runner first reproduces the selected `.inc`/`.h` from the physical oracle,
then compiles the actual Core sources with assertions and `-O2` into a temporary
directory. It does not run `make`, modify Core, or trust a prebuilt library.

No external boot ROM is needed for these eight minimal probes: their first
256 bytes supply their own synthetic boot stub. All probe ROMs, boot stubs,
objects and the native runner are temporary and deleted at completion. Logs,
generated assembly, exact commands and hash-bearing `results.json` remain.
The C fixture defines `GB_INTERNAL` itself; the direct Core build also sets it
for other translation units, so GCC's harmless macro-redefinition warning is
retained in `build.log`. No warnings or failed cases are hidden.

## What it checks

The 1,792-byte table describes 11,587 eligible first-update states: 192 stops
and 11,395 survivors across shifts 1–7. It is regenerated from
`../inputs/dmg-probe-v2-erekgdxk.json`, not inferred from the emulator table.
Unmeasured/ineligible states are zero-filled and must be excluded by the caller.

The existing focused regression executes real CPU instructions at shift 2
frequencies `$4CD`, `$51A`–`$51F` and shift 3 frequency `$64F`. Packed `10`
means initially active then stopped; `11` means still active. These cases cover
both recorded stops and survivors at boundaries that rejected circuit models
got wrong. Their minimal assembly and measurement macros are unchanged copies;
only paths/build plumbing changed. The bundled sources are:

- `fixtures/minimal-4cd.asm` and `fixtures/readback-test.c`, from the companion
  lab's `benchmark/sweep-circuit/research/`.
- `../src/macros.inc`, byte-identical to `benchmark/torture/macros.inc`.
- The shared frozen physical oracle in `../inputs/`.

These checks establish faithful table encoding and CPU integration, **not
independent prediction of unmeasured silicon**: implementation and assertions
share one unit's capture. No circuit-research netlists or their separate license
are required or included. Arbitrary register histories, timing phases, negative
sweep and other CPU revisions remain outside the measurement claim.

The full-domain standalone regression is `../validation/check.py`; the older
audio-ROM, multi-update and waveform suites are not bundled or claimed to have
been rerun here. See [../verification/README.md](../verification/README.md) for
the current `.inc` run and fresh-copy reproduction results. Legacy `.h` lookup
was checked using a temporary fixture containing the same table; this is not a
claim that an old core revision was rebuilt or validated.
