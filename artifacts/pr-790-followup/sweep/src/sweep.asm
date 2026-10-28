INCLUDE "macros.inc"

; Each shift owns 512 bytes: four pre/post NR52 bit pairs per byte.
; No SRAM is used. Unmeasured padding is never included in the results.
SECTION "Observations", WRAM0[$c000]
wObserved:: ds 7 * 512
wReadback:: ds 255
wReadbackSize:: ds 1
wStats:: ds 7 * 6             ; little-endian stops, differences, invalid controls
wBatchPointer: ds 2
wDestination: ds 2
wShift:: ds 1
wFrequency:: ds 2
wLimit: ds 2
wPair:: ds 1
wExpected: ds 1
wMismatch: ds 1
wPage:: ds 1
wKeys: ds 1
wAnyDifference: ds 1
wAnyInvalid: ds 1
wStateEnd:
ASSERT wStateEnd < $d000

SECTION "Entry", ROM0[$100]
    nop
    jp Start
    ds $150 - @, 0

SECTION "Program", ROM0[$150]
Start:
    di
    ld sp, $dfff
    xor a
    ld [$ffff], a
    ldh [$ff0f], a
    ldh [$ff26], a
    call LCDOff
    ld hl, wObserved
    ld bc, wStateEnd - wObserved
.clear:
    xor a
    ld [hli], a
    dec bc
    ld a, b
    or c
    jr nz, .clear
    ld de, Font
    ld hl, $8000
    ld bc, FontEnd - Font
    call Copy
    xor a
    ldh [$ff42], a
    ldh [$ff43], a
    ld a, $e4
    ldh [$ff47], a
    ld de, IntroScreen
    call LoadScreen
    call LCDOn
.wait_start:
    call PressedKeys
    bit 7, a
    jr z, .wait_start
    call LCDOff
    write_reg $ff82, 2        ; external validation marker: measurement starts
    call Measure
    write_reg $ff26, 0
    call Analyze
    call ShowSummary
    call WaitFrame
    call WaitFrame
    write_reg $ff82, 4        ; complete, with rendered results already visible
.buttons:
    call PressedKeys
    bit 7, a
    jr nz, .summary
    bit 0, a
    jr nz, .right
    bit 1, a
    jr nz, .left
    ld b, a
    ld a, [wPage]
    and a
    jr z, .buttons
    bit 2, b
    jr nz, .previous
    bit 3, b
    jr z, .buttons
    call NextDifference
    call ShowDetail
    jr .buttons
.previous:
    call PreviousDifference
    call ShowDetail
    jr .buttons
.summary:
    call ShowSummary
    jr .buttons
.right:
    ld a, [wPage]
    inc a
    cp 8
    jr nz, .select
    xor a
    jr .select
.left:
    ld a, [wPage]
    sub 1
    jr nc, .select
    ld a, 7
.select:
    and a
    jr z, .summary
    ld [wPage], a
    ld [wShift], a
    call SetLimit
    ld a, $ff
    ld [wFrequency], a
    ld [wFrequency + 1], a
    call NextDifference
    call ShowDetail
    jr .buttons

; The measurement bodies below are copied verbatim from the frozen audio ROM.
; Only dispatch and reporting change. LCD and interrupts stay off throughout.
Measure:
    ld hl, BatchTable
.next:
    ld a, [hli]
    ld e, a
    ld a, [hli]
    ld d, a
    or e
    ret z
    push de
    ld a, [hli]
    ld [wDestination], a
    ld a, [hli]
    ld [wDestination + 1], a
    ld a, l
    ld [wBatchPointer], a
    ld a, h
    ld [wBatchPointer + 1], a
    pop hl
    ld de, .returned
    push de
    jp hl
.returned:
    ld a, [wDestination]
    ld l, a
    ld a, [wDestination + 1]
    ld h, a
    ld de, wReadback
    ld a, [wReadbackSize]
    ld c, a
    ld b, 0
    call Copy
    ld a, [wBatchPointer]
    ld l, a
    ld a, [wBatchPointer + 1]
    ld h, a
    jr .next

Analyze:
    ld a, 1
    ld [wShift], a
.shift:
    call SetLimit
    xor a
    ld [wFrequency], a
    ld [wFrequency + 1], a
.frequency:
    call Result
    cp 2
    jr z, .invalid
    ld a, [wPair]
    bit 0, a
    jr nz, .difference
    call StatsAddress
    call IncrementWord
.difference:
    ld a, [wMismatch]
    and a
    jr z, .continue
    ld [wAnyDifference], a
    call StatsAddress
    inc hl
    inc hl
    call IncrementWord
    jr .continue
.invalid:
    ld [wAnyInvalid], a
    call StatsAddress
    ld bc, 4
    add hl, bc
    call IncrementWord
.continue:
    ld hl, wFrequency
    call IncrementWord
    call AtLimit
    jr nz, .frequency
    ld hl, wShift
    inc [hl]
    ld a, [hl]
    cp 8
    jr nz, .shift
    ret

; Result: A=0 agrees, 1 differs, 2 invalid initial state. Side results retain
; both observed bits and the expected STOP bit, for honest error reporting.
Result:
    ld a, [wShift]
    dec a
    add a
    add HIGH(wObserved)
    ld h, a
    ld l, 0
    call LoadFrequency
    ld a, e
    and 3
    inc a
    ld c, a
    srl d
    rr e
    srl d
    rr e
    add hl, de
    ld a, [hl]
.pair:
    rlca
    rlca
    dec c
    jr nz, .pair
    and 3
    ld [wPair], a

    ld a, [wShift]
    dec a
    add HIGH(Reference)
    ld h, a
    ld l, 0
    call LoadFrequency
    ld a, e
    and 7
    ld c, a
    REPT 3
        srl d
        rr e
    ENDR
    add hl, de
    ld b, [hl]
    ld a, c
    and a
    ld a, b
    jr z, .reference
.bit:
    rrca
    dec c
    jr nz, .bit
.reference:
    and 1
    ld [wExpected], a
    ld b, a
    ld a, [wPair]
    bit 1, a
    jr z, .invalid
    and 1
    xor 1
    xor b
    ld [wMismatch], a
    ret
.invalid:
    ld a, 2
    ld [wMismatch], a
    ret

LoadFrequency:
    ld a, [wFrequency]
    ld e, a
    ld a, [wFrequency + 1]
    ld d, a
    ret

SetLimit:
    ld a, [wShift]
    dec a
    add a
    ld e, a
    ld d, 0
    ld hl, Limits
    add hl, de
    ld a, [hli]
    ld [wLimit], a
    ld a, [hl]
    ld [wLimit + 1], a
    ret

AtLimit:
    ld a, [wLimit]
    ld b, a
    ld a, [wFrequency]
    cp b
    ret nz
    ld a, [wLimit + 1]
    ld b, a
    ld a, [wFrequency + 1]
    cp b
    ret

StatsAddress:
    ld a, [wShift]
    dec a
    ld b, a
    add a
    add b
    add a
    ld c, a
    ld b, 0
    ld hl, wStats
    add hl, bc
    ret

IncrementWord:
    inc [hl]
    ret nz
    inc hl
    inc [hl]
    ret

HasDifferences:
    call StatsAddress
    inc hl
    inc hl
    ld a, [hli]
    ld b, a
    ld a, [hli]
    or b
    ld b, a
    ld a, [hli]
    or b
    ld b, a
    ld a, [hl]
    or b
    ret

NextDifference:
    call HasDifferences
    ret z
.next:
    ld hl, wFrequency
    call IncrementWord
    call AtLimit
    jr nz, .check
    xor a
    ld [wFrequency], a
    ld [wFrequency + 1], a
.check:
    call Result
    and a
    jr z, .next
    ret

PreviousDifference:
    call HasDifferences
    ret z
.next:
    call LoadFrequency
    ld a, d
    or e
    jr nz, .decrement
    ld a, [wLimit]
    ld e, a
    ld a, [wLimit + 1]
    ld d, a
.decrement:
    dec de
    ld a, e
    ld [wFrequency], a
    ld a, d
    ld [wFrequency + 1], a
    call Result
    and a
    jr z, .next
    ret

ShowSummary:
    call LCDOff
    ld de, SummaryScreen
    call LoadScreen
    xor a
    ld [wPage], a
    ld a, 1
    ld [wShift], a
.row:
    ld a, [wShift]
    add 3
    call ScreenRow
    ld a, [wShift]
    add '0' - 32
    ld [hli], a
    inc hl
    push hl
    call StatsAddress
    call LoadWord
    pop hl
    call Print4
    ld a, [wShift]
    dec a
    add a
    ld e, a
    ld d, 0
    ld hl, ReferenceCounts
    add hl, de
    call LoadWord
    ld a, [wShift]
    add 3
    call ScreenRow
    ld bc, 7
    add hl, bc
    call Print3
    call StatsAddress
    inc hl
    inc hl
    call LoadWord
    ld a, [wShift]
    add 3
    call ScreenRow
    ld bc, 11
    add hl, bc
    call Print4
    call StatsAddress
    ld bc, 4
    add hl, bc
    call LoadWord
    ld a, [wShift]
    add 3
    call ScreenRow
    ld bc, 16
    add hl, bc
    call Print4
    ld hl, wShift
    inc [hl]
    ld a, [hl]
    cp 8
    jr nz, .row
    ld de, MatchText
    ld a, [wAnyDifference]
    and a
    jr z, .invalid
    ld de, DifferentText
.invalid:
    ld a, [wAnyInvalid]
    and a
    jr z, .status
    ld de, InvalidText
.status:
    ld hl, $9800 + 13 * 32
    call PrintText
    jp LCDOn

ShowDetail:
    call LCDOff
    ld de, DetailScreen
    call LoadScreen
    ld a, [wShift]
    add '0' - 32
    ld [$9800 + 1 * 32 + 6], a
    ld a, [wLimit]
    ld e, a
    ld a, [wLimit + 1]
    ld d, a
    ld hl, $9800 + 2 * 32 + 7
    call Print4
    call StatsAddress
    call LoadWord
    ld hl, $9800 + 3 * 32 + 5
    call Print4
    ld a, [wShift]
    dec a
    add a
    ld e, a
    ld d, 0
    ld hl, ReferenceCounts
    add hl, de
    call LoadWord
    ld hl, $9800 + 3 * 32 + 14
    call Print4
    call StatsAddress
    inc hl
    inc hl
    call LoadWord
    ld hl, $9800 + 4 * 32 + 5
    call Print4
    call StatsAddress
    ld bc, 4
    add hl, bc
    call LoadWord
    ld hl, $9800 + 4 * 32 + 14
    call Print4
    call HasDifferences
    jr nz, .detail
    ld hl, $9800 + 7 * 32
    ld de, NoDifferenceText
    call PrintText
    jp LCDOn
.detail:
    call Result
    ld hl, $9800 + 7 * 32
    ld de, FrequencyText
    call PrintText
    call LoadFrequency
    ld a, d
    call PrintHexDigit
    ld a, e
    swap a
    call PrintHexDigit
    ld a, e
    call PrintHexDigit
    ld hl, $9800 + 9 * 32
    ld de, BitsText
    call PrintText
    ld a, [wPair]
    rrca
    and 1
    add '0' - 32
    ld [$9800 + 9 * 32 + 9], a
    ld a, [wPair]
    and 1
    add '0' - 32
    ld [$9800 + 9 * 32 + 16], a
    ld hl, $9800 + 11 * 32
    ld de, JeffText
    call PrintText
    ld a, [wExpected]
    call PrintOutcome
    ld hl, $9800 + 12 * 32
    ld de, UnitText
    call PrintText
    ld a, [wMismatch]
    cp 2
    jr z, .invalid
    ld a, [wPair]
    and 1
    xor 1
    call PrintOutcome
    jp LCDOn
.invalid:
    ld de, InvalidShortText
    call PrintText
    jp LCDOn

PrintOutcome:
    and a
    ld de, LiveText
    jr z, PrintText
    ld de, StopText
PrintText:
    ld a, [de]
    inc de
    and a
    ret z
    sub 32
    ld [hli], a
    jr PrintText

PrintHexDigit:
    and 15
    cp 10
    jr c, .number
    add 'A' - '0' - 10
.number:
    add '0' - 32
    ld [hli], a
    ret

LoadWord:
    ld a, [hli]
    ld e, a
    ld a, [hl]
    ld d, a
    ret

; DE unsigned value, HL tile destination. Decimal, including leading zeroes.
Print4:
    push hl
    ld h, d
    ld l, e
    ld de, 1000
    call DecimalDigit
    pop de
    ld [de], a
    inc de
    push de
    jr PrintHundreds
Print3:
    push hl
    ld h, d
    ld l, e
PrintHundreds:
    ld de, 100
    call DecimalDigit
    pop de
    ld [de], a
    inc de
    push de
    ld de, 10
    call DecimalDigit
    pop de
    ld [de], a
    inc de
    ld a, l
    add '0' - 32
    ld [de], a
    ret

DecimalDigit:
    ld b, '0' - 32
.loop:
    ld a, h
    cp d
    jr c, .done
    jr nz, .subtract
    ld a, l
    cp e
    jr c, .done
.subtract:
    ld a, l
    sub e
    ld l, a
    ld a, h
    sbc d
    ld h, a
    inc b
    jr .loop
.done:
    ld a, b
    ret

ScreenRow:
    ld l, a
    ld h, 0
    REPT 5
        add hl, hl
    ENDR
    ld bc, $9800
    add hl, bc
    ret

LoadScreen:
    ld hl, $9800
    ld bc, 32 * 18
Copy:
    ld a, [de]
    inc de
    ld [hli], a
    dec bc
    ld a, b
    or c
    jr nz, Copy
    ret

LCDOff:
    ldh a, [$ff40]
    bit 7, a
    ret z
.wait:
    ldh a, [$ff44]
    cp 144
    jr c, .wait
    xor a
    ldh [$ff40], a
    ret
LCDOn:
    write_reg $ff40, $91
    ret
WaitFrame:
    ldh a, [$ff44]
    cp 144
    jr nc, WaitFrame
.wait:
    ldh a, [$ff44]
    cp 144
    jr c, .wait
    ret

PressedKeys:
    call WaitFrame
    write_reg $ff00, $20
    ldh a, [$ff00]
    ldh a, [$ff00]
    ldh a, [$ff00]
    cpl
    and 15
    ld b, a
    write_reg $ff00, $10
    ldh a, [$ff00]
    ldh a, [$ff00]
    ldh a, [$ff00]
    cpl
    and 15
    swap a
    or b
    ld b, a
    write_reg $ff00, $30
    ld a, [wKeys]
    cpl
    and b
    ld c, a
    ld a, b
    ld [wKeys], a
    ld a, c
    ret

MatchText: db "MATCHES JEFF'S UNIT", 0
DifferentText: db "DIFFERS FROM JEFF", 0
InvalidText: db "INVALID INITIAL DATA", 0
NoDifferenceText: db "NO DIFFERENCES", 0
FrequencyText: db "FREQ $", 0
BitsText: db "NR52 PRE   POST  ", 0
JeffText: db "JEFF: ", 0
UnitText: db "UNIT: ", 0
LiveText: db "LIVE", 0
StopText: db "STOP", 0
InvalidShortText: db "INVALID INIT", 0

INCLUDE "tables.inc"
INCLUDE "batches.inc"
INCLUDE "screens.inc"
Font:
    INCBIN "font.bin"
FontEnd:

SECTION "Physical comparison", ROM0, ALIGN[8]
Reference:
    INCBIN "reference.bin"
ReferenceEnd:
ASSERT ReferenceEnd - Reference == 7 * 256
