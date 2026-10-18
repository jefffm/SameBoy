# Connected-DAC restart: inconsistent synthesis latency

The instrumented frontend links a snapshot of the unmodified f51cb47b static
library. Full-suite audio, APU writes, suite markers and final screen are
byte-identical to the preserved render. Only cases178/180/182/184 emit additional
native callback state; no checkout/build changes were made.

Each case first plays its tested channel, then drives digital zero, disables its
DAC, and later enables and retriggers it. Another inactive, unrouted DAC stays
enabled through the off/reconnect interval. Thus the coupling capacitor remains
connected. Pulse/noise digital-zero phases use NRx2=$08 with a trigger; wave uses
NR32=$00. The final startup is multiple register writes, not only a DAC enable.

At all four reconnects the tested channel's band-limited input is120 and its
band-limited accumulated output is120*65536. These values do not change while
its DAC fade rises from0 to5/12 to5/6 to1. CH1's first output samples are1200,
3476,3823: raw1440,3720,4080 minus capacitor integer240,244,257 respectively.
Those are a real source-voltage step against a partly discharged capacitor,
not the all-DAC-off negative pulse already fixed. Their initial transition does
not arise from a new oscillator value or synthesis-kernel update.

`render()` multiplies the already band-limited channel by the newly updated
`dac_discharge` immediately. In contrast, digital DAC values and NR51/NR50
routing/volume changes enter `band_limited_update()`, whose64-tap kernel delays
the bulk of a step by about32 samples. Packet-based hardware alignment uses
these band-limited tones, so the direct enable multiplier acts on a different
output-time axis.

The minimal `equivalent-step.c` demonstrates this without capacitor history,
oscillator phase or physical high-pass modeling: high-pass OFF, CH2 DAC enabled
and unrouted, CH1 inactive digital-zero source, NR50=$77. Both stimuli change
the terminal level from0 to4080PCM:

| Path | First-difference centroid, samples after write |
|---|---:|
| Route an already enabled DAC with NR51 |31.524019607843137|
| Enable an already routed DAC with NR12 |0.7352941176470589|

Difference:30.78872549019608 samples, or0.6414317810457516ms at48kHz. The centroid
uses the signed first differences of the complete settled response, without a
threshold or timing fit. The small distinction from exactly32 samples includes
the existing50us DAC attack and the kernel's phase/centering.

A timing-only candidate should shift DAC gain/connection history by the kernel's
common delay while preserving the gain shape and capacitor logic. Prediction:
the connected positive edges move later by32 samples; their voltage step must
remain. Blanking these onsets or resetting the capacitor would conceal a real
source change and fail the existing changed-source reconnect regression.

The minimal production seam is the shared audio synthesis timeline, rather than
register timing or per-case offsets. Delaying render-control history provides a
small consistency correction if the frozen hardware comparisons confirm it.
Moving DAC gating/fades before the band-limited synthesis would also filter their
shape, but is a broader change. This investigation establishes the latency
mismatch in emulator code, not the physical DAC fade shape or exact hardware
latency. Hardware evidence must distinguish those separately.
