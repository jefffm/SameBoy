# PR #790 review follow-up evidence

These are the promised CH3/CH4 recording probes and standalone sweep comparison
for [SameBoy PR #790](https://github.com/LIJI32/SameBoy/pull/790).
They accompany [core revision e31144f1](https://github.com/jefffm/SameBoy/commit/e31144f18194ff8dc9f9e648a7cac7be412d3aec).
Review cleanup and the focused behavior fixes remain separate commits.

The physical evidence is from Jeff's one DMG. Its CPU revision is unconfirmed.
DMG-B names the emulated model, not an identified hardware revision. The
focused CH3/CH4 fixes and empirical sweep map remain DMG-B-scoped. Other
pre-CGB generalization follows the reviewer's requested assumption, except
for the wave-RAM behavior independently confirmed on MGB by the reviewer.

## CH3 and CH4

[Download the complete package (45.4 MB)](sameboy-pr790-ch34-evidence.zip?raw=1)
· [Sources, instructions and measurement limits](ch34/README.md)
· [Verification results](ch34/validation/README.md)

- [Focused ROM](ch34/ch34-focused-probe/rom/ch34-focused-probe.gb?raw=1):
  calibration plus 62 CH3 and 54 CH4 cases, about 45 seconds from boot.
- [Initial ROM](ch34/ch34-clock-probe/rom/ch34-clock-probe.gb?raw=1):
  calibration plus 36 CH3 and 48 CH4 cases, about 33 seconds from boot.

Both run once automatically with the LCD off and report observations through
headphone audio. They do not display pass/fail. Record before powering on and
allow 60 seconds for the focused ROM. The package includes all three lossless
physical recordings, frozen references, analysis, current companion tests,
and portable rebuild/regression commands.

All 62 focused wave cases and 54 noise cases pass the documented recorded-output
allowances. Both earlier takes also pass. Fresh Clang/GCC runs and a relocated
package against a fresh source tree reproduce these results. The observations
do not directly establish hidden CH3-index or CH4-LFSR continuation.

## Standalone sweep test

[Download the complete package (6.2 MB)](sameboy-pr790-sweep-evidence.zip?raw=1)
· [Sources and instructions](sweep/README.md)
· [Hardware photo and result](sweep/hardware/README.md)
· [Verification results](sweep/verification/README.md)

- [Hardware-tested MBC5 image](sweep/rom/sweep-verify-mbc5.gb?raw=1), for a
  compatible cartridge or loader.
- [ROM-only image](sweep/rom/sweep-verify.gb?raw=1), for a compatible ROM-only
  loader. This variant was not separately boot-tested on physical hardware.

Press **Start** and allow about 95 seconds with the LCD off. Photograph the
summary. Left/Right selects shifts, Up/Down selects differing or invalid
frequencies, and Start returns to the summary. Include the console model and
known CPU revision when sharing results.

The exact MBC5 image reproduced all 192 stops across 11,587 eligible trials on
Jeff's same DMG, with zero differences and invalid initial states. The two
images differ only in three cartridge-header/checksum bytes. The test compares
each observation against that unit, not against a universal hardware verdict.
Other units can provide useful different results.

Both images rebuild byte-for-byte. Eight complete emulator/reporting runs
exercise native models, both variants, and synthetic differing/invalid results.
The empirical map also regenerates exactly, and all eight CPU boundary cases
pass with the final `.inc` path. Legacy `.h` lookup remains supported.

## Frozen package integrity and provenance

[SHA256SUMS](SHA256SUMS) and [package manifest](package-manifest.json) identify
the exact verified ZIPs. Each ZIP contains its own complete file checksum list.
The extracted source directories match those ZIPs byte-for-byte. No host
executables, objects, proprietary music ROMs, or cartridge-programming tools
are included.

The packages preserve the verified preparation snapshots, including historical
reports and their original paths. Statements inside them about being local or
unpublished describe that preparation stage. This page is their publication
entry point. Historical paths are provenance, not runtime requirements.

The packaged scripts were run after relocation, and both ZIPs passed complete
extraction/hash checks. The final extracted sweep replay is also retained in
[extracted-sweep-checks.json](extracted-sweep-checks.json).
No new hardware recording or flash was performed during publication.

Native state roundtrips previously passed their recorded-output thresholds but
were not PCM-byte-identical. Existing sanitizer and full-packaging limitations
remain documented. No new sanitizer-clean, all-platform, or cross-device
accuracy claim is made. Existing license terms in the repository root apply.
