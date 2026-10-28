; All timing is in normal-speed DMG T-cycles (4,194,304 Hz).

MACRO write_reg
    ld a, \2
    ldh [\1], a
ENDM

; Preserve every register and flag. The counted loop costs 28*N+64 T,
; including the pushes, setup and pops. Larger waits use the full 16-bit
; counter range; the remaining multiple of four is filled with NOPs.
MACRO _wait_counted
    push af
    push bc
    ld bc, \1
.wait\@:
    dec bc
    ld a, b
    or c
    jr nz, .wait\@
    pop bc
    pop af
ENDM

MACRO wait_cycles
    ASSERT (\1) >= 0 && ((\1) % 4) == 0, "wait_cycles requires a nonnegative multiple of four"
    DEF _wait_full\@ = 28 * 65535 + 64
    REPT (\1) / _wait_full\@
        _wait_counted 65535
    ENDR
    DEF _wait_rest\@ = (\1) % _wait_full\@
    IF _wait_rest\@ >= 92
        DEF _wait_n\@ = (_wait_rest\@ - 64) / 28
        _wait_counted _wait_n\@
        REPT (_wait_rest\@ - (28 * _wait_n\@ + 64)) / 4
            nop
        ENDR
    ELSE
        REPT _wait_rest\@ / 4
            nop
        ENDR
    ENDC
ENDM
