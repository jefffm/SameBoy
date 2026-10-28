# CH4 focused hardware result — ew_png0d

## Finding

**The DAC-off surface retains P dependence but has no resolved D or R dependence.** Of the six specified hypotheses, it strongly supports **retained phase, stopped by DAC disable and not resumed merely by DAC enable; resumption at retrigger** (`phi = C + P mod 1536`). This includes the shortest off interval, D=0, which is actually **20 T between NR42 writes**. Timing-matched DAC-on controls instead follow `C_on + P + D + R`.

This is a CH4 result for **all 54 cases, IDs 64–117**, not a suite pass or a direct observation of a physical counter. The reference renderer follows `P+D+R` even for DAC-off cases and therefore does **not** reproduce the physical DAC-off phase surface. The recording's stale manifest label was not used; the new kit manifest, its CPU/APU schedule, and the parent's new-manifest decoding were used.

## Measurement and independent calibration

- Both 48 kHz hardware channels and the reference were measured. One hardware sample is **87.38 T**. Right has inverted polarity; timing extraction accommodates either sign, and pattern checks normalize that inversion.
- Each hardware channel has **one affine mapping for the entire CH4 capture**, fitted to its 54 **initial DAC-on steps before P/D/R**. No tested noise edge enters timing calibration; there is no per-case alignment, time warp, or phase shift.
- Mapping is `hardware_seconds = offset + clock * CPU_log_seconds`:

| Channel | Offset (s) | Clock | Anchor RMS / max absolute residual (T) |
|---|---:|---:|---:|
| Left | 9.394105805398 | 1.000059306058 | 7.317 / 11.327 |
| Right | 9.394105707616 | 1.000059306824 | 7.011 / 11.763 |

- Reference-only initial DAC edges give a **constant renderer delay of 647.647821 µs** (31.087095 samples; 2716.431845 T). Across anchors its SD is 23.639 T. This delay was not estimated from noise.
- Steps use interpolated 50% crossings between locally extrapolated linear baselines (3 samples each). Hardware derivative threshold is 0.003 full-scale, reference 0.04. Peak detection is padded before window inclusion to avoid losing a peak at a truncated derivative endpoint.
- Noise window is retrigger +1 ms through poweroff −0.2 ms, i.e. **4194.304–64869.1392 T** after retrigger. All first three noise edges are well inside it. The last 0.2 ms is deliberately censored to avoid poweroff contamination.
- Main phase statistic is the mean phase of the first three edges, expressed on the first-edge latency branch: `L1 + mean(wrap1536(Li−L1))`. Each edge also has a separate fit. This aggregates measurements; it does not align waveforms.
- Model residual is `wrap1536(L + aP + bD + cR − C)`. Only **one shared C** is fitted per model/width/mode/channel; coefficients, period, and case timing are fixed. Latency changes have the opposite sign from phase changes. Controls have their own C_on as specified in the design.

## Numerical discrimination

RMS circular residuals in T, **18 DAC-off cases per width**, including D=0; entries are **left / right**:

| Variable part of phase | Hypothesis | $53 | $5B | Verdict on this matrix |
|---|---|---:|---:|---|
| P+D+R | Retained counter continues throughout | 265.14 / 265.13 | 262.44 / 262.55 | Rejected by D and R |
| P+R | Frozen off, resumes on enable | 257.43 / 257.39 | 255.34 / 255.19 | Rejected by R |
| **P** | **Frozen off, resumes only on retrigger** | **3.808 / 3.780** | **3.174 / 3.028** | **Supported** |
| D+R | Disable resets phase, then runs | 427.90 / 427.87 | 427.29 / 427.46 | Rejected, especially by P |
| R | Disable resets/freezes until enable, or enable resets/starts | 427.82 / 427.82 | 428.54 / 428.63 | Both rejected as complete models; the two mechanisms remain equivalent to each other |
| 0 | Reset/held until retrigger | 423.60 / 423.64 | 424.16 / 423.94 | Rejected by P |

The **nine DAC-on controls per width** fit P+D+R at **3.062 / 3.037 T RMS ($53)** and **2.736 / 2.812 T ($5B)**. The next-best control fit is ≥119.44 T. All 18 matched off/on pairs and every individual model residual are retained in `measurements.json`.

The design's three independent comparisons, relative to `(P,D,R)=(0,1536,0)`, give these **latency differences in T, left / right**:

| Width / treatment | P: target (384,1536,0) | D: target (0,384,0) | R: target (0,1536,384) |
|---|---:|---:|---:|
| $53 DAC off | −374.76 / −374.63 | +9.68 / +9.75 | +9.29 / +8.88 |
| $53 DAC on | −385.46 / −385.96 | −381.58 / −382.34 | −384.69 / −384.88 |
| $5B DAC off | −383.41 / −383.94 | +8.50 / +7.63 | −0.004 / −0.257 |
| $5B DAC on | −376.51 / −376.23 | −381.30 / −380.84 | −384.89 / −384.66 |

Here D changes by −1152 T, which aliases to +384 T modulo 1536. Thus a running-counter prediction is approximately −384 T for all three columns; the physical off result is instead approximately **(−384, 0, 0)**. R=768 gives off differences **+11.62 / +11.59 T ($53)** and **−2.05 / −2.83 T ($5B)**, versus on **−772.96 / −773.32** and **−765.36 / −765.21 T**. The nonaliased R points, not R=1536 alone, reject resumption at enable.

Across both widths, all one-axis off comparisons have maximum absolute disagreement with P-only of **10.50 T (P), 9.98 T (D), and 11.62 T (R)** on left; **9.92, 9.75, 11.59 T** on right. D=0-only and D>0-only fits are retained: both are consistent with the same P-only surface; no special transient subset was discarded.

## Residuals, counts, and sensitivity

- Worst primary off-fit residuals: **case 81, −7.681 T** ($53 left; right −7.586), and **case 92, +7.648 T** ($5B left; right +6.875). Case 66 produces several ≈+9–12 T contrasts because it lies low, not because those contrasts were silently rounded to zero. No outlier was removed.
- Each of the first three edges independently selects the same winning hypothesis; winning individual-edge RMS is **6.24–8.20 T** across hardware widths/treatments. First-three intra-case phase spread is at most **18.928 T left / 18.526 T right**. Consecutive-edge intervals are integer multiples of 1536 with **11.726 / 11.727 T RMS** residual (maximum absolute **19.722 / 19.411 T**).
- Observed counts, not assumed equal: $53 has **3 edges in all 27 cases** in each stream. $5B hardware has **15 edges in 23 cases, 14 in 4** on each channel; reference has **15 in 22, 14 in 5**. The count differences are cases **98, 101, 106**, where the final transition falls on different sides of the analysis window. Extending the tail window to poweroff −0.12 ms observes **15 edges in every $5B case in all three streams**, with no change to first-three measurements. Common-prefix edge signs and rounded interval sequences match the reference in **all 54 cases on both channels**. Counts describe the censored window, not the complete hold.
- All **54 decoded CH4 readbacks on each hardware channel** have low nibbles `[8,0,0,8]`: active initially, expired, still inactive after enable/before retrigger, active after retrigger. This validates activity setup, not divider state. Decoding is inherited from the parent, not repeated here.
- Left−right first-three phase-latency difference: mean **−0.448 T**, RMS **0.797 T**, worst **−4.962 T (case 98)**. These channels share one physical experiment, so are not independent repeat captures.
- `sensitivity.json` records **14 complete reruns**. Derivative thresholds 0.002/0.004/0.005 leave the first-three measurements unchanged. Flanks 2/4/5, constant versus linear baselines, and 40%/60% crossings all preserve the winner. Across all variants, P-only off-fit RMS is **1.638–12.803 T**, while its closest competitor is **≥253.886 T**. The worst latency change is **15.428 T**, using the short two-sample baseline.
- Recalibrating from controls only or alternating anchors changes first-three latencies by at most **5.955 T**. The existing single global packet mapping, corrected only by the reference-derived renderer delay, changes them by at most **6.233 T** and preserves the same winner. These are alternate global maps, never per-case fits.

## What is and is not identified

The former fixed-disable-phase ambiguity is resolved at the sampled 384 T scale: **P information survives the DAC interruption**, inconsistent with a *phase-erasing* reset at disable, enable, or retrigger. R independence says enabling the DAC does not by itself restore the observed phase progression while this expired channel remains inactive.

This does **not** uniquely locate a stopped physical divider or prove that no component resets. A stopped selected counter, stored/restored upstream phase, or partial-divider reset that preserves the same sampled P information can be observationally equivalent. In particular, the design's “reset at disable then freeze” versus “reset at enable” pair remains indistinguishable internally, but its common R-only surface is rejected here. Do not conflate that rejected pair with the supported retained-P surface.

The supported off-model shared constants in the **latency** residual formula exceed the on-model constants by **79.839 T ($53 left)** and **78.720 T ($5B left)**. The literal disable-to-retrigger interval at D=R=0 is 68 T; the remaining ≈11 T is not resolved into digital gating, write synchronization, or analog/estimator delay. Shared offsets were allowed rather than hiding that mismatch with a per-case adjustment.

No hidden-LFSR progression claim follows: retrigger resets LFSR state. This is one DMG, two widths, one physical capture, finite analog bandwidth, and a coarse P/D/R lattice; SDs and sensitivity spreads are not population confidence bounds. D=0/1536 and R=0/1536 alias. The six proposed sampled surfaces are distinguished without another capture. **Only if sub-divider reset/gating semantics are needed**, a genuinely new discriminator is a dense **4 T-stepped P sweep** with nonzero fixed D, supplemented by short 4 T-stepped D/R points: the current 384 T grid cannot reveal lower-stage phase loss or a short enable/disable synchronization boundary. Even that cannot distinguish mechanisms deliberately preserving exactly the same observable phase.

## Reproduction and files

Run from any directory:

```sh
/home/jeff/Documents/ChatGPT/dmg/.venv/bin/python -B \
  /home/jeff/Work/SameBoy-review/.amp/in/artifacts/ch34-focused-hardware-ew_png0d/ch4-analysis/analyze_ch4.py
```

- `analyze_ch4.py`: standalone measurement, schedule checks, model fits, and sensitivity reruns; old helper mathematics adapted without importing old globals.
- `measurements.json`: 54 cases, all raw edge samples/times/steps, independent anchors/maps, individual and combined fits, all axis contrasts, 18 control pairs, input SHA-256 hashes.
- `case-measurements.csv`: **162 rows** (54 cases × reference/left/right).
- `edges.csv`: **1445 edge rows** across those streams, including later edges.
- `sensitivity.json`: all calibration variants, per-case latency/count results, and complete shared-offset model residuals.

Only this analysis directory was written. No Core/ROM/input-artifact/WAV changes, flash, post, or commit.
