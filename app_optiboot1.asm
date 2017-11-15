.DEVICE ATMega328p
.org 0x3f00

    eor R1, R1
    in  R24, 0x34
    out $34, R1
    sbrs R24, 1
    rcall a7fea
    ldi R24, 0x05
    sts 0x0081, R24
    ldi R24, 0x02
    sts 0x00c0, R24
    ldi R24, 0x18
    sts 0x00c1, R24
    ldi R24, 0x06
    sts 0x00c2, R24
    ldi R24, 0x10
    sts 0x00c4, R24
    ldi R24, 0x0e
    rcall a7fbe
    sbi $04, 5
    ldi R24, 0x06
    ldi R18, 0x30
    ldi R19, 0xfc
    ldi R25, 0x01
a7e36:
    sts 0x0085, R19
    sts 0x0084, R18
    out $16, R25
a7e40:
    sbis $16, 0
    rjmp a7e40
    sbi $03, 5
    wdr 
a7e48:
    subi R24, 0x01
    brne a7e36
    eor R12, R12
a7e4e:
    eor R13, R13
    eor R8, R8
    inc R8
    ldi R27, 0x05
    mov R10, R27
a7e58:
    ldi R26, 0x11
    mov R9, R26
    ldi R31, 0x03
    mov R11, R31
a7e60:
    rcall a7fa6
a7e62:
    cpi R24, 0x41
a7e64:
    brne a7e7e
    rcall a7fa6
    mov R16, R24
    rcall a7fca
    cpi R16, 0x82
a7e6e:
    breq a7e74
    cpi R16, 0x81
    brne a7e78
a7e74:
    ldi R24, 0x04
a7e76:
    rjmp a7e7a
a7e78:
    ldi R24, 0x03
a7e7a:
    rcall a7f96
    rjmp a7f90
a7e7e:
    cpi R24, 0x42
    brne a7e86
    ldi R24, 0x14
    rjmp a7e8c
a7e86:
    cpi R24, 0x45
    brne a7e90
    ldi R24, 0x05
a7e8c:
    rcall a7fda
    rjmp a7f90
a7e90:
    cpi R24, 0x55
    brne a7eb2
a7e94:
    rcall a7fa6
    mov R14, R24
    eor R15, R15
    rcall a7fa6
a7e9c:
    mov R16, R24
    ldi R17, 0x00
    mov R17, R16
    eor R16, R16
    or R16, R14
    or R17, R15
    add R16, R16
    adc R17, R17
a7eac:
    rcall a7fca
    movw R12, R16
    rjmp a7f90
a7eb2:
    cpi R24, 0x56
    brne a7ebe
    ldi R24, 0x04
    rcall a7fda
a7eba:
    ldi R24, 0x00
    rjmp a7e7a
a7ebe:
    cpi R24, 0x64
a7ec0:
    breq a7ec4
a7ec2:
    rjmp a7f44
a7ec4:
    rcall a7fa6
    rcall a7fa6
    mov R16, R24
a7eca:
    rcall a7fa6
    ldi R24, 0x00
    cp  R12, R24
    ldi R24, 0x70
    cpc R13, R24
    brcc a7edc
    movw R30, R12
    out $37, R11
    spm
a7edc:
    ldi R28, 0x00
    ldi R29, 0x01
a7ee0:
    rcall a7fa6
    st Y+, R24
a7ee4:
    cp R16, R28
a7ee6:
    brne a7ee0
    ldi R31, 0x00
a7eea:
    cp R12, R31
    ldi R31, 0x70
    cpc R13, R31
    brcs a7ef8
a7ef2:
    movw R30, R12
a7ef4:
    out $37, R11
a7ef6:
    spm
a7ef8:
    rcall a7fca
a7efa:
    in R0, 0x37
    sbrc R0, 0
    rjmp a7efa
    movw R20, R12
    ldi R26, 0x00
    ldi R27, 0x01
a7f06:
    ld  R18, X
    ldi R19, 0x00
    adiw R26, 0x01
    ld R24, X
    sbiw R26, 0x01
    ldi R25, 0x00
a7f12:
    mov R25, R24
    eor R24, R24
    or R24, R18
    or R25, R19
    adiw R26, 0x02
    movw R30, R20
    movw R0, R24
    out $37, R8
    spm
    eor R1, R1
    subi R20, 0xfe
    sbci R21, 0xff
    ldi R31, 0x01
    cpi R26, 0x80
    cpc R27, R31
a7f30:
    brne a7f06
    movw R30, R12
    out $37, R10
a7f36:
    spm 
a7f38:
    in R0, 0x37
    sbrc R0, 0
a7f3c:
    rjmp a7f38
a7f3e:
    out $37, R9
a7f40:
    spm 
a7f42:
    rjmp a7f90
a7f44:
    cpi R24, 0x74
a7f46:
    brne a7f74
a7f48:
    rcall a7fa6
a7f4a:
    rcall a7fa6
    mov R15, R24
    rcall a7fa6
    rcall a7fca
    movw R30, R12
    mov R14, R15
a7f56:
    movw R16, R30
    subi R16, 0xff
    sbci R17, 0xff
    lpm R24, Z
a7f5e:
    rcall a7f96
a7f60:
    dec R14
a7f62:
    movw R30, R16
a7f64:
    brne a7f56
    sec 
    adc R12, R1
    adc R13, R1
    dec R15
    add R12, R15
    adc R13, R1
    rjmp a7f90
a7f74:
    cpi R24, 0x75
    brne a7f86
    rcall a7fca
    ldi R24, 0x1e
    rcall a7f96
    ldi R24, 0x95
    rcall a7f96
    ldi R24, 0x0f
    rjmp a7e7a
a7f86:
    cpi R24, 0x51
a7f88:
    brne a7f8e
    ldi R24, 0x08
    rcall a7fbe
a7f8e:
    rcall a7fca
a7f90:
    ldi R24, 0x10
    rcall a7f96
    rjmp a7e60
a7f96:
    mov R25, R24
a7f98:
    lds R24, 0x00c0
    sbrs R24, 5
    rjmp a7f98
    sts 0x00c6, R25
    ret
a7fa6:
    lds R24, 0x00c0
    sbrs R24, 7
    rjmp a7fa6
    lds R24, 0x00c0
    sbrc R24, 4
    rjmp a7fb8
    wdr
a7fb8:
    lds R24, 0x00c6
    ret
a7fbe:
    ldi R30, 0x60
a7fc0:
    ldi R31, 0x00
    ldi R25, 0x18
    st Z, R25
    st Z, R24
    ret 
a7fca:
    rcall a7fa6
    cpi R24, 0x20
    breq a7fd6
    ldi R24, 0x08
    rcall a7fbe
a7fd4:
    rjmp a7fd4
a7fd6:
    ldi R24, 0x14
a7fd8:
    rjmp a7f96
a7fda:
    push R17
    mov R17, R24
a7fde:
    rcall a7fa6
    subi R17, 0x01
    brne a7fde
    rcall a7fca
    pop R17
    ret 
a7fea:
    ldi R24, 0x00
    rcall a7fbe
    eor R30, R30
    eor R31, R31
    ijmp    
    cpc R0, R4


