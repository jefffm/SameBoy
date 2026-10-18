# First DAC reconnect: causal instrumentation

This is an instrumented build of the unchanged `79fb345c` core, linked to a
snapshot of its existing static library. Only the frontend sample callback is
modified. It records native output samples and current internal state. No new
hardware evidence is created.

The first noise DAC reconnect writes NR42=$38 at 3.141431808 seconds. Before this
write the retained noise output is exactly 105*65536; its full-scale output is
105*34 = 3570. The held high-pass capacitor is 3569.9674565658829.

- First enabled render: the attack fraction is 5/12. Cubic smoothing then integer
  multiplier truncation produces multiplier12, giving1260. Subtracting the old
  capacitor integer3569 produces the observed sample−2309.
- Next render: attack fraction5/6 yields multiplier31 and raw3255. The capacitor
  has been pulled down to3561.5051991406212, producing sample−306.
- Third render: full multiplier34 returns raw3570. The capacitor has fallen again
  to3560.382358258194, so a false positive residual10 remains after the spike.

Throughout the reconnect, trigger, later routing write, and following note write,
the band-limited input/output and digital sample remain unchanged. No new BLEP,
LFSR edge, routing change, or source-voltage change accounts for this negative
pulse. The DAC attack ramp against the held coupling capacitor accounts for it
exactly. Bypassing that ramp at reconnection would initially yield +1 PCM count,
without disturbing the capacitor by the false ramp. This is a candidate mechanism,
not by itself proof of correct hardware behavior.

A targeted regression should compare full raw source minus held capacitor, rather
than asserting arbitrary silence. Changing mixer gain while disconnected and then
reconnecting must preserve the legitimate new-voltage minus old-capacitor step.
