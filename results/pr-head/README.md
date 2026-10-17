# PR-head focused validation

SameBoy commit: `8182d8edbaddbac9f6057930f6cebdabd3b83228` (`codex/dmg-audio-fidelity`). The tracked worktree was clean before and after the run. These checks validate the current publication head after braces-only save-state formatting and a comment clarifying that the physical CPU/board revision is unconfirmed.

All commands completed successfully, sequentially against the same release library:

- Noise feedback: DMG/CGB phase behavior, reset/retrigger edges, narrow-sequence preservation, native-state round trip, and legacy 104-byte APU compatibility.
- Pulse clock: CH1/CH2 phase advances after length expiry, and legacy 112-byte APU gate reconstruction.
- Pulse duty: CH1/CH2 retained duty output across retrigger, native-state round trip, legacy 120-byte reconstruction, and zero-volume retention.
- Mixer: analog held DAC sample behavior on DMG/CGB and unchanged digital AGB behavior.
- Complete v2 CPU probe: all 57 readback cases match the physical oracle, with zero length mismatches or differing bytes. This includes all 11,587 sweep transitions (192 stops and 11,395 survivors), prefix controls, and wave readbacks.
- `git -C SameBoy diff --check 26a56bd6..HEAD`.
- Complete measured sweep header regenerates byte for byte.

`focused-validation.json` records exact commands, timings, commit, tool versions, and source/library/log hashes. `cpu-full/results.json` records every expected and observed readback, plus the original physical oracle, manifest, and ROM hashes. Individual `.log` files preserve command output.

This is an emulator regression run against previously decoded physical measurements. It is not a new hardware recording. Previously completed full WAV comparisons and broader regression suites were not rerun because the publication changes do not alter executable behavior.
