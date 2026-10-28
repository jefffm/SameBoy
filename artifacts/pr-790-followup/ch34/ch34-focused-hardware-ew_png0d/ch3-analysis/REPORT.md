# CH3: the held output changes after expiry without a wave-RAM rewrite

The focused recording separates a delayed change from an immediate expiry
effect: short waits retain the initial sample, while longer waits produce an
output equivalent to nibble F even when wave RAM is untouched. Disabling the
DAC promptly preserves the initial sample. Neither the reference's inactive
RAM-value refill nor freezing that whole path explains all the observations.

This is an audio observation after retrigger, not a direct read of the internal
sample buffer or evidence that the hidden waveform index continued advancing.
It is consistent with clocked capture of a pulled-up value, but does not yet
establish the exact bus mechanism, the low nibble of any internal byte, or an
unconditional FF rule for every inactive CPU/bus phase.

## Recording and setup

- Capture: `ch34-clocks-ew_png0d`, 54.976 seconds, stereo 48 kHz PCM24.
- WAV SHA-256:
  `21cbdadda811688f8c432b58358e9e4eeea1d3ac595f6f266edcb9ae7e768c68`.
- ROM SHA-256:
  `35e40243730d7d686721c987ef40ceb6b5dedb31a6051c0c6e4a586a75c498ca`.
- All 234 expected packets are present in order on each channel, with no
  missing or duplicate valid packets and the suite-end packet received.
- All 62 CH3 cases have their intended four NR52 activity values on both
  channels. Activity readbacks establish setup, not oscillator behavior.
- Digital peaks: -14.773 dBFS left, -15.623 dBFS right; no full-scale clipping.
  Channels have opposite polarities and were analyzed separately, not summed.
- The recorder metadata names the preceding probe. Decoding with the focused
  manifest identifies the actual complete focused run; no re-recording is
  required to repair that metadata mismatch. The original metadata is retained.

## Untouched wave RAM separates short and long waits

Left-channel held-output equivalents, inferred from the transition to the
known first freshly fetched sample:

| Initial RAM byte | Wait 0 T | 256 T | 1024 T | 4096 T | 16384 T |
| --- | ---: | ---: | ---: | ---: | ---: |
| 00 | 0.008 | 0.049 | 0.006 | 14.930 | 14.911 |
| 55 | 4.966 | 5.031 | 5.027 | 15.338 | 14.935 |
| AA | 10.043 | 9.946 | 9.982 | 14.745 | 15.063 |
| FF | 15.024 | 15.018 | 15.051 | 14.950 | 15.019 |

The decimal precision records the estimator's output, not physical measurement
accuracy. These cluster around digital sample values 0, 5, 10, and 15.

Waits are explicit waits after observed expiry, not exact times since hardware
expiry. The successful NR52 read precedes retrigger by another 104 T; polling
adds up to 56 T. The 1024/4096 comparison bounds a change for this acquisition
phase only, not a universal expiry-to-change delay. The acquisition frequency
register is 1, corresponding to a 4094-T sample period.

## RAM/bus changes do not select the held value in this schedule

All 24 treatments start from 55, rewrite to 00/55/AA/FF, and then perform idle,
wave-RAM-read, or WRAM-read exposure. All produce approximately 15, regardless
of replacement data or read address class. Left-channel estimates range from
14.835 to 15.464; right-channel estimates range from 14.790 to 15.477.

The two timing-matched no-rewrite controls also produce approximately 15:
15.026 and 14.917 on the left. Thus a rewrite is not necessary for the change.
The exposure is 8192 T after setup; this is not a survey of all possible CPU
access phases or instruction sequences.

## Controls preserve the distinctions

- Eight never-triggered controls produce approximately zero held output,
  despite rewriting and optionally reading wave RAM. Left estimates range
  from -0.541 to 0.163; right from -0.603 to 0.136. Slight negative estimates
  are analog/baseline error, not negative digital samples.
- Eight DAC-history controls disable the DAC while active or promptly after
  observed length expiry, then wait 8192 T. They retain the corresponding
  initial nibble 0/5/10/15 rather than converging to 15.
- The reference differs from hardware by more than two equivalent sample
  units in 26 of 62 cases; the frozen-inactive-buffer candidate does so in 32
  cases, on each channel. This is a descriptive comparison, not a universal
  pass/fail threshold. The large differences are roughly 5, 10, or 15 units.
- The separate hidden-running candidate has byte-identical audio and APU
  traces to the reference. These tests cannot distinguish hidden index
  progression when retrigger resets the index before output resumes.

## Measurement and uncertainty

`analyze_wave.py` uses one packet-derived affine clock/offset and calibration
gain per recording channel, with no per-case tested-waveform alignment. The
renderer delay is estimated from reference-only NR32 mute edges: median
649.055 microseconds; edge estimates span 637.937 to 657.977 microseconds.

The first new fetch is observed about 0.978 ms after retrigger. Local linear
baselines before and after that event estimate its amplitude step. Adding the
step, expressed in sample units, to the known new nibble estimates the held
output. One amplitude scale per channel comes from 47 nonzero final-sample to
digital-zero mute endpoints, not a fit to the held-sample hypotheses.

- Calibration residual RMS: 0.218/0.219 sample units, left/right.
- Largest channel-to-channel difference: 0.117 sample units.
- Largest spread from three baseline windows: 1.063/1.045 sample units.
- Largest spread from moving the step center by plus/minus one native sample:
  1.586/1.529 units. A native sample is 20.833 microseconds, or 87.381 T.

These are sensitivity checks, not statistical confidence intervals or complete
systematic-error bounds. Treat roughly one-to-two-unit deviations cautiously;
the multi-unit changes are the useful result. Analog filtering and ringing
remain limitations. In particular, the trigger-step estimate is not an
independent confirmation for every case: nearby expiry edges contaminate
short-gap cases, and DAC enable/disable transients confound DAC-history cases.
The first-fetch estimates are the principal discriminator.

## Isolated clocked-FF hypothesis

An experimental copy of `apu.c`, outside Core, changes the two inactive-DMG
RAM refill paths to FF without changing timer progression or the ordinary
active fetch. It does not force FF at expiry or retrigger. This tests one
specific hypothesis, not a universal SRAM rule or a final PR patch.

Against all 62 focused cases, its held-output-equivalent RMS error is
0.142/0.141 sample units, left/right, versus 6.983/6.973 for the reference.
The largest candidate residual is 0.529 units across either channel; no
candidate residual exceeds two units. Short-gap retention, DAC-history, and
never-triggered controls remain consistent with the measurements.

The same candidate was then run against the previous ROM and compared with
both earlier physical takes, using their existing time/gain calibration and
all 36 CH3 windows, without refitting. Maximum per-case waveform RMSE falls
from 0.160873 to 0.013510 in the first take and from 0.161182 to 0.013339 in the
second. Remaining analog waveform discrepancies are not declared solved.

Both ROM runs preserve byte-identical APU-write and marker logs. Re-linking
the unchanged capture driver against the current review-cleanup library also
reproduces the frozen focused reference audio and logs byte-for-byte. Thus
the discrepancy is not just the reference predating review cleanup.

The candidate is useful evidence for a timer-clocked inactive value change.
It still cannot establish the unobserved low nibble, every CPU read phase,
other frequencies, other models, or other silicon instances. Its source,
patch, renderer, and earlier-take comparison are retained under
`../candidate-pulled-up/`; no production Core file was changed.

## Implication for the PR

Report that CH3 retains history after length expiry and that its observable
held output changes with the inactive interval while DAC remains on. Do not
say the recording directly proves hidden index advancement or an FF byte in
the buffer. The timer-clocked pulled-up-value candidate now fits these tested
observables, but broader bus/model scope needs validation before a general
Core fix.

This recording is from one DMG, not independent evidence for other units or
MGB/SGB/CGB behavior. No Core changes, PR comments, commits, pushes, or further
cartridge operations were made by this analysis.

## Reproduce

Run the analysis using the existing lab Python environment:

```sh
/home/jeff/Documents/ChatGPT/dmg/.venv/bin/python \
  /home/jeff/Work/SameBoy-review/.amp/in/artifacts/ch34-focused-hardware-ew_png0d/ch3-analysis/analyze_wave.py
```

The script reads the original WAV, frozen focused kit, and rendered scratch
candidate. Outputs are `results.json`, `held-output.csv`, and the inspected
`waveforms.png`. The plotted alternatives are the reference and frozen path;
the additional FF candidate measurements are in JSON/CSV.
