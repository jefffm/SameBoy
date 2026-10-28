# CH4 DAC-off phase/duration probe

`noise_cases.py` supplies **54 unfinished `Case` objects**. The package builder
must make the unchanged `torture/cases.py` importable, call `noise_cases()`, then
call `finish()` once per case. The existing engine supplies the per-case APU/DIV
reset, power-on, master gain, packet handling, and deterministic case entry.
This module does not reset the APU inside a case or change routing mid-case.

## Matrix and observations

For **each** NR43 value `$53` (15-bit width) and `$5B` (7-bit width):

| Treatment | Explicit wait arguments (T-cycles) | Count |
| --- | --- | ---: |
| DAC off, phase versus duration | P = 0, 384, 768, 1152; D = 0, 384, 1536; R = 0 | 12 |
| DAC off, resume delay | P = 0, 768; D = 1536; R = 384, 768, 1536 | 6 |
| DAC stays on, phase controls | P = 0, 384, 768, 1152; D = 1536; R = 0 | 4 |
| DAC stays on, duration controls | P = 0; D = 0, 384; R = 0 | 2 |
| DAC stays on, resume controls | P = 0; D = 1536; R = 384, 768, 1536 | 3 |

Mode names are `dac-off` and `expired-dac-on`; group is `ch4-dac-phase`.
All nine controls have an exactly timing-matched DAC-off case, but not every
DAC-off case has a separate control. Interpret each width independently.

The fixed prefix is the original expired-length setup: NR51 = `$88`, NR41 =
`$3F` (one count), NR42 = `$F0` (volume 15, envelope pace zero), NR43 = the chosen
polynomial, NR44 = `$C0`; wait 64 T and read NR52; wait 32768 T and read NR52.
The second read establishes length expiry before any variable wait begins.

Then wait P, write NR42 = `$00`, wait D, write NR42 = `$F0`, wait R, read NR52,
write NR44 = `$80`, wait 64 T, read NR52, and hold for 65536 T. Each DAC-on
control replaces **each** of the two NR42 writes with a separate 20 T wait;
there are no same-value live NR42 rewrites that could introduce zombie-envelope
behavior. NR51 remains `$88` throughout. Re-enabling the DAC must not itself
reactivate the expired channel.

Four expected NR52 **low nibbles** are `[8, 0, 0, 8]`: initially active, expired,
still inactive immediately before retrigger, active after retrigger. The raw
readback bytes retain their upper bits; mask with `$0F` when checking activity.
These validate the setup, not a preferred phase hypothesis. There is no universal
hardware audio PASS/FAIL threshold and no observation of hidden LFSR evolution:
the retrigger reinitializes that state. The observable is subsequent noise-edge
timing/shape, which can constrain surviving upstream phase.

## Timing contract

P, D, and R are `wait_cycles` arguments, **not edge-to-edge intervals**. Setup,
write, and read overhead stays fixed across the matrix, including both control
write replacements. At normal DMG speed:

- `write_reg` is `ld a, imm8` (8 T) plus `ldh [imm8], a` (12 T): **20 T**.
- `Case.read` is `ldh a, [imm8]` (12 T) plus `ld [imm16], a` (16 T): **28 T**.
- P begins after both instructions of the fixed post-expiry read.
- The two NR42 bus writes are **D + 20 T** apart; D = 0 is not a zero-length
  DAC-off interval.
- The enable and retrigger bus writes are **R + 48 T** apart because the
  intervening NR52 read/storage takes 28 T. R = 0 still leaves that overhead.
- The initial and final NR44 writes are **32976 + P + D + R T** apart.
- Before the caller's `finish()`, each case takes **98704 + P + D + R T** from
  its first instruction through the final hold. The final 65536 T hold is in
  addition to the post-retrigger 64 T wait and 28 T read/storage.

Those bus-write differences compare the same instruction/bus phase, not an
assumed instantaneous hardware response. Counter gating, synchronization, and
analog response can add their own latency. Generated assembly and actual CPU
traces remain the scheduling authority. The waits preserve registers/flags;
the control substitution changes no later instruction scheduling.

## Why the axes distinguish the basic hypotheses

The original `ch34-clock-probe/hardware-qw3uz2uv/README.md` reports repeatable
gap-dependent phase with the expired channel's DAC enabled, but nearly fixed
phase for nonzero DAC-off gaps. A fixed disable phase cannot distinguish a
retained frozen counter from a reset/rephased counter. P changes that disable
phase; D changes off duration independently; R tests whether phase resumes
after DAC enable, before a trigger.

For a simple selected-counter period S = 1536 T, let phi be phase at the final
trigger. Absorb fixed instruction delays and fixed startup phase into a constant
C per model/width. These are **hypothesis predictions**, not measurements or a
claim that every physical mechanism is a single counter:

| DAC-off hypothesis | Variable part of phi modulo S | Effect of varying P / D / R |
| --- | --- | --- |
| Retained counter keeps running through DAC off | P + D + R | yes / yes / yes |
| Retained counter freezes off, resumes on enable | P + R | yes / no / yes |
| Retained counter freezes off, resumes only on retrigger | P | yes / no / no |
| Disable resets/rephases counter, which then runs through off and on | D + R | no / yes / yes |
| Disable resets then freezes until enable, or enable resets then starts it | R | no / no / yes |
| Phase reset/held until retrigger | 0 | no / no / no |

An ordinary running DAC-on control predicts C_on + P + D + R modulo S.
For the DAC-off matrix, the three independent comparisons from the baseline
`(P,D,R) = (0,1536,0)` are:

1. `(384,1536,0)`: isolates a 384 T P change.
2. `(0,384,0)`: isolates a D change of -1152 T, equivalent to +384 modulo S.
3. `(0,1536,384)`: isolates a 384 T R change.

Thus the six rows above have distinct phase-difference triples even if their
absolute C values are unknown. The nine DAC-on controls include all these
comparisons. The rest of the grid checks recurrence, half-cycle offsets, and
P/R interaction rather than merely measuring a single offset. First-edge
**latency** is not phi itself: it commonly varies oppositely and wraps, and its
mapping can depend on the width/start sequence. Compare the early edge pattern
and timing across cases rather than fitting away each case's phase separately.

## Residual identifiability limits

- Reset at disable followed by a stopped clock, and the same reset at enable,
  are observationally equivalent if they leave the same phase when counting
  resumes. This matrix does not locate that reset edge in that situation.
- A reset, clamp, or rephase that produces the same phase at retrigger cannot
  be distinguished just by naming different internal mechanisms. A reset at
  retrigger can also erase all pre-trigger information.
- D = 0 and 1536, and R = 0 and 1536, alias under a strictly 1536 T periodic
  model. The 384/768 T points provide non-aliased discrimination. All variable
  waits are multiples of 384 T, so finer clock structure, longer-period state,
  and other models with identical sampled phases remain unresolved.
- Partial-divider resets, gating delays, or phase-dependent synchronization can
  produce surfaces outside this simple table. The table is not an exhaustive
  model selection algorithm. A D = 0 transient need not invalidate the settled
  nonzero-D surface.
- NR52 exposes channel activity, not divider state or LFSR contents. Retrigger
  removes direct evidence of any hidden pre-trigger LFSR progression.
- DAC disconnection/reconnection transients and capture filtering can shift an
  audio edge estimate. Timing-matched controls do not replicate the DAC analog
  transient; R also changes its settling time. Use multiple edges, capture
  timing calibration, and the two widths before assigning a digital mechanism.
- One DMG and two widths do not establish other hardware revisions. The prior
  repeat captures support the need for this experiment, not its future result.

Generation/static schedule checks do not establish actual NR52 values or audio
phase. Integrated CPU trace validation, ROM assembly, and physical recording
remain the package owner's checks.
