# Physical standalone sweep comparison, 2026-10-28

Jeff supplied `summary.jpg` after the authorized sweep-ROM flash and run on
his DMG. The original attachment is preserved unchanged:

https://ampcode.com/user-content/attachments/e72e34c767f537f097584fcf93807a397ca4f60e81f30e8d4effec780b784745-amp-camera.jpg

Photo SHA-256:
`fee501ee1f55c0f25f1ebb309793614a70b934cb3494c63498194c4793371d31`

The table and status were visually checked in upright crops of the original
photo. `table.jpg` and `status.png` are inspection aids, not new observations.

## Displayed result

| Shift | STOP | REF | DIFF | BAD |
| --- | ---: | ---: | ---: | ---: |
| 1 | 127 | 127 | 0 | 0 |
| 2 | 54 | 54 | 0 | 0 |
| 3 | 7 | 7 | 0 | 0 |
| 4 | 0 | 0 | 0 | 0 |
| 5 | 2 | 2 | 0 | 0 |
| 6 | 1 | 1 | 0 | 0 |
| 7 | 1 | 1 | 0 | 0 |
| Total | 192 | 192 | 0 | 0 |

The screen reads `TRIALS 11587`, `MATCHES JEFF'S UNIT`, and
`NOT A HARDWARE PASS`. The comparator reports zero frequency-level
differences and zero invalid initial controls across its measured domain,
not merely equal aggregate stop counts.

## Artifact identity and limits

The flashed 32768-byte MBC5-compatible variant has SHA-256
`784825ebfa76fcb33f45aeb12275d34cfdf373a4e9f176f2bea6452e3d507b03`.
The exact image is `../rom/sweep-verify-mbc5.gb`.
The unchanged flash record is `../archive/flash-readback.json`; it records
complete byte-for-byte ROM readback and no SRAM write.
Its local device/ROM paths are archival provenance, not reproduction inputs.

Relative to the distributed ROM-only image, only offsets `$147`, `$149`,
and `$14D` changed (cartridge type, RAM declaration, and header checksum).
All test code and reference bytes are unchanged. Variant provenance and
its emulator trace are in `../archive/variant.json` and
`../archive/mbc5-emulator/`. Its complete trial
trace, retained WRAM, and screens matched the original ROM before flashing.

This is a physical execution of the standalone cartridge-compatible test
on Jeff's existing DMG, reproducing the earlier audio-derived reference.
The console's CPU revision remains unconfirmed. It is not evidence from a
second unit, MGB, or SGB. The photo supplies a summary, not a raw per-trial
hardware dump, new timing measurements, or an observed run duration.

This supplement supersedes the pre-run status for the cartridge-compatible
variant. The ROM-only header variant has not separately been physically
boot-tested. Packaging copies existing evidence only: no additional flash,
hardware run, upload, GitHub post, push, or commit was performed.
