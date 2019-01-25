.define PINB 0x03
.define DDRB 0x04
.define TIFR1 0x16
.define MCUSR 0x34
.define SPMCSR 0x37
.define TCCR1B 0x81
.define TCNT1L 0x84
.define TCNT1H 0x85
.define UCSR0A 0xc0
.define UCSR0B 0xc1
.define UCSR0C 0xc2
.define UBRR0 0xc4
.define UDR0 0xc6

;.define UCSR0A 0xc8
;.define UCSR0B 0xc9
;.define UCSR0C 0xca
;.define UBRR0 0xcc
;.define UDR0 0xce

.define STK_GET_PARAMETER 0x41
.define STK_SET_DEVICE 0x42
.define STK_SET_DEVICE_EXT 0x45
.define STK_LOAD_ADDRESS 0x55
.define STK_UNIVERSAL 0x56
.define STK_READ_PAGE 0x74
.define STK_READ_SIGN 0x75
.define STK_SW_MAJOR 0x81
.define STK_SW_MINOR 0x82

.DEVICE ATMega328p
.org 0x3f00

    eor R1, R1          ;R1 = 0
    in  R24, MCUSR      ;r24 = MCUSR
    out MCUSR, R1       ;MCUSR = 0
    sbrs R24, 1         ;if (r24 & 1<<1)
    rcall a7fea
    ldi R24, 0x05       ;r24 = 5
    sts TCCR1B, R24

    ; UCSR0A = 1<<U2X0;
    ldi R24, 0x02
    sts UCSR0A, R24

    ; UCSR0B = 1<<TXEN0 | 1<<RXEN0;
    ldi R24, 0x18
    sts UCSR0B, R24
    
    ; UCSR0C = 1<<UCSZ00 | UCSZ01;
    ldi R24, 0x06
    sts UCSR0C, R24

    ; UBRR0 = 16;
    ldi R24, 0x10
    sts UBRR0, R24

    ; watchdogConfig(WATCHDOG_1S);
    ldi R24, 0x0e
    rcall watchdogConfig

    ; DDRB |= 1<<5;
    sbi DDRB, 5
    ldi R24, 0x06       ;r24 = 6
    ldi R18, 0x30       ;r18 = 0x30
    ldi R19, 0xfc       ;r19 = 0xfc
    ldi R25, 0x01       ;r25 = 1
a7e36:
    sts TCNT1H, R19
    sts TCNT1L, R18
    out TIFR1, R25
a7e40:
    sbis TIFR1, 0
    rjmp a7e40
    sbi PINB, 5
    wdr                 ;watchdog reset
a7e48:
    subi R24, 0x01      ;r24 -= 1;
    brne a7e36
    eor R12, R12        ;r12 = 0
a7e4e:
    eor R13, R13        ;r13 = 0
    eor R8, R8          ;r8 = 0
    inc R8              ;r8 = 1
    ldi R27, 0x05       ;r27 = 5
    mov R10, R27        ;r10 = r27
a7e58:
    ldi R26, 0x11       ;r26 = 0x11
    mov R9, R26         ;r9 = r26
    ldi R31, 0x03       ;r31 = 3
    mov R11, R31        ;r11 = r31
a7e60:
    rcall getch
a7e62:
    cpi R24, 0x41
a7e64:
    brne a7e7e
    rcall getch
    mov R16, R24    ;r16 = r24
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
    cpi R24, STK_SET_DEVICE_EXT
    brne a7e90
    ldi R24, 0x05
a7e8c:
    rcall a7fda
    rjmp a7f90
a7e90:
    cpi R24, STK_LOAD_ADDRESS
    brne a7eb2
a7e94:
    rcall getch
    mov R14, R24    ;r14 = r24
    eor R15, R15    ;r15 = 0
    rcall getch
a7e9c:
    mov R16, R24
    ldi R17, 0x00
    mov R17, R16    ;r17 = r16;
    eor R16, R16    ;r16 = 0;
    or R16, R14
    or R17, R15
    add R16, R16    ;r16 += r16;
    adc R17, R17    ;r17 += r17;
a7eac:
    rcall a7fca
    movw R12, R16
    rjmp a7f90
a7eb2:
    cpi R24, STK_UNIVERSAL
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
    rcall getch
    rcall getch
    mov R16, R24
a7eca:
    rcall getch
    ldi R24, 0x00
    cp  R12, R24
    ldi R24, 0x70
    cpc R13, R24
    brcc a7edc
    movw R30, R12
    out SPMCSR, R11
    spm
a7edc:
    ldi R28, 0x00
    ldi R29, 0x01
a7ee0:
    rcall getch
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
    out SPMCSR, R11
a7ef6:
    spm
a7ef8:
    rcall a7fca
a7efa:
    in R0, SPMCSR
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
    mov R25, R24    ;r25 = r24
    eor R24, R24    ;r24 = 0
    or R24, R18
    or R25, R19
    adiw R26, 0x02
    movw R30, R20
    movw R0, R24
    out SPMCSR, R8
    spm             ; store program memory
    eor R1, R1      ;r1 = 0
    subi R20, 0xfe
    sbci R21, 0xff
    ldi R31, 0x01
    cpi R26, 0x80
    cpc R27, R31
a7f30:
    brne a7f06
    movw R30, R12
    out SPMCSR, R10
a7f36:
    spm             ; store program memory
a7f38:
    in R0, SPMCSR
    sbrc R0, 0
a7f3c:
    rjmp a7f38
a7f3e:
    out SPMCSR, R9
a7f40:
    spm             ; store program memory
a7f42:
    rjmp a7f90
a7f44:
    cpi R24, STK_READ_PAGE   ;ZF = r24 == 0x74
a7f46:
    brne a7f74
a7f48:
    rcall getch
a7f4a:
    rcall getch
    mov R15, R24    ;r15 = r24
    rcall getch
    rcall a7fca
    movw R30, R12
    mov R14, R15    ;r14 = r15
a7f56:
    movw R16, R30
    subi R16, 0xff
    sbci R17, 0xff
    lpm R24, Z
a7f5e:
    rcall a7f96
a7f60:
    dec R14     ;r14--
a7f62:
    movw R30, R16
a7f64:
    brne a7f56
    sec 
    adc R12, R1
    adc R13, R1
    dec R15         ;r15--
    add R12, R15
    adc R13, R1
    rjmp a7f90
a7f74:
    cpi R24, STK_READ_SIGN   ;Zf = r24 == 0x75
    brne a7f86
    rcall a7fca
    ldi R24, 0x1e
    rcall a7f96
    ldi R24, 0x95
    rcall a7f96
    ldi R24, 0x0f
    rjmp a7e7a
a7f86:
    cpi R24, 0x51   ;ZF = r24 == 0x51
a7f88:
    brne a7f8e
    ldi R24, 0x08
    rcall watchdogConfig
a7f8e:
    rcall a7fca
a7f90:
    ldi R24, 0x10
    rcall a7f96
    rjmp a7e60
a7f96:
    mov R25, R24    ;r25 = r24

    ; while ((UCSR0A & 1<<UDRE0) == 0);
a7f98:
    lds R24, UCSR0A
    sbrs R24, 5
    rjmp a7f98
    
    ; UDR0 = ch;
    sts UDR0, R25
    ret

    ; while ((UCSR0A & 1<<RXC0) == 0);
getch:
    lds R24, UCSR0A
    sbrs R24, 7
    rjmp getch

    ; if (UCSR0A & 1<<FE0) wdr();
    lds R24, UCSR0A
    sbrc R24, 4
    rjmp a7fb8
    wdr

    ; return UDR0;
a7fb8:
    lds R24, UDR0
    ret
watchdogConfig:
    ldi R30, 0x60   ;r30 = 0x60
a7fc0:
    ldi R31, 0x00   ;r31 = 0
    ldi R25, 0x18   ;r25 = 0x18
    st Z, R25
    st Z, R24
    ret
a7fca:
    rcall getch
    cpi R24, 0x20   ;ZF = r24 == 0x20
    breq a7fd6
    ldi R24, 0x08
    rcall watchdogConfig
a7fd4:
    rjmp a7fd4
a7fd6:
    ldi R24, 0x14
a7fd8:
    rjmp a7f96
a7fda:
    push R17
    mov R17, R24    ;r17 = r24
a7fde:
    rcall getch
    subi R17, 0x01
    brne a7fde
    rcall a7fca
    pop R17
    ret 
a7fea:
    ldi R24, 0x00   ;r24 = 0
    rcall watchdogConfig
    eor R30, R30    ;r30 = 0
    eor R31, R31    ;r31 = 0
    ijmp    
    cpc R0, R4


