# PR #790 original-scope cleanup

Baseline: `40c34cf931aa14e0a56ff78c804648a878af808b`, following cleanup
`82b45cfe`. No commit, push, post, main-build write, or other source-file edit
was performed. `CONTRIBUTING.md` was read. The existing repository-local
exclude already contained `/.amp/in/`.

## Changes

Only two executable conditions changed in `Core/apu.c`:

- `update_square_sample`: use the retained duty bit on all pre-CGB models,
  rather than DMG-B alone.
- Background square clock in `GB_apu_run`: continue frequency/duty clocks
  after length/sweep expiry on all pre-CGB models while the clock gate is set.

Six scoped `TODO: only tested on DMG-B` comments mark the pre-CGB assumptions:
DAC synthesis timing, last-DAC disconnect, first-DAC reconnect, pulse duty,
pulse background clock, and noise-feedback latching including reset/NR43.
The reviewer request was retrieved read-only through `gh api`; see
`reviewer-request.json` and its PR discussion URL.

The empirical sweep map, `read_wave_sample_bus`, and the focused NR42
post-expiry gate remain DMG-B-only. Wave-RAM corruption is untouched; its
existing MGB confirmation is not evidence for these other assumptions.
`verification.log` includes a comment/whitespace-stripped comparison proving
that the only executable differences are the two intended conditions.

## Tests and red/green result

Extended the existing `.amp/in/tests/pulse-clock-test.c` and
`pulse-duty-test.c`; no new permanent test or shared helper file was added.
Existing DMG internal-state/save-state checks were preserved, including their
`PULSE_CLOCK_STATE` and `PULSE_DUTY_STATE` configurations.

New assertions use the agreed observable seam: public register writes/NR52
reads and stereo PCM callbacks. They synchronize from audible pulse edges,
not private duty indices, latches, or channel samples. Like `dac-timing-test`,
the fixture drives the APU/sample clocks directly at one sample per APU tick
and bypasses the SGB intro animation. This is not a CPU instruction-timing
test or a new hardware measurement.

Each test covers CH1 and CH2 on all eight pre-CGB variants from
`pre-cgb-models.h`, plus CGB 0/A/B/C/D/E, AGB, and GBP controls (32 model/channel
cases per executable).

| Check | Baseline Clang/GCC | Final Clang/GCC |
| --- | --- | --- |
| Pulse duty | 14 failed checks, exit 1 | 0 failures, exit 0 |
| Pulse clock | 14 failed checks, exit 1 | 0 failures, exit 0 |
| Five other companion regressions | All pass | All pass |
| Existing pending-wave scratch check | Four cases pass | Four cases pass |

All 14 intended failures in each red executable are MGB/SGB-family cases,
two channels per model. DMG-B and every CGB/AGB/GBP control pass both before
and after, with unchanged logged control output.

- Duty: a 12.5% to 25% duty rewrite/retrigger at the falling edge must retain
  digital zero until the next clock on pre-CGB. Each 512T window checks all
  256 stereo samples. Baseline MGB/SGB instead produces -4080; final produces
  the expected +4080. CGB remains -4080 and AGB/GBP remains -8160.
- Clock: six periods of post-expiry waiting must advance the next audible
  edge by 12,288T on pre-CGB. The immediate edge is 14,348T after retrigger;
  the background case becomes 2,060T, versus 14,348T before cleanup on
  MGB/SGB. DAC-off, followed by DAC-enable without triggering, must leave
  the edge at 14,348T. NR52 stays clear and the expired channel remains at
  its digital-zero PCM level while waiting. CGB/AGB/GBP remain at 14,348T
  for all three histories.

The duty test was run red before its one-condition change and then green.
The clock test was subsequently run red before its one-condition change and
then green. Both baseline archives were built before either change.

The five unchanged companion tests are mixer-DAC, DAC edge, DAC reconnect,
DAC timing, and noise-feedback/save compatibility. Each test's entire output
is byte-identical red/green for each compiler. All seven final companion
outputs and pending-wave output are also byte-identical between compilers.

The existing focused CH3/CH4 ROM was rendered through its suite-end marker
on both compilers, before and after: 45.076791763 emulated seconds,
2,163,686 stereo samples, 38,343 APU writes, and 469 marker events. WAV,
screenshot, APU trace, and marker trace are byte-identical before/after for
each compiler. This preserves the existing focused behavior; physical
recording analysis was not rerun.

## Logs and reproduction

All paths below are relative to this directory:

- `clang-red-build.log`, `gcc-red-build.log`: isolated baseline library builds.
- `clang-green-build.log`, `gcc-green-build.log`: isolated final builds.
- `clang-duty-green-build.log`: first duty-only green library build.
- `{clang,gcc}-pulse-{clock,duty}-{red,green}.log`: canonical test results.
- `{clang,gcc}-{mixer-dac,dac-edge,dac-reconnect,dac-timing,noise-feedback}-{red,green}.log`:
  companion regression output; corresponding `-compile.log` files capture builds.
- `{clang,gcc}-pending-wave-{red,green}.log`: unchanged scratch boundary test.
- `{clang,gcc}-focused-{red,green}.log`: complete focused-ROM render summaries.
- `{clang,gcc}-{red,green}/focused/`: WAV, screenshot, and traces.
- `verification.log`: exact comparisons, source-scope audit, main-build hashes.
- `provenance.txt`, `main-build-before.sha256`, `core.patch`, `pulse-tests.patch`:
  baseline/compiler/input hashes and complete edits, including excluded tests.

Compiler versions: Clang 23.1.4 and GCC 16.2.1 (20260810). Libraries were
built with the repository's debug configuration and default warning-as-error
flags; test drivers use `-O2` with assertions enabled. No sanitizers, full
header packaging, upstream screenshot sanity suite, or standalone sweep ROM
validation were run here.

From the repository root, build the final static archive without touching
the main build (repeat with `compiler=gcc`):

```sh
compiler=clang
root=.amp/in/artifacts/scope-cleanup/$compiler-green
make -j4 CC="$compiler" CONF=debug OBJ="$root/obj" LIBDIR="$root/lib" \
    "$root/lib/libsameboy.a"
for test in pulse-clock pulse-duty mixer-dac dac-edge dac-reconnect dac-timing noise-feedback; do
    "$compiler" -std=gnu11 -O2 -Wall -Werror -Wno-multichar \
        -DPULSE_CLOCK_STATE -DPULSE_DUTY_STATE -I. \
        ".amp/in/tests/$test-test.c" "$root/lib/libsameboy.a" \
        -lm -ldl -o "$root/$test"
    "$root/$test" .amp/in/artifacts/ch34-focused-probe/rom/ch34-focused-probe.gb
done
```

Replay red with the preserved `*-red` binaries/archives; do **not** run make
against those directories with the final source, which would replace the
baseline. `apu-before.c` and `pulse-{clock,duty}-before.c` preserve the inputs
for audit. `build/lib/libsameboy.{a,o,so}` hashes are unchanged.

## Initial check corrections and remaining uncertainty

The first clock-test draft used the active AGB envelope bias to predict
inactive PCM. AGB drops that bias on expiry, so its expected inactive level
is +4080, not zero. This fixture error caused four extra failures; the logs
are retained as `{clang,gcc}-clock-initial-fixture.log`. It was corrected
before changing the production clock condition, after checking the existing
AGB mixer behavior. The corrected red tests still fail exactly the 14
intended MGB/SGB cases. An initial GCC driver compile lacked the repository's
`-Wno-multichar` option and rejected existing save-state magic constants;
subsequent invocations use that existing project warning policy.

Pre-CGB generalization is the reviewer's requested assumption, not new MGB
or SGB hardware proof. Only retrigger wave-RAM corruption has the stated MGB
confirmation. The focused fixes and empirical sweep map keep their narrower
evidence boundaries. No new unresolved implementation failure was found;
hardware confirmation of the generalized assumptions remains outstanding.

## Parent integration after the scoped implementation

The sections above describe the subagent's preserved snapshot, not the
subsequent parent integration. The standards review found no hard violation.
The evidence review correctly noted that the physical CPU revision is
unconfirmed. All six TODOs now say "only measured on one DMG (CPU revision
unconfirmed)" and mark the pre-CGB generalization as an assumption. No
executable condition changed after the scoped implementation's checks.
`integrated-core.patch` captures this final wording.

The parent rebuilt the main release core, shared library, exported headers,
and tester after that comment clarification. The initial integration
invocation accidentally used the default debug configuration with existing
release/LTO objects and failed to link (`final-integration-build.log`). A
forced, consistently configured `CONF=release` rebuild succeeded
(`final-release-rebuild.log`); no source or warning checks were bypassed.
Consequently, the main-build preservation hashes above describe only the
earlier isolated subagent work, not the now-rebuilt main outputs.

All six upstream screenshot hashes passed on the final tester; see
`final-integration-sanity.log` and `sanity-output/`. The earlier successful
sanity run is retained separately. Existing focused-ROM before/after
comparisons remain byte-identical; no additional physical recording was
made.

The standalone sweep ROM passed the sibling `sweep-verify/validation`
checks: full-domain timing/readback equality against the original audio
ROM, four emulated models, three synthetic reporting fixtures, navigation,
invalid handling, and complete retained results. A further run against the
final release core exactly matched the preserved DMG-B baseline's complete
trial trace, WRAM, and all ten screenshots. Representative intro, matching,
differing, and invalid screens were visually inspected. Synthetic screens
are labeled as such and do not constitute hardware evidence.

The scope change is assigned to a local fixup of the original review-cleanup
commit, not the focused CH3/CH4 commit. Experimental tests and ROM artifacts
remain locally excluded. No push, PR post, or cartridge flash was performed.
