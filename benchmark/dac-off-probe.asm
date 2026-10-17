; Minimal mute-tail diagnostic. Build with KEEP_DAC=0 or 1.
; NR43=$fa stops LFSR clocks (as in the LSDj trace), so this exercises the
; analog output path without an unknown noise sequence. KEEP_DAC leaves an
; unrouted, inactive pulse DAC enabled as a control: all-DAC disconnection
; must not be confused with channel activity or panning.

MACRO wait_cycles
    ; LD BC=12 cycles; loop=28*n-4. The remainder is whole 4-cycle NOPs.
    ASSERT (\1) % 4 == 0
    DEF count\@ EQU ((\1) - 8) / 28
    ASSERT count\@ > 0 && count\@ <= $ffff
    ld bc, count\@
.delay\@:
    dec bc
    ld a, b
    or c
    jr nz, .delay\@
    REPT ((\1) - 8 - 28 * count\@) / 4
        nop
    ENDR
ENDM

SECTION "Entry", ROM0[$100]
    nop
    jp Start
    ds $150 - @, 0

SECTION "Diagnostic", ROM0[$150]
Start:
    di
    ld sp, $dfff
.vblank:
    ldh a, [$ff44]
    cp 144
    jr c, .vblank
    xor a
    ldh [$ff40], a
    ldh [$ff26], a
    ldh [$ff04], a
    ld a, $80
    ldh [$ff26], a
    ld a, $66
    ldh [$ff24], a
    ld a, $88
    ldh [$ff25], a
IF KEEP_DAC
    ld a, $08
ELSE
    ld a, $00
ENDC
    ldh [$ff12], a
    ld a, $fa
    ldh [$ff22], a
.repeat:
    ld a, $80
    ldh [$ff21], a
    ldh [$ff23], a
    ; Use the observed 105336-cycle restart interval as the diagnostic hold
    ; duration. Instruction overhead is explicit in the captured trace.
    wait_cycles 105336
    xor a
    ldh [$ff21], a
    wait_cycles 105336
    jr .repeat
