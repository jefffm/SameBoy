# CH4 numerical verification — first physical DMG capture xryrmxjl

## Finding

**The length-expired, DAC-on treatment supports an advancing upstream counter,
not the supplied inactive-freeze candidate, for both NR43=$53 and $5B.** This is
a restart-phase result, **not evidence that the physical LFSR or feedback latch
continued evolving while inactive**. Reference and hidden-run WAVs are
byte-identical; all three marker and APU logs are byte-identical.

There is also a **real DAC-off control discrepancy**: hardware restart latency
is essentially independent of nonzero gap, whereas the reference keeps its
running-counter sawtooth. The supplied freeze candidate is not the right
DAC-off prediction either. Do not call CH4, or the complete suite, an exact pass.

## Measurement and independent calibration

- Only CH4 cases 38–85, hardware channel 0, were analyzed. Packet preambles are
  used solely as independent calibration signals; packet integrity and capture
  quality were not re-audited here.
- Observable: first isolated **negative noise step** after retrigger. This is
  not the DAC-on/routing transient or an arbitrary RMS onset. Times are the
  interpolated midpoint between locally extrapolated pre/post step levels.
- No per-case reference shift, tested-edge alignment, gain fit, or template
  phase fit was performed. All 48 cases use one common affine time mapping.
- Primary calibration: the **32 initial NR42=$F0 DAC-on transitions before the
  initial trigger**, in the expired/DAC-off groups. These fixed source-scheduled
  edges precede both the variable gap and the tested retrigger. Active-muted
  initial DAC-on is inaudible and is not used as an anchor.
- Hardware audio time versus CPU trace time:
  `t_hw = 14.01760665687806 + 1.0000594335283093 * t_log`.
  Anchor residual RMS **5.959 T**, maximum **15.566 T**. Formal OLS slope SE is
  0.05483 ppm, but this does not include analog/systematic uncertainties.
- Reference-only effective write-to-PCM latency from the corresponding 32
  transitions: **658.685998 us = 31.616928 samples = 2762.729 T**, SD 14.568 T.
  This includes renderer delay and instruction-boundary/write phase. It is not
  a claim that the renderer kernel alone has precisely that delay.
- Thus all delays below have the approximately 0.65 ms rendering latency
  removed. APU log timestamps are GB_run boundaries, not exact bus-write times.
- Final NR52-off edges were investigated but **not used** for calibration:
  adjacent noise transitions and the emulator's abrupt highpass-state clearing
  contaminate several apparent endpoints.

### Packet-only cross-check and polarity

A multistart common-preamble search reproduces the existing global minimum:
normalized residual objective **0.01927799**, positive fitted gain
**0.06293989**. Competing approximately ±490 us phase minima score **0.33559**
and **0.33608**; the best inspected inverted-polarity minimum scores **0.86242**.
There is no competitive polarity or whole-subcycle alternative in this search.

Using only the 96 common preambles adjoining the CH4 cases, with no stimulus
samples, gives:

`t_hw = 14.016952675833222 + 1.0000591125251108 * t_reference_PCM`.

Its objective is **0.00818403** versus **0.01986142** for the original global
mapping on those same preambles; positive gain is **0.06336422**. This is a
small local calibration difference, not a different phase branch. Substituting
this independent packet calibration changes the reported hardware onset by at
most **24.40 T**; retaining the original global calibration changes it by at
most **39.68 T**. Conclusions do not change. Gains are diagnostic here; onset
measurements do not require amplitude calibration.

## Length-expired, DAC-on results

First negative noise-edge delay after retrigger, in CPU T cycles. One T is
0.238418579 us. Figures below are measured estimates, **not cycle-exact values**.
Reference/freeze columns come from their supplied audio, not a hand-written
counter simulator. Exact native hardware sample coordinates and times are in
`onsets.csv` and `results.json`.

| Gap T | $53 hardware | $53 reference | $53 freeze | $5B hardware | $5B reference | $5B freeze |
|---:|---:|---:|---:|---:|---:|---:|
| 0 | 23119.3 | 23073.4 | 23638.8 | 10827.6 | 10800.9 | 11368.8 |
| 256 | 22856.3 | 22853.7 | 23644.2 | 10563.9 | 10578.7 | 11370.0 |
| 512 | 22609.9 | 22601.8 | 23655.8 | 10318.3 | 10297.8 | 11370.2 |
| 768 | 22351.8 | 22336.5 | 23658.7 | 10060.0 | 10033.2 | 11352.0 |
| 1024 | 23613.0 | 23623.4 | 23640.3 | 11320.2 | 11343.4 | 11357.8 |
| 1280 | 23373.7 | 23370.2 | 23649.7 | 11081.3 | 11065.2 | 11369.1 |
| 1536 | 23122.1 | 23089.4 | 23658.4 | 10828.8 | 10808.5 | 11352.0 |
| 4096 | 23625.8 | 23619.2 | 23641.1 | 11313.3 | 11339.2 | 11354.2 |

The direct phase measurement `(hardware_delay + gap) mod 1536` is:

- **$53: 75.974 T mean, 6.555 T SD**, range 60.954–82.072 T.
- **$5B: 69.184 T mean, 10.064 T SD**, range 49.330–78.338 T.

That is the running-counter signature: adding 256 T to the silent interval
advances the retained phase by approximately 256 T, with a 1536-T wrap. Gaps
0 and 1536 repeat within **2.81 T ($53)** and **1.20 T ($5B)**. The frozen
candidate instead has almost constant latency for every gap. These relative
findings survive any common offset or renderer-delay ambiguity.

Hardware minus reference first-onset RMS: **21.35 T ($53)** and **22.22 T ($5B)**.
All of the first three edge differences in the 16 primary cases are within
**53.01 T**. The first three signs are negative, positive, negative. Their
spacing also identifies the edge sequence: approximately 14 then 1 selected
periods for $53; 6 then 1 for $5B. Within the retained measurement window there
are 3 $53 edges and 13 $5B edges per case, with matching ordered sign sequences
across hardware, reference, and freeze. Full raw edge coordinates are saved.

**Discriminating gaps:** 0, 256, 512, 768, 1280, 1536, in both widths: 12 cases.
The smallest hardware/freeze onset separation among those is **276.08 T**;
the largest is **1306.94 T**. **Gaps 1024 and 4096 are counterexamples to treating
every case as discriminating:** both models predict almost the same onset
there. They cannot independently select either hypothesis.

## Controls and counterexamples

Measured hardware first-edge delays, using exactly the same common calibration:

| Gap T | $53 active-muted | $53 DAC-off | $5B active-muted | $5B DAC-off |
|---:|---:|---:|---:|---:|
| 0 | 23175.5 | 23219.6 | 10878.8 | 10928.3 |
| 256 | 22856.7 | 23297.7 | 10541.9 | 11004.0 |
| 512 | 22615.1 | 23299.2 | 10320.1 | 11001.7 |
| 768 | 22351.6 | 23297.9 | 10037.2 | 10984.0 |
| 1024 | 23618.3 | 23280.4 | 11325.7 | 10986.7 |
| 1280 | 23379.4 | 23288.7 | 11086.7 | 10997.0 |
| 1536 | 23101.8 | 23297.5 | 10803.8 | 10998.8 |
| 4096 | 23607.7 | 23297.5 | 11316.5 | 10982.5 |

**Active-muted** reproduces the running-phase pattern. Reference and freeze
coincide in this active control. Hardware/reference onset RMS is 28.20 T ($53)
and 34.78 T ($5B). Preserve the small zero-gap anomaly: gap 0 is **73.68 T** and
**74.95 T** later than the corresponding gap-1536 repeat. Its first-onset error
against reference is +68.99/+78.69 T; the following two edges retain a similar
positive displacement. This is not enough for a cycle-exact mechanism claim at
48 kHz, but should not be silently fitted away.

**DAC-off** does not reproduce the reference sawtooth. For gaps 256–4096,
hardware delay is **23294.130 T, SD 6.477 T ($53)** and **10993.528 T, SD 8.254 T
($5B)**. Gap 0 is approximately 70–78 T earlier than the gap-1536 repeat.

The strongest specific counterexample is DAC-off gap 768: hardware is
**+967.28 T ($53)** / **+934.18 T ($5B)** later than reference. The reference
has retained the running phase, while hardware is near the same latency as
the other nonzero DAC-off gaps. Conversely, the freeze-at-inactivity candidate
is consistently **342–426 T later than hardware** across these DAC-off cases.
Thus neither supplied audio model completely describes this control. The
result is compatible with a DAC-off stop/reset/rephase mechanism but does not
distinguish those mechanisms. DAC-off must not be equated to length-expired
DAC-on.

## Uncertainty, checks, and limits

- Native sample duration: **87.381 T = 20.833 us**. Subsample interpolation
  improves repeatability, not the guaranteed physical bandwidth/resolution.
- Changing local baseline flank length from 2 through 7 samples, including
  recomputing the independent calibration each time, changes hardware onset
  by at most **39.26 T** from the reported setting; full variant range is
  **45.06 T**. First-edge-only baseline sensitivity is at most 30.28 T hardware
  and 21.69 T reference.
- **624 threshold checks** (five hardware and four per-reference-model
  thresholds over all 48 cases) selected the same first derivative peak.
- Separate $53/$5B fixed-anchor clock estimates are 1.00005947177 and
  1.00005928073, with approximately 5.9-T residual RMS each.
- Conservative interpretation allowances: **±128 T hardware onset**, **±48 T
  reference/freeze prediction**. These are engineering sensitivity allowances,
  **not 95% confidence intervals or proven hard bounds**. NR42 DAC enable and a
  noise step might have different analog group delays; unknown common latency
  remains a caveat. The large gap-dependent sawtooth versus flat prediction is
  stronger evidence than absolute CPU-cycle latency.
- All source identity assertions, 48-case/32-anchor checks, edge-count/sign
  checks, and all preamble optimizations completed successfully. SHA-256s are
  in `results.json`; hardware SHA-256 matches the supplied metrics.
- One DMG/capture is not a revision-population claim. Retrigger resets the
  downstream state: advancing the hidden LFSR is observationally unidentifiable
  here. Only retained upstream phase is supported.

## Files and reproduction

All newly created evidence is confined to this `ch4-analysis/` directory:

- `results.json`: conclusion, calibration minima, uncertainties, all 48 onset/
  phase measurements, input hashes, and counterexamples.
- `onsets.csv`: the 48-case numerical table, including native hardware seconds
  and fractional sample coordinates, model residuals, and alternate mappings.
- `measurements.json`: measured signed edge coordinates and initial-DAC anchors.
- `analyze_ch4.py`: reproducible primary measurement and calibration code.
- `REPORT.md`: this report.

From the worktree root:

```sh
PYTHONDONTWRITEBYTECODE=1 OPENBLAS_NUM_THREADS=1 \
  /home/jeff/Documents/ChatGPT/dmg/.venv/bin/python \
  .amp/in/artifacts/ch34-clock-probe/hardware-xryrmxjl/ch4-analysis/analyze_ch4.py
```

No original WAV, kit source, Core file, PR, or pre-existing report was changed.
