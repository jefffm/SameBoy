INCLUDE "macros.inc"
SECTION "boot", ROM0[$0000]
 ld a,1
 ldh [$ff50],a
 nop
 jp Start
SECTION "entry", ROM0[$0150]
Start:
 di
 ld sp,$dfff
 xor a
 ldh [$ff40],a
    ld de, $04cd
    ld bc, 0
.trial:
    write_reg $ff26, $00
    write_reg $ff04, $00
    write_reg $ff26, $80
    write_reg $ff24, $77
    write_reg $ff25, $00
    write_reg $ff10, $12
    write_reg $ff11, $80
    write_reg $ff12, $f0
    ld a, e
    ldh [$ff13], a
    ld a, d
    or $80
    ldh [$ff14], a
    wait_cycles 8192
    ldh a, [$ff26]
    rrca
    rl b
    wait_cycles 24576
    ldh a, [$ff26]
    rrca
    rl b
 ld a,b
 ld [$c004],a
 write_reg $ff82,4
 xor a
 ld [$e5a4],a
 ld [$e5a5],a
 jr @
