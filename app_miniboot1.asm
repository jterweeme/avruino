.define TIFR1 0x16
.define TCCR1B 0x81
.define TCNT1L 0x84
.define TCNT1H 0x85
.define UCSR0A 0xc0
.define UCSR0B 0xc1
.define UCSR0C 0xc2
.define UBRR0 0xc4
.define UDR0 0xc6

.DEVICE ATMega328p
.org 0x3f00

    ldi r24, 5
    sts TCCR1B, r24
    ldi r24, 2
    sts UCSR0A, r24
    ldi r24, 0x18
    sts UCSR0B, r24
    ldi r24, 6
    sts UCSR0C, r24
    ldi r24, 0x10
    sts UBRR0, r24
    in r24, 0x34
    out 0x34, r1
    sbrs r24, 1
    rcall label31
    ldi r24, 0x29
    rcall label15
    sbi 4, 5
    ldi r24, 6
    ldi r18, 0x30
    ldi r19, 0xfc
    ldi r25, 1
label3:
    sts TCNT1H, r19
    sts TCNT1L, r18
    out TIFR1, r25
label2:
    sbis 0x16, 0
    rjmp label2
    sbi 3, 5
    wdr
    subi r24, 1
    brne label3
    eor r13, r13
    inc r13
    ldi r26, 5
    mov r14, r26
    ldi r31, 0x11
    mov r15, r31
    rcall label26
    cpi r24, 0x41
    brne label4
    ldi r24, 1
.db 0xBE,0xD0
    ldi r24, 3
.db 0x24,0xC0
label4:
    cpi r24, 0x42
    brne label5
    ldi r24, 0x14
    rjmp label6
label5:
    cpi r24, 0x45
    brne label7
    ldi r24, 5
label6:
.db 0xB4,0xD0
.db 0x8A,0xC0
label7:
    cpi r24, 0x55
    brne label20
.db 0x92,0xD0
    mov r16, r24
    ldi r17, 0
    sts 0x0201, r17
    sts 0x0200, r16
.db 0x8B,0xD0
    ldi r25, 0
    mov r25, r24
    eor r24, r24
    or r24, r16
    or r25, r17
    add r24, r24
    adc r25, r25
    sts 0x0201, r25
    sts 0x0200, r24
    .db 0x73,0xC0
label20:
    cpi r24, 0x56
    brne label8
    ldi r24, 4
.db 0x99,0xD0
    ldi r24, 0
.db 0x71,0xD0
.db 0x6D,0xC0
label8:
    cpi r24, 0x64
    breq label9
.db 0x43,0xC0
label9:
.db 0x7C,0xD0
    lds r30, 0x0200
    lds r31, 0x0201
    ldi r24, 3
    sts 0x0057, r24
    spm
    ldi r28, 0
    ldi r29, 1
.db 0x69,0xD0
    st Y+, r24
    lds r24, 0x0202
    subi r24, 1
    sts 0x0202, r24
    and r24, r24
.db 0xB9,0xF7
.db 0x78,0xD0
label12:
    in r0, 0x37
    sbrc r0, 0
rjmp label12
    lds r20, 0x0200
    lds r21, 0x0201
    ldi r26, 0
    ldi r27, 1
label17:
    ld r18, X
    ldi r19, 0
    adiw r26, 1
    ld r24, X
    sbiw r26, 1
    ldi r25, 0
    mov r25, r24
    eor r24, r24
    or r24, r18
    or r25, r19
    adiw r26, 2
    movw r30, r20
    movw r0, r24
    sts 0x0057, r13
    spm
    eor r1, r1
    subi r20, 0xfe
    sbci r21, 0xff
    ldi r31, 1
    cpi r26, 0x80
    cpc r27, r31
    brne label17
    lds r30, 0x0200
    lds r31, 0x0201
    sts 0x0057, r14
    spm
label10:
    in r0, 0x37
    sbrc r0, 0
    rjmp label10
    sts 0x0057, r15
    spm
.db 0x27,0xC0
    cpi r24, 0x74
.db 0xB9,0xF4
.db 0x37,0xD0
.db 0x46,0xD0
    lds r30, 0x0200
    lds r31, 0x0201
    adiw r30, 1
    sts 0x0201, r31
    sts 0x0200, r30
    sbiw r30, 1
    lpm r30, Z
    mov r24, r30
.db 0x19,0xD0
    lds r24, 0x0202
    subi r24, 1
    sts 0x0202, r24
    and r24, r24
.db 0x61,0xF7
    rjmp label11
    cpi r24, 0x75
    brne label14 
.db 0x2E,0xD0
    ldi r24, 0x1e
.db 0x0C,0xD0
    ldi r24, 0x95
.db 0x0A,0xD0
    ldi r24, 0x0f
.db 0x96,0xCF
label14:
    cpi r24, 0x51
.db 0x11,0xF4
    ldi r24, 0x08
.db 0x19,0xD0
.db 0x23,0xD0
label11:
    ldi r24, 0x10
.db 0x01,0xD0
.db 0x63,0xCF
    mov r25, r24
    lds r24, 0x00c0
    sbrs r24, 5
.db 0xFC,0xCF
    sts UDR0, r25
    ret
label26:
    wdr
    lds r24, 0x00c0
    sbrs r24, 7
.db 0xFC,0xCF
    lds r24, UDR0
    ret
.db 0xF7,0xDF
.db 0xF6,0xDF
    sts 0x0202, r24
    rjmp label26
label15:
    ldi r30, 0x60
    ldi r31, 0
    ldi r25, 0x18
    st Z, r25
    st Z, r24
    ret
label31:
    ldi r24, 0
    rcall label15
label30:
    eor r30, r30
    eor r31, r31
    ijmp
.db 0xE7,0xDF
    cpi r24, 0x20
    breq label13
.db 0xF7,0xDF
label13:
    ldi r24, 0x14
.db 0xDA,0xCF
    push r17
    mov r17, r24
label1:
.db 0xDF,0xDF
    subi r17, 1
    brne label1
.db 0xF4,0xDF
    pop r17
    ret


