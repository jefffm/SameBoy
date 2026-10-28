# Second physical recording confirms repeatability

The second take reproduces the CH3 retained-sample discrepancy and CH4 DAC-off
phase discrepancy seen in the first take. It also reproduces the positive
evidence for continued CH4 upstream counting after length expiry with DAC on.
This establishes repeatability on this DMG, not a complete hardware mechanism
or behavior across other units. No emulator-core changes were made from these
recordings, and no PR response was posted.

## Capture verification

- Second take: `ch34-clocks-qw3uz2uv`, 43.968 seconds, 48 kHz stereo PCM24.
- WAV SHA-256: `3f2a198aac29fd6bc6fd1e874e600c6a32d5f56083fbd85a9653be32bdc834de`.
- First-take comparison: `ch34-clocks-xryrmxjl`, SHA-256
  `ec4101753ee160bbe6ffa8f7068e0fd9e5144b4a67d283cacc9f9e63ab839ff0`.
- Same ROM: `e68b3184162eb5f848e5428e12b2d89bc9139154bce776299976197e190e8882`.
- All 170 packets decoded in order, with no missing or duplicate packets,
  independently on both recording channels. Payloads also match the first take.
- All 84 four-byte NR52 activity readbacks match the intended setup controls.
- Peak levels are -15.03 dBFS left and -15.88 dBFS right; neither channel has
  any sample at or above 0.999 full scale. No digital full-scale clipping.
- Left and right have opposite polarity; they were analyzed separately, not summed.
- The second recording ended on the suite-end packet. Its shorter duration
  does not indicate missing cases; pre-roll/start timing differs.
- Calibration-pulse gain differs by only -0.02294% on the left. This is a
  measured recording-level comparison, not independent knowledge of knob settings.

## CH3 repeats, including the controls

The same twelve cases show the conspicuous first-sample mismatch with the
reference: three short-gap 00→FF primary cases, all six 00→FF DAC-off cases,
and three long-gap FF→00 primary cases. These are not twelve newly chosen
pass/fail thresholds; they are the same missing/extra held-sample intervals
observed and described in the first take.

All 36 CH3 probe windows were compared directly between recordings using one
packet-derived time mapping and one calibration gain per recording, without
fitting individual tested waveforms. The largest calibrated take-to-take RMS
difference is 0.0008213; the large reference discrepancies are approximately
0.155–0.162 in those same amplitude units. Never-triggered controls reproduce
their delayed-onset/no-pulse behavior rather than the previously triggered
held-output behavior.

The observation is repeatable, but its cause is not isolated: repeated inactive
fetching, inactive-bus state, a one-time expiry effect, and trigger-dependent
behavior have not all been separated. Do not replace the current RAM-value
approximation with an unconditional FF constant on the basis of these two
extreme patterns.

## CH4 phase findings repeat

The original first-take edge measurement procedure and thresholds were reused.
Each take uses one calibration from 32 fixed initial DAC-enable events, before
the variable gaps and tested retriggers. The second take was also measured on
the opposite-polarity recording channel as a cross-check.

- All 48 CH4 cases retain the expected edge counts and ordered signs.
- The largest interpolated first-edge delay change between takes is 3.47 us;
  the largest left/right difference in the second take is 0.19 us. These are
  estimator differences, not guaranteed physical timing resolution: a native
  sample spans 20.83 us, and analog/systematic timing uncertainty still applies.
- With length expired and DAC enabled, restart latency varies with the gap
  and wraps around the selected-counter period in both noise widths.
- With the DAC disabled, nonzero-gap restart latency is nearly constant:
  the full spread is 4.42 us for NR43=$53 and 4.69 us for $5B.
- At gap 768 T, the DAC-off/reference disagreement repeats: hardware is
  230.53 us later for $53 and 222.92 us later for $5B.
- The active-muted controls and their small zero-gap anomaly also repeat.

The surviving upstream phase is observable. Continued hidden LFSR evolution is
not: retrigger resets that state, and the existing hidden-running candidate
produces audio identical to the reference.

## Recommendation

No third identical take is needed for this immediate repeatability check.
Prepare a focused follow-up ROM before choosing emulator fixes:

1. CH3: vary the post-expiry interval before rewriting wave RAM, separate no
   wave-RAM access from explicit reads, and use intermediate data patterns.
   This should distinguish RAM-value capture from a pulled-up inactive bus
   and help locate when the held value changes.
2. CH4: independently vary DAC-disable phase and off duration, with length
   expiry held fixed, to separate stopping, resetting, and rephasing.

Keep the already-understood PR review cleanup separate from these new behavior
investigations. Do not claim direct CH3-index/CH4-LFSR proof or multi-model scope.

## Updated reviewer wording — draft only, not posted

> I recorded the dedicated CH3/CH4 probe twice on my DMG, and the results repeat.
> CH4 restart phase after length expiry supports the upstream counter continuing
> while the DAC remains enabled. Turning the DAC off behaves differently and
> exposes a mismatch with the current model. CH3 also shows a repeatable
> retained-sample mismatch. These tests do not directly observe inactive CH3
> index or CH4 LFSR progression because retrigger resets those states. I would
> isolate the new discrepancies before proposing further behavioral changes.

## Evidence and reproduction

- `repeat-comparison.png`: two representative CH3 mismatches plus CH4 phase
  sweeps, overlaid for both physical takes. Repeatability is not emulator agreement.
- `repeat-comparison.json`: all 36 CH3 window comparisons and all 48 CH4 onset
  measurements on both channels of the second take, including calibration.
- `wave-measurements.json`: second-take capture levels and CH3 event measurements.
- `reference-comparison/metrics.json` and `right-channel-check/metrics.json`:
  full protocol diagnostics and independently calibrated waveform comparisons.
- `recording-metadata.json`: unchanged copy of the recorder metadata.
- `../hardware-xryrmxjl/README.md`: detailed first-take findings and limitations.
- Scratch analysis scripts under the worktree's `.amp/in/`:
  `ch34-hardware-wave.py --results <this directory>` and
  `ch34-repeat-analysis.py`. The repeat script imports the original CH4
  measurement functions without modifying their source or first-take results.

The original captures remain unchanged. The analysis did not write cartridge
contents, commit, push, or post to GitHub.
