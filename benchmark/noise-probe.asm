; A paired hardware diagnostic, not a known-failing regression.
; These values occur together in the opening LSDj noise sequence.
; Build with RETRIGGER=0 for continuous noise or 1 for NR44-only restarts.
;
; 105336 T-cycles is an observed restart interval in baseline.csv and equals
; 1.5 DMG video frames. It is not a claim that every LSDj interval is identical.
; The uninterrupted 7-bit sequence takes 127 * 6144 = 780288 T-cycles.

SECTION "Entry", ROM0[$100]
    nop
    jp Start
    ds $150 - @, 0

SECTION "Diagnostic", ROM0[$150]
Start:
    di
    ld sp, $dfff
    ; Turn the LCD off during VBlank. Neither mode polls joypad or uses IRQs.
.vblank:
    ldh a, [$ff44]
    cp 144
    jr c, .vblank
    xor a
    ldh [$ff40], a
    ldh [$ff26], a             ; APU off, all DACs off
    ldh [$ff04], a             ; one documented DIV reset, before APU on
    ld a, $80
    ldh [$ff26], a
    ld a, $66               ; settled NR50 during the selected LSDj section
    ldh [$ff24], a
    ld a, $88               ; noise to both sides, other channels disabled
    ldh [$ff25], a
    xor a
    ldh [$ff20], a
    ld a, $98               ; volume 9, envelope pace 0
    ldh [$ff21], a
    ld a, $6e               ; 7-bit noise, divisor 6, shift 6
    ldh [$ff22], a
    ld a, $80               ; length disabled
    ldh [$ff23], a             ; shared first trigger
    jp .wait
.wait:
    ld bc, 3760
.delay:
    dec bc
    ld a, b
    or c
    jr nz, .delay
    nop
    nop
    nop
    ld a, $80
IF RETRIGGER
    ldh [$ff23], a
ELSE
    ; Same instruction duration as LDH. Leave the LFSR running continuously.
    nop
    nop
    nop
ENDC
    jp .wait
    ; Between LDH write starts: 12 + 16 + 12 + (28*3760-4) + 12 + 8
    ; = 105336 T-cycles. No NR42/NR43 changes or DAC resets in either loop.
