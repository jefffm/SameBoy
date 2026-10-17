; Standalone CPU-path reduction of physical v2 probe case 35.
; The measured inputs are wave[0] = FF, wave[4] = 00, wave[7] = FF.
; After the active retrigger, this unit returns FF at both wave[0] and wave[4].
; Plain block-copy emulation incorrectly returns 00 at both addresses.
;
; Expected 16 bytes (hardware packet, not a formula-generated expectation):
; FF 56 67 FF FF 56 67 FF 89 9A AB BC CD DE EF F0
; Physical oracle: benchmark/oracles/dmg-probe-v2-erekgdxk.json, wave case 35.
; HRAM FF82 = 3 marks completed WRAM output for wave-readback.c.

MACRO write_reg
    ld a, \2
    ldh [\1], a
ENDM

SECTION "Result", WRAM0
wReadbackSize:: ds 1
wReadback:: ds 16

SECTION "Entry", ROM0[$100]
    nop
    jp Start
    ds $150 - @, 0

SECTION "Repro", ROM0[$150]
Start:
    di
    ld sp, $dfff
    write_reg $ff26, $00
    write_reg $ff04, $00
    write_reg $ff26, $80
    write_reg $ff24, $77
    write_reg $ff25, $00
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $12
    write_reg $ff32, $23
    write_reg $ff33, $34
    write_reg $ff34, $00
    write_reg $ff35, $56
    write_reg $ff36, $67
    write_reg $ff37, $ff
    write_reg $ff38, $89
    write_reg $ff39, $9a
    write_reg $ff3a, $ab
    write_reg $ff3b, $bc
    write_reg $ff3c, $cd
    write_reg $ff3d, $de
    write_reg $ff3e, $ef
    write_reg $ff3f, $f0
    write_reg $ff1b, $00
    write_reg $ff1c, $20
    write_reg $ff1d, $f8
    write_reg $ff1a, $80
    write_reg $ff1e, $87

    ; The measured 208 T-cycle wait, including preservation of registers.
    push af
    push bc
    ld bc, 5
.wait:
    dec bc
    ld a, b
    or c
    jr nz, .wait
    pop bc
    pop af
    nop

    write_reg $ff1e, $87
    write_reg $ff1a, $00
    DEF i = 0
    REPT 16
        ldh a, [$ff30 + i]
        ld [wReadback + i], a
        REDEF i = i + 1
    ENDR
    ld a, 16
    ld [wReadbackSize], a
    write_reg $ff82, 3
.done:
    halt
    jr .done
