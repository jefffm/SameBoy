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
SECTION "ch3-hold-00-0", ROMX
Case_0002:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-00-256", ROMX
Case_0003:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 256
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-00-1024", ROMX
Case_0004:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 1024
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-00-4096", ROMX
Case_0005:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-00-16384", ROMX
Case_0006:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 16384
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-55-0", ROMX
Case_0007:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-55-256", ROMX
Case_0008:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 256
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-55-1024", ROMX
Case_0009:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 1024
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-55-4096", ROMX
Case_000a:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-55-16384", ROMX
Case_000b:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 16384
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-aa-0", ROMX
Case_000c:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-aa-256", ROMX
Case_000d:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 256
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-aa-1024", ROMX
Case_000e:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 1024
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-aa-4096", ROMX
Case_000f:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-aa-16384", ROMX
Case_0010:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 16384
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-ff-0", ROMX
Case_0011:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-ff-256", ROMX
Case_0012:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 256
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-ff-1024", ROMX
Case_0013:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 1024
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-ff-4096", ROMX
Case_0014:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-hold-ff-16384", ROMX
Case_0015:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 16384
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-0-00-idle", ROMX
Case_0016:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
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
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-0-00-wave-read", ROMX
Case_0017:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
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
    wait_cycles 20
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
SECTION "ch3-bus-0-00-wram-read", ROMX
Case_0018:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
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
    ld hl, $c800
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-0-55-idle", ROMX
Case_0019:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    ld hl, $ff3f
    REPT 1024
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-0-55-wave-read", ROMX
Case_001a:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    ld hl, $ff3f
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-0-55-wram-read", ROMX
Case_001b:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    ld hl, $c800
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-0-aa-idle", ROMX
Case_001c:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    ld hl, $ff3f
    REPT 1024
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-0-aa-wave-read", ROMX
Case_001d:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    ld hl, $ff3f
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-0-aa-wram-read", ROMX
Case_001e:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    ld hl, $c800
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-0-ff-idle", ROMX
Case_001f:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
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
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-0-ff-wave-read", ROMX
Case_0020:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
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
    wait_cycles 20
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
SECTION "ch3-bus-0-ff-wram-read", ROMX
Case_0021:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
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
    ld hl, $c800
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-no-rewrite-0", ROMX
Case_0022:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 0
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    ld hl, $ff3f
    REPT 1024
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-4096-00-idle", ROMX
Case_0023:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
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
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-4096-00-wave-read", ROMX
Case_0024:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
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
    wait_cycles 20
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
SECTION "ch3-bus-4096-00-wram-read", ROMX
Case_0025:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
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
    ld hl, $c800
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-4096-55-idle", ROMX
Case_0026:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    ld hl, $ff3f
    REPT 1024
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-4096-55-wave-read", ROMX
Case_0027:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    ld hl, $ff3f
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-4096-55-wram-read", ROMX
Case_0028:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    ld hl, $c800
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-4096-aa-idle", ROMX
Case_0029:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    ld hl, $ff3f
    REPT 1024
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-4096-aa-wave-read", ROMX
Case_002a:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    ld hl, $ff3f
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-4096-aa-wram-read", ROMX
Case_002b:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    ld hl, $c800
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-4096-ff-idle", ROMX
Case_002c:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
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
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-bus-4096-ff-wave-read", ROMX
Case_002d:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
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
    wait_cycles 20
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
SECTION "ch3-bus-4096-ff-wram-read", ROMX
Case_002e:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
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
    ld hl, $c800
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-no-rewrite-4096", ROMX
Case_002f:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    wait_cycles 4096
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    wait_cycles 20
    ld hl, $ff3f
    REPT 1024
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-never-00-idle", ROMX
Case_0030:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $40
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 65536
    ldh a, [$ff26]
    ld [wReadback + 1], a
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
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-never-00-wave-read", ROMX
Case_0031:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $40
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 65536
    ldh a, [$ff26]
    ld [wReadback + 1], a
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
    wait_cycles 20
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
SECTION "ch3-never-55-idle", ROMX
Case_0032:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $40
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 65536
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    ld hl, $ff3f
    REPT 1024
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-never-55-wave-read", ROMX
Case_0033:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $40
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 65536
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    ld hl, $ff3f
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-never-aa-idle", ROMX
Case_0034:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $40
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 65536
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    ld hl, $ff3f
    REPT 1024
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-never-aa-wave-read", ROMX
Case_0035:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $40
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 65536
    ldh a, [$ff26]
    ld [wReadback + 1], a
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    ld hl, $ff3f
    REPT 1024
        ld a, [hl]
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-never-ff-idle", ROMX
Case_0036:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $40
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 65536
    ldh a, [$ff26]
    ld [wReadback + 1], a
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
        nop
        nop
    ENDR
    ldh a, [$ff26]
    ld [wReadback + 2], a
    wait_cycles 20
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
SECTION "ch3-never-ff-wave-read", ROMX
Case_0037:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $40
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    wait_cycles 65536
    ldh a, [$ff26]
    ld [wReadback + 1], a
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
    wait_cycles 20
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
SECTION "ch3-dac-stop-active-00", ROMX
Case_0038:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    write_reg $ff1a, $00
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
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
SECTION "ch3-dac-off-after-expiry-00", ROMX
Case_0039:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
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
SECTION "ch3-dac-stop-active-55", ROMX
Case_003a:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    write_reg $ff1a, $00
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
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
SECTION "ch3-dac-off-after-expiry-55", ROMX
Case_003b:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $55
    write_reg $ff31, $55
    write_reg $ff32, $55
    write_reg $ff33, $55
    write_reg $ff34, $55
    write_reg $ff35, $55
    write_reg $ff36, $55
    write_reg $ff37, $55
    write_reg $ff38, $55
    write_reg $ff39, $55
    write_reg $ff3a, $55
    write_reg $ff3b, $55
    write_reg $ff3c, $55
    write_reg $ff3d, $55
    write_reg $ff3e, $55
    write_reg $ff3f, $55
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
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
SECTION "ch3-dac-stop-active-aa", ROMX
Case_003c:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    write_reg $ff1a, $00
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
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
SECTION "ch3-dac-off-after-expiry-aa", ROMX
Case_003d:
    ld a, $3c
    ld [$c800], a
    write_reg $ff25, $44
    write_reg $ff1a, $00
    write_reg $ff30, $aa
    write_reg $ff31, $aa
    write_reg $ff32, $aa
    write_reg $ff33, $aa
    write_reg $ff34, $aa
    write_reg $ff35, $aa
    write_reg $ff36, $aa
    write_reg $ff37, $aa
    write_reg $ff38, $aa
    write_reg $ff39, $aa
    write_reg $ff3a, $aa
    write_reg $ff3b, $aa
    write_reg $ff3c, $aa
    write_reg $ff3d, $aa
    write_reg $ff3e, $aa
    write_reg $ff3f, $aa
    write_reg $ff1b, $fc
    write_reg $ff1c, $20
    write_reg $ff1d, $01
    write_reg $ff1a, $80
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
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
SECTION "ch3-dac-stop-active-ff", ROMX
Case_003e:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    write_reg $ff1a, $00
    ldh a, [$ff26]
    ld [wReadback + 1], a
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
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
SECTION "ch3-dac-off-after-expiry-ff", ROMX
Case_003f:
    ld a, $3c
    ld [$c800], a
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
    wait_cycles 1024
    write_reg $ff1e, $c0
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 0], a
    ld bc, 4096
.expiry_poll:
    ldh a, [$ff26]
    bit 2, a
    jr z, .expired
    dec bc
    ld a, b
    or c
    jr nz, .expiry_poll
    ldh a, [$ff26]
.expired:
    ld [wReadback + 1], a
    write_reg $ff1a, $00
    wait_cycles 8192
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff1a, $80
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
SECTION "ch4-53-dac-off-p0-d0-r0", ROMX
Case_0040:
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
    wait_cycles 0
    write_reg $ff21, $00
    wait_cycles 0
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p0-d384-r0", ROMX
Case_0041:
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
    wait_cycles 0
    write_reg $ff21, $00
    wait_cycles 384
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p0-d1536-r0", ROMX
Case_0042:
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
    wait_cycles 0
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p384-d0-r0", ROMX
Case_0043:
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
    wait_cycles 384
    write_reg $ff21, $00
    wait_cycles 0
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p384-d384-r0", ROMX
Case_0044:
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
    wait_cycles 384
    write_reg $ff21, $00
    wait_cycles 384
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p384-d1536-r0", ROMX
Case_0045:
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
    wait_cycles 384
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p768-d0-r0", ROMX
Case_0046:
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
    wait_cycles 768
    write_reg $ff21, $00
    wait_cycles 0
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p768-d384-r0", ROMX
Case_0047:
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
    wait_cycles 768
    write_reg $ff21, $00
    wait_cycles 384
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p768-d1536-r0", ROMX
Case_0048:
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
    wait_cycles 768
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p1152-d0-r0", ROMX
Case_0049:
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
    wait_cycles 1152
    write_reg $ff21, $00
    wait_cycles 0
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p1152-d384-r0", ROMX
Case_004a:
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
    wait_cycles 1152
    write_reg $ff21, $00
    wait_cycles 384
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p1152-d1536-r0", ROMX
Case_004b:
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
    wait_cycles 1152
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p0-d1536-r384", ROMX
Case_004c:
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
    wait_cycles 0
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 384
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p0-d1536-r768", ROMX
Case_004d:
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
    wait_cycles 0
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 768
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p0-d1536-r1536", ROMX
Case_004e:
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
    wait_cycles 0
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 1536
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p768-d1536-r384", ROMX
Case_004f:
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
    wait_cycles 768
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 384
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p768-d1536-r768", ROMX
Case_0050:
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
    wait_cycles 768
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 768
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-dac-off-p768-d1536-r1536", ROMX
Case_0051:
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
    wait_cycles 768
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 1536
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-p0-d1536-r0", ROMX
Case_0052:
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
    wait_cycles 0
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-p384-d1536-r0", ROMX
Case_0053:
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
    wait_cycles 384
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-p768-d1536-r0", ROMX
Case_0054:
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
    wait_cycles 768
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-p1152-d1536-r0", ROMX
Case_0055:
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
    wait_cycles 1152
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-p0-d0-r0", ROMX
Case_0056:
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
    wait_cycles 0
    wait_cycles 20
    wait_cycles 0
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-p0-d384-r0", ROMX
Case_0057:
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
    wait_cycles 0
    wait_cycles 20
    wait_cycles 384
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-p0-d1536-r384", ROMX
Case_0058:
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
    wait_cycles 0
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 384
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-p0-d1536-r768", ROMX
Case_0059:
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
    wait_cycles 0
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 768
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-53-expired-dac-on-p0-d1536-r1536", ROMX
Case_005a:
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
    wait_cycles 0
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 1536
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p0-d0-r0", ROMX
Case_005b:
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
    wait_cycles 0
    write_reg $ff21, $00
    wait_cycles 0
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p0-d384-r0", ROMX
Case_005c:
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
    wait_cycles 0
    write_reg $ff21, $00
    wait_cycles 384
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p0-d1536-r0", ROMX
Case_005d:
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
    wait_cycles 0
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p384-d0-r0", ROMX
Case_005e:
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
    wait_cycles 384
    write_reg $ff21, $00
    wait_cycles 0
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p384-d384-r0", ROMX
Case_005f:
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
    wait_cycles 384
    write_reg $ff21, $00
    wait_cycles 384
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p384-d1536-r0", ROMX
Case_0060:
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
    wait_cycles 384
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p768-d0-r0", ROMX
Case_0061:
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
    wait_cycles 768
    write_reg $ff21, $00
    wait_cycles 0
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p768-d384-r0", ROMX
Case_0062:
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
    wait_cycles 768
    write_reg $ff21, $00
    wait_cycles 384
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p768-d1536-r0", ROMX
Case_0063:
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
    wait_cycles 768
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p1152-d0-r0", ROMX
Case_0064:
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
    wait_cycles 1152
    write_reg $ff21, $00
    wait_cycles 0
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p1152-d384-r0", ROMX
Case_0065:
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
    wait_cycles 1152
    write_reg $ff21, $00
    wait_cycles 384
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p1152-d1536-r0", ROMX
Case_0066:
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
    wait_cycles 1152
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p0-d1536-r384", ROMX
Case_0067:
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
    wait_cycles 0
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 384
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p0-d1536-r768", ROMX
Case_0068:
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
    wait_cycles 0
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 768
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p0-d1536-r1536", ROMX
Case_0069:
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
    wait_cycles 0
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 1536
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p768-d1536-r384", ROMX
Case_006a:
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
    wait_cycles 768
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 384
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p768-d1536-r768", ROMX
Case_006b:
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
    wait_cycles 768
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 768
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-dac-off-p768-d1536-r1536", ROMX
Case_006c:
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
    wait_cycles 768
    write_reg $ff21, $00
    wait_cycles 1536
    write_reg $ff21, $f0
    wait_cycles 1536
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-p0-d1536-r0", ROMX
Case_006d:
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
    wait_cycles 0
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-p384-d1536-r0", ROMX
Case_006e:
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
    wait_cycles 384
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-p768-d1536-r0", ROMX
Case_006f:
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
    wait_cycles 768
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-p1152-d1536-r0", ROMX
Case_0070:
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
    wait_cycles 1152
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-p0-d0-r0", ROMX
Case_0071:
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
    wait_cycles 0
    wait_cycles 20
    wait_cycles 0
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-p0-d384-r0", ROMX
Case_0072:
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
    wait_cycles 0
    wait_cycles 20
    wait_cycles 384
    wait_cycles 20
    wait_cycles 0
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-p0-d1536-r384", ROMX
Case_0073:
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
    wait_cycles 0
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 384
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-p0-d1536-r768", ROMX
Case_0074:
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
    wait_cycles 0
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 768
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
SECTION "ch4-5b-expired-dac-on-p0-d1536-r1536", ROMX
Case_0075:
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
    wait_cycles 0
    wait_cycles 20
    wait_cycles 1536
    wait_cycles 20
    wait_cycles 1536
    ldh a, [$ff26]
    ld [wReadback + 2], a
    write_reg $ff23, $80
    wait_cycles 64
    ldh a, [$ff26]
    ld [wReadback + 3], a
    wait_cycles 65536
    ld a, 4
    ld [wReadbackSize], a
    ret
