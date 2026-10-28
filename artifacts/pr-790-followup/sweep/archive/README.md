# Archival provenance, not current execution instructions

These selected files are byte-for-byte copies of the original local evidence.
Historical absolute paths may not exist on a reviewer's machine. None is used
as a runtime path by the portable scripts. They are retained to avoid rewriting
what the original experiments recorded.

| Packaged file/tree | Original artifact-relative path | Meaning |
| --- | --- | --- |
| `build-manifest.json` | `sweep-verify/rom/manifest.json` | Original ROM-only build record; the old `build.py` hash is not the adapted wrapper's hash |
| `variant.json` | `sweep-verify-flash/variant.json` | Exact MBC5 image identity and three changed header offsets |
| `flash-readback.json` | `sweep-verify-write-ow3fx1tw/result.json` | Prior authorized flash, complete ROM readback verified, no SRAM write |
| `validation/checks.json` | `sweep-verify/validation/checks.json` | Pre-hardware emulator-validation report; `hardware_tested: false` describes that check only |
| `validation/original/` | `sweep-verify/validation/original/` | Earlier sweep-only audio ROM's per-trial CPU trace and log |
| `validation/final-dmg/` | `sweep-verify/validation/final-dmg/` | Standalone ROM on final release core, historical run |
| `validation/native-*/` | `sweep-verify/validation/native-*/` | Historical four-model emulator traces, WRAM, screen text and logs |
| `validation/mixed/`, `opposite/`, `invalid/` | corresponding `sweep-verify/validation/` directories | Synthetic NR52 injections testing the reporter, never hardware evidence |
| `mbc5-emulator/` | `sweep-verify-flash/emulator/` | Exact hardware-header variant's pre-flash emulator trace, WRAM, screen text and log |

The current physical result is in [../hardware/README.md](../hardware/README.md).
The MBC5 image was run on the same DMG used for the earlier audio reference,
not an independent device. The photo is a summary rather than a hardware
per-trial dump. No historical flash authorization is authority to flash again.

Frozen `../inputs/` files also contain original source/audio/manifest paths and
hashes. They are preserved unchanged. Raw audio, unrelated probes, binaries,
object files and programmer tools are deliberately not included. The selected
screen text and four compact screenshots replace redundant historical PPMs;
new validation regenerates pixel captures if needed.

The original audio ROM is not bundled. The portable check compares its frozen
trace here to new standalone-ROM execution; it does not rerun that old ROM or
claim new timing measurements from hardware. No circuit-research netlists or
separately licensed model implementations are included.
