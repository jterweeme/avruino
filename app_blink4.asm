.DEVICE ATmega328p

    jmp	0x0034
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    jmp	0x003e
    eor	R1, R1
    out	$3f, R1
    ser	R28
    ldi	R29, 0x08
    out	$3e, R29
    out	$3d, R28
    call 0x0040
    jmp	0x0067
    jmp	0x0000
    push R28
    push R29
    rcall alpha
alpha:
    in R28, 0x3d
    in R29, 0x3e
    ldi	R24, 0x24
    ldi	R25, 0x00
    ldi	R18, 0xfa
    movw R30, R24
    st Z, R18
echo:
    ldi	R24, 0x25
    ldi	R25, 0x00
    ldi	R18, 0x25
    ldi	R19, 0x00
    movw R30, R18
    ld R19, Z
    ldi R18, 0x20
    eor	R18, R19
    movw R30, R24
    st Z, R18
    std	Y+2, R1
    std	Y+1, R1
    rjmp charlie
delta:
    ldd	R24, Y+1
    ldd	R25, Y+2
    adiw R24, 0x01
    std	Y+2, R25
    std	Y+1, R24
charlie:
    ldd	R24, Y+1
    ldd	R25, Y+2
    ldi	R18, 0x01
    cpi	R24, 0xff
    ser	R31
    cpc	R25, R31
    brne bravo
    ldi	R18, 0x00
bravo:
    and	R18, R18
    brne delta
    rjmp echo
einde:
    cli
    rjmp einde


