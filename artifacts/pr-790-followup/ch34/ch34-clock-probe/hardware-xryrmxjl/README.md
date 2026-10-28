# First physical CH3/CH4 recording: xryrmxjl

Follow-up: the second take reproduced these findings. See
[`../hardware-qw3uz2uv/README.md`](../hardware-qw3uz2uv/README.md) for the current
two-take result and updated reviewer wording. The first-take analysis below is
preserved as originally recorded.

Recorded 2026-10-28 on Jeff's DMG; physical CPU/board revision is not established.
This is one take, not yet an independently repeated measurement. No PR comment
or code change is implied by these results.

## Capture and transport

- Source: `/home/jeff/Documents/ChatGPT/dmg/artifacts/hardware/ch34-clocks-xryrmxjl/capture.wav`.
- WAV SHA-256: `ec4101753ee160bbe6ffa8f7068e0fd9e5144b4a67d283cacc9f9e63ab839ff0`.
- ROM SHA-256: `e68b3184162eb5f848e5428e12b2d89bc9139154bce776299976197e190e8882`.
- 47.978667 seconds, 48 kHz, stereo PCM24. The extra duration includes pre-roll/boot;
  all test packets fit in the recording, including the suite-end packet.
- All 170 expected packets recovered in order, with no missing or duplicate
  packets, independently from each recording channel.
- All 84 four-byte NR52 readbacks match the expected activity controls; there
  are 85 cases including the calibration case, which emits no readback.
- Peak levels: left -13.90 dBFS; right -14.69 dBFS. Neither channel has samples
  at or above 0.999 full scale. This establishes no digital full-scale clipping,
  not a general certification of the analog chain.
- Left polarity matches the emulator; right polarity is inverted. Each was
  calibrated separately, not summed. The observations agree across channels.
- The recorder stopped on the end packet. Its exit code 1 is the recorder's
  documented intentional-SIGINT behavior, not an incomplete-file indication.
- The decoder also rejected 60 non-packet preamble candidates on the left after
  clock correction. These diagnostics are retained in the full report; they are
  not missing expected packets or silently repaired readbacks.

Packet coverage is not an emulator-accuracy pass.

## Alignment and timing precautions

The existing analyzer fits one shared time mapping from acoustic packets and
their common preambles, then one gain from the separate CH2 calibration case.
It does not optimize tested waveforms separately by case or hypothesis.

For the left channel: hardware seconds = 14.0169690719692 +
1.0000585034320986 × reference-audio seconds. The measured relative timing drift
is +58.50 ppm; packet-anchor residual RMS is 9.52 microseconds. This is a
recorder/emulator relative ratio, not an independent DMG crystal calibration.

The emulator's PCM has a 64-tap band-limited-step delay relative to its CPU-write
log. Ignoring it mislabels absolute event times by about 0.65 ms. The CH3 plot
removes a single effective delay of 648.628 microseconds, measured from the
reference-only digital-mute endpoints in all 18 replacement-FF cases. The
individual measurements span 639.188–657.417 microseconds, within one native
48 kHz sample. No physical retrigger edge was used to fit this correction.
These measurements resolve millisecond behavior, not individual CPU cycles.

## CH3: a retained-sample discrepancy, not a simple running/frozen result

All previously triggered cases show a substantial nonzero held output at
retrigger. The never-triggered controls do not. Specifically:

| Treatment | Physical observation |
|---|---|
| Initial 00, replacement FF, length expired with DAC on | Output begins at retrigger for every tested gap, including zero additional gap; digital mute ends it around 1.98 ms. |
| Initial 00, replacement FF, DAC disabled after expiry | Also begins at retrigger, unlike both modeled alternatives. |
| Initial FF, replacement 00, length expired with DAC on | Nonzero output begins at retrigger and ends at the first fetch, about 0.98 ms later, even with the longest gap and repeated reads of zero-filled RAM. |
| Initial FF, replacement 00, DAC disabled after expiry | Same approximate 0.98-ms held-output interval. |
| Never triggered, replacement FF | No initial held-output pulse; output starts at the first fetch, measured 0.968–0.986 ms after retrigger, then ends at mute. |
| Never triggered, replacement 00 | No corresponding pulse. |

The scheduled first-fetch and mute times are about 0.978 and 1.980 ms. Measured
trigger-step times in the primary cases lie within about 13 microseconds of
zero after the reference-delay correction; a native sample is 20.83 microseconds.
Where a step is absent, the tiny largest-noise derivative inside its search
window is **not** an observed event. See `wave-measurements.json` for raw values.

The current SameBoy approximation replaces the held byte with addressed wave
RAM during inactive timer events. That predicts a delayed onset for the short
00→FF cases and no held-output pulse for the long FF→00 cases. Neither matches
this take. Freezing the inactive timer/buffer is not a solution either: it
predicts delayed onset for all 00→FF primary cases, contrary to the recording.
The DAC-off and never-triggered controls distinguish prior activity from simple
CPU writes to wave RAM, but DAC-off occurs only **after** the expiry interval.

This supports investigating how inactive wave-bus/sample-buffer state is
established. It does not yet isolate repeated inactive sample clocks from a
one-time expiry transition or a trigger-dependent mechanism. In particular,
the recording does not establish that inactive sample data is always FF; the
analog pulses are consistent with a high held sample, not a direct byte readout.
The two extreme RAM patterns and this single timing history are insufficient
to choose a replacement hardware model.

The hidden sample index still resets at trigger and is not directly observed.

## CH4: continuing phase after length expiry, different behavior with DAC off

The length-expired, DAC-enabled cases support continued upstream counting for
both tested LFSR widths. Increasing the silent gap by 768 T (183.1 microseconds)
moves the first post-trigger noise edge about 183.0 microseconds earlier. An
additional 1536 T (one selected-counter period) brings the restart phase back
around. The frozen-counter alternative instead predicts nearly constant delay.
Six gaps per width discriminate; gaps 1024 and 4096 do not because the two
modeled alternatives nearly coincide there.

This does **not** make CH4 an exact-model pass. Turning the DAC off produces
nearly constant restart latency across the nonzero gaps, unlike the reference.
At gap 768, hardware is about 231 microseconds later than the reference for
NR43=$53 and 223 microseconds later for $5B. Freezing the counter at length
expiry does not reproduce those DAC-off controls either. Stopping, resetting,
or rephasing on DAC transitions remain competing explanations.

The active-but-unrouted control reproduces the running-phase pattern, but a
small zero-gap timing anomaly remains. Do not interpret its approximately
18-microsecond discrepancy as a cycle-exact finding at 48 kHz.

The CH4 analysis uses a separate common calibration from 32 fixed, initial
DAC-enable edges before the variable gaps and tested retriggers. Packet-only
calibration independently preserves the conclusions. All 48 onset measurements
and the 32-anchor calibration were recomputed and reproduced during integration.
See `ch4-analysis/REPORT.md` and `ch4-analysis/results.json` for every case,
sensitivity checks, renderer latency, and non-statistical uncertainty allowances.

Retrigger clears the LFSR/feedback state. Reference and hidden-running candidate
audio are byte-identical, so this is **upstream divider evidence only**, not
direct measurement of inactive LFSR evolution.

## Recommended follow-up

First repeat this exact ROM from a cold power-on without changing gains. No new
flash is required. If the CH3 discrepancy repeats, build a focused probe that
varies the interval after expiry **before** replacing RAM, separates no RAM
access from explicit reads, and includes intermediate byte patterns. That can
separate a pulled-up inactive bus from RAM-value capture and one-time events.
For CH4, separately vary DAC-off duration and its phase relative to length
expiry to distinguish stop/reset/rephase behavior. Do not make a SameBoy core
change merely to fit this one take.

## Provisional reviewer reply — not posted

> I recorded a dedicated CH3/CH4 probe on my DMG. For CH4, the length-expired,
> DAC-enabled cases show gap-dependent restart phase consistent with the
> upstream counter continuing. Turning the DAC off behaves differently and
> exposes a mismatch with the current model. CH3 also shows a retained-sample
> mismatch, so I would not call either channel fully verified yet. I am checking
> repeatability before proposing more changes. These recordings cannot directly
> establish inactive CH3 index or CH4 LFSR progression because retrigger resets
> those states.

## Evidence

- `reference-comparison/metrics.json`: full left-channel decoder diagnostics,
  timing calibration, all per-case readbacks and whole-stimulus comparisons.
- `right-channel-check/metrics.json`: independent right-channel decode/calibration.
- `wave-measurements.json`: capture levels and CH3 event-window measurements.
- `wave-comparison.png`: eight representative CH3 primary/control overlays,
  including both RAM patterns and both modeled alternatives.
- `ch4-analysis/REPORT.md`, `results.json`, and `onsets.csv` within that directory:
  CH4 phase measurements, controls, calibration checks, and interpretation limits.
- `recording-metadata.json`: unchanged copy of the original recorder metadata.
- Analysis script: `../../../ch34-hardware-wave.py` relative to this directory
  (the source lives under the worktree's `.amp/in/` scratch area).
