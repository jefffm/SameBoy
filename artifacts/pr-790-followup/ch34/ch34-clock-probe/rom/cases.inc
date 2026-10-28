SECTION "calibration-pulse", ROMX
Case_0001:
    write_reg $ff25, $22
    write_reg $ff16, $80
    write_reg $ff17, $88
    write_reg $ff18, $00
    write_reg $ff19, $86
    wait_cycles 524288
    ld a, 0
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-expired-dac-on-0", ROMX
Case_0002:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 0
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-expired-dac-on-1024", ROMX
Case_0003:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 128
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-expired-dac-on-2048", ROMX
Case_0004:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 256
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-expired-dac-on-4096", ROMX
Case_0005:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 512
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-expired-dac-on-8192", ROMX
Case_0006:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-expired-dac-on-16384", ROMX
Case_0007:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 2048
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-dac-off-0", ROMX
Case_0008:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 0
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-dac-off-1024", ROMX
Case_0009:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 128
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-dac-off-2048", ROMX
Case_000a:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 256
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-dac-off-4096", ROMX
Case_000b:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 512
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-dac-off-8192", ROMX
Case_000c:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-dac-off-16384", ROMX
Case_000d:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 2048
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-never-triggered-0", ROMX
Case_000e:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $40
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 0
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-never-triggered-1024", ROMX
Case_000f:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $40
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 128
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-never-triggered-2048", ROMX
Case_0010:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $40
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 256
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-never-triggered-4096", ROMX
Case_0011:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $40
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 512
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-never-triggered-8192", ROMX
Case_0012:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $40
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-00-ff-never-triggered-16384", ROMX
Case_0013:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $40
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    ld hl, $ff3f
    REPT 2048
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-expired-dac-on-0", ROMX
Case_0014:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 0
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-expired-dac-on-1024", ROMX
Case_0015:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 128
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-expired-dac-on-2048", ROMX
Case_0016:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 256
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-expired-dac-on-4096", ROMX
Case_0017:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 512
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-expired-dac-on-8192", ROMX
Case_0018:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-expired-dac-on-16384", ROMX
Case_0019:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 2048
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-dac-off-0", ROMX
Case_001a:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 0
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-dac-off-1024", ROMX
Case_001b:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 128
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-dac-off-2048", ROMX
Case_001c:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 256
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-dac-off-4096", ROMX
Case_001d:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 512
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-dac-off-8192", ROMX
Case_001e:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-dac-off-16384", ROMX
Case_001f:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 2048
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-never-triggered-0", ROMX
Case_0020:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $40
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 0
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-never-triggered-1024", ROMX
Case_0021:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $40
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 128
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-never-triggered-2048", ROMX
Case_0022:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $40
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 256
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-never-triggered-4096", ROMX
Case_0023:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $40
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 512
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-never-triggered-8192", ROMX
Case_0024:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $40
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch3-ff-00-never-triggered-16384", ROMX
Case_0025:
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $ff
    write_reg $ff31, $ff
    write_reg $ff32, $ff
    write_reg $ff33, $ff
    write_reg $ff34, $ff
    write_reg $ff35, $ff
    write_reg $ff36, $ff
    write_reg $ff37, $ff
    write_reg $ff38, $ff
    write_reg $ff39, $ff
    write_reg $ff3a, $ff
    write_reg $ff3b, $ff
    write_reg $ff3c, $ff
    write_reg $ff3d, $ff
    write_reg $ff3e, $ff
    write_reg $ff3f, $ff
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    write_reg $ff1e, $40
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 81920
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff1a, $80
    write_reg $ff30, $00
    write_reg $ff31, $00
    write_reg $ff32, $00
    write_reg $ff33, $00
    write_reg $ff34, $00
    write_reg $ff35, $00
    write_reg $ff36, $00
    write_reg $ff37, $00
    write_reg $ff38, $00
    write_reg $ff39, $00
    write_reg $ff3a, $00
    write_reg $ff3b, $00
    write_reg $ff3c, $00
    write_reg $ff3d, $00
    write_reg $ff3e, $00
    write_reg $ff3f, $00
    ld hl, $ff3f
    REPT 2048
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
    write_reg $ff1d, $00
    write_reg $ff1e, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 8192
    write_reg $ff1c, $00
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-0", ROMX
Case_0026:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-256", ROMX
Case_0027:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 256
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-512", ROMX
Case_0028:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 512
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-768", ROMX
Case_0029:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 768
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-1024", ROMX
Case_002a:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 1024
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-1280", ROMX
Case_002b:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 1280
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-1536", ROMX
Case_002c:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 1536
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-4096", ROMX
Case_002d:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 4096
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-0", ROMX
Case_002e:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-256", ROMX
Case_002f:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 256
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-512", ROMX
Case_0030:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 512
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-768", ROMX
Case_0031:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 768
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-1024", ROMX
Case_0032:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 1024
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-1280", ROMX
Case_0033:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 1280
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-1536", ROMX
Case_0034:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 1536
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-4096", ROMX
Case_0035:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 4096
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-active-muted-0", ROMX
Case_0036:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-active-muted-256", ROMX
Case_0037:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 256
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-active-muted-512", ROMX
Case_0038:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 512
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-active-muted-768", ROMX
Case_0039:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 768
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-active-muted-1024", ROMX
Case_003a:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 1024
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-active-muted-1280", ROMX
Case_003b:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 1280
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-active-muted-1536", ROMX
Case_003c:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 1536
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-active-muted-4096", ROMX
Case_003d:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $53
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 4096
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-0", ROMX
Case_003e:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-256", ROMX
Case_003f:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 256
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-512", ROMX
Case_0040:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 512
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-768", ROMX
Case_0041:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 768
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-1024", ROMX
Case_0042:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 1024
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-1280", ROMX
Case_0043:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 1280
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-1536", ROMX
Case_0044:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 1536
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-4096", ROMX
Case_0045:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 4096
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-0", ROMX
Case_0046:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-256", ROMX
Case_0047:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 256
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-512", ROMX
Case_0048:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 512
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-768", ROMX
Case_0049:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 768
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-1024", ROMX
Case_004a:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 1024
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-1280", ROMX
Case_004b:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 1280
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-1536", ROMX
Case_004c:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 1536
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-4096", ROMX
Case_004d:
    write_reg $ff25, $88
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $c0
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff21, $00
    wait_cycles 4096
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
    write_reg $ff21, $f0
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-active-muted-0", ROMX
Case_004e:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-active-muted-256", ROMX
Case_004f:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 256
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-active-muted-512", ROMX
Case_0050:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 512
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-active-muted-768", ROMX
Case_0051:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 768
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-active-muted-1024", ROMX
Case_0052:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 1024
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-active-muted-1280", ROMX
Case_0053:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 1280
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-active-muted-1536", ROMX
Case_0054:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 1536
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-active-muted-4096", ROMX
Case_0055:
    write_reg $ff25, $00
    write_reg $ff20, $3f
    write_reg $ff21, $f0
    write_reg $ff22, $5b
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 32768
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 20
    wait_cycles 4096
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff25, $88
    wait_cycles 20
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
