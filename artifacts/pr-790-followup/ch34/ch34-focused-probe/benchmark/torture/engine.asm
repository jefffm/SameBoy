INCLUDE "macros.inc"

; Case routines may clobber all registers, but must balance their stack and RET.
; The table and engine state therefore live outside those registers.
SECTION "Torture state", WRAM0
wCaseTablePointer: ds 2
wCaseBank: ds 1
wCaseAddress: ds 2
wReadbackSize:: ds 1
wReadback:: ds 255             ; payload length is one byte
wPacket: ds 6 + 255 + 1       ; header, maximum payload, XOR checksum

SECTION "Entry", ROM0[$100]
    nop
    jp Start
    ds $150 - @, 0

SECTION "Engine", ROM0[$150]
Start:
    di
    ld sp, $dfff
.vblank:
    ldh a, [$ff44]
    cp 144
    jr c, .vblank
    xor a
    ldh [$ff40], a             ; LCD off throughout measurement
    ld [$3000], a              ; MBC5 bank bit 8 stays zero
    ld hl, CaseTable
    ld a, l
    ld [wCaseTablePointer], a
    ld a, h
    ld [wCaseTablePointer + 1], a
.next_case:
    ld a, [wCaseTablePointer]
    ld l, a
    ld a, [wCaseTablePointer + 1]
    ld h, a
    ld a, [hli]
    and a
    jp z, .finished
    ld [wCaseBank], a
    ld a, [hli]
    ld [wCaseAddress], a
    ld a, [hli]
    ld [wCaseAddress + 1], a
    ld a, [hli]
    ldh [$ff80], a
    ld a, [hli]
    ldh [$ff81], a
    ld a, l
    ld [wCaseTablePointer], a
    ld a, h
    ld [wCaseTablePointer + 1], a

    ld a, 1                   ; acoustic case marker, no payload
    call SendPacket
    xor a
    ldh [$ff26], a             ; reset all channels and DACs
    ldh [$ff04], a             ; reset DIV while the APU is off
    ld [wReadbackSize], a
    write_reg $ff26, $80
    write_reg $ff24, $77
    write_reg $ff25, $00

    ld a, [wCaseBank]
    ld [$2000], a
    ld a, [wCaseAddress]
    ld l, a
    ld a, [wCaseAddress + 1]
    ld h, a
    ld de, .case_returned
    push de
    write_reg $ff82, 2
    jp hl
.case_returned:
    write_reg $ff82, 3
    write_reg $ff26, $00
    ld a, [wReadbackSize]
    and a
    jr z, .no_readback
    ld a, 2
    call SendPacket
.no_readback:
    jp .next_case
.finished:
    write_reg $ff80, $ff
    write_reg $ff81, $ff
    ld a, 3
    call SendPacket
    write_reg $ff82, 4
.halt:
    halt
    jr .halt

; D contains the next MSB-first byte. Both data paths take 44 T before
; LDH NR18. The NR18 write and trigger take another 32 T.
MACRO _send_bit
    sla d
    ld a, $c0
    jr nc, .zero\@
    ld a, $e0
    jr .ready\@
.zero\@:
    REPT 4
        nop
    ENDR
.ready\@:
    ldh [$ff18], a
    write_reg $ff19, $87
    wait_cycles \1
ENDM

; A = kind: 1 case marker, 2 readback, 3 suite end.
; Packet: D3 91 kind id_lo id_hi length payload... checksum.
; Checksum excludes D3 91 and XORs every other byte before itself.
SendPacket:
    ld hl, wPacket
    ld b, a
    ld a, $d3
    ld [hli], a
    ld a, $91
    ld [hli], a
    ld a, b
    ld [hli], a
    ldh a, [$ff80]
    ld [hli], a
    xor b
    ld b, a
    ldh a, [$ff81]
    ld [hli], a
    xor b
    ld b, a
    ld a, [wPacket + 2]
    cp 2
    ld a, 0
    jr nz, .have_length
    ld a, [wReadbackSize]
.have_length:
    ld [hli], a
    ld c, a
    xor b
    ld b, a
    ld a, c
    and a
    jr z, .checksum
    ld de, wReadback
.payload:
    ld a, [de]
    inc de
    ld [hli], a
    xor b
    ld b, a
    dec c
    jr nz, .payload
.checksum:
    ld a, b
    ld [hl], a

    write_reg $ff26, $00
    ; One complete frame-sequencer cycle on each side of each packet.
    ; This is a cadence guarantee, not an analog settling assumption.
    wait_cycles 65536
    write_reg $ff26, $80
    write_reg $ff16, $80        ; CH2, 50% duty
    write_reg $ff17, $88        ; volume 8, increase, envelope pace 0
    write_reg $ff24, $77
    write_reg $ff25, $22        ; CH2 to both outputs

    ld a, [wPacket + 5]
    ld c, a
    ld b, 0
    ld hl, 7
    add hl, bc
    ld b, h
    ld c, l
    ld hl, wPacket
    ld a, [wPacket + 2]
    cp 1
    jr z, .marker_event
    cp 2
    jr z, .readback_event
    ld a, 6
    jr .event
.readback_event:
    ld a, 5
    jr .event
.marker_event:
    ld a, 1
.event:
    ldh [$ff82], a
.byte:
    ld a, [hli]
    ld d, a
    REPT 7
        _send_bit 8116         ; 32 + 8116 + next decode 44 = 8192 T
    ENDR
    _send_bit 8072             ; allow 44 T for byte loop and next load
    dec bc
    ld a, b
    or c
    jp nz, .byte
    ; Final byte uses the untaken branch. Hold its final symbol for the
    ; same full 8192 T slot before the APU-off write starts.
    wait_cycles 56
    xor a
    ldh [$ff26], a
    wait_cycles 65536
    ret

CaseTable:
INCLUDE "case_table.inc"

INCLUDE "cases.inc"
