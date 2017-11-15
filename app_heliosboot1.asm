.DEVICE ATmega32
.org 0x3800

a7000:
    rjmp a70ac
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a7a06
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a7136
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
    rjmp a70e2
    nop 
a70ac:
    eor R1, R1
    out $3f, R1
    ser R28
    ldi R29, 0x0a
    out $3e, R29
    out $3d, R28
    ldi R17, 0x01
a70ba:
    ldi R26, 0x00
    ldi R27, 0x01
    ldi R30, 0x26
    ldi R31, 0x7f
    rjmp a70c8
a70c4:
    lpm R0, Z+
    st X+, R0
a70c8:
    cpi R26, 0xa8
    cpc R27, R17
a70cc:
    brne a70c4
a70ce:
    ldi R17, 0x01
a70d0:
    ldi R26, 0xa8
a70d2:
    ldi R27, 0x01
a70d4:
    rjmp a70d8
a70d6:
    st X+, R1
a70d8:
    cpi R26, 0xbe
a70da:
    cpc R27, R17
a70dc:
    brne a70d6
a70de:
    rcall a77fe
    rjmp a7f22
a70e2:
    rjmp a7000
a70e4:  cli 
    sts 0x006f, R1
    sts 0x0081, R1
    sts 0x0085, R1
    sts 0x0084, R1
a70f6:
    ldi R24, 0x01
    out $35, R24
    out $35, R1
    cbi $08, 7
    sbi $0b, 5
    sbi $05, 0
    jmp 0x0000
    ret 
a7108:
    lds R18, 0x01b2
    lds R19, 0x01b3
a7110:
    subi R18, 0xff
a7112:
    sbci R19, 0xff
a7114:
    sts 0x01b3, R19
a7118:
    sts 0x01b2, R18
a711c:
    mov R25, R19
a711e:
    sbrs R19, 7
a7120:
    rjmp a7128
a7122:
    ldi R24, 0xfe
a7124:
    sub R24, R19
a7126:
    mov R25, R24
a7128:
    add R25, R25
a712a:
    cp R25, R18
a712c:
    brcc a7132
a712e:
    cbi $08, 7
a7130:
    ret 
a7132:
    sbi $08, 7
a7134:
    ret 
a7136:
    push R1
    push R0
a713a:
    in R0, 0x3f
a713c:
    push R0
a713e:
    eor R1, R1
a7140:
    push R18
    push R24
    push R25
    push R30
    push R31
    sts 0x0085, R1
    sts 0x0084, R1
    lds R24, 0x01a8
    lds R25, 0x01a9
    sbiw R24, 0x00
a715c:
    breq a716e
a715e:
    sbiw R24, 0x01
a7160:
    sts 0x01a9, R25
a7164:
    sts 0x01a8, R24
a7168:
    or R24, R25
a716a:
    brne a716e
a716c:
    sbi $0b, 5
a716e:
    lds R24, 0x01aa
a7172:
    lds R25, 0x01ab
a7176:
    sbiw R24, 0x00
a7178:
    breq a718a
a717a:
    sbiw R24, 0x01
a717c:
    sts 0x01ab, R25
a7180:
    sts 0x01aa, R24
a7184:
    or  R24, R25
a7186:
    brne a718a
a7188:
    sbi $05, 0
a718a:
    ldi R30, 0x00
a718c:
    ldi R31, 0x00
a718e:
    lpm R24, Z+
a7190:
    lpm R25, Z
a7192:
    subi R24, 0xff
a7194:
    sbci R25, 0xff
a7196:
    breq a71aa
a7198:
    lds R24, 0x01ac
a719c:
    lds R25, 0x01ad
a71a0:
    adiw R24, 0x01
a71a2:
    sts 0x01ad, R25
a71a6:
    sts 0x01ac, R24
a71aa:
    pop R31
    pop R30
    pop R25
    pop R24
    pop R18
    pop R0
a71b6:
    out $3f, R0
a71b8:
    pop R0
a71ba:
    pop R1
a71bc:
    reti    
a71be:
    ldi R24, 0x04
a71c0:
    sts 0x00e9, R24
a71c4:
    rjmp a71e0
a71c6:
    lds R24, 0x00e8
a71ca:
    andi R24, 0x7b
a71cc:
    sts 0x00e8, R24
a71d0:
    rjmp a71d8
a71d2:
    in  R24, 0x1e
a71d4:
    and R24, R24
a71d6:
    breq a71ec
a71d8:
    lds R24, 0x00e8
a71dc:
    sbrs R24, 2
a71de:
    rjmp a71d2
a71e0:
    lds R24, 0x00e8
a71e4:
    sbrs R24, 5
a71e6:
    rjmp a71c6
a71e8:
    lds R24, 0x00f1
a71ec:
    ret 
a71ee:
    mov R25, R24
a71f0:
    ldi R24, 0x03
a71f2:
    sts 0x00e9, R24
a71f6:
    lds R24, 0x00e8
a71fa:
    sbrc R24, 5
a71fc:
    rjmp a7218
a71fe:
    lds R24, 0x00e8
    andi R24, 0x7e
    sts 0x00e8, R24
    rjmp a7210
a720a:
    in R24, 0x1e
a720c:
    and R24, R24
    breq a722a
a7210:
    lds R24, 0x00e8
    sbrs R24, 0
    rjmp a720a
a7218:
    sts 0x00f1, R25
    cbi $0b, 5
a721e:
    ldi R24, 0x64
a7220:
    ldi R25, 0x00
a7222:
    sts 0x01a9, R25
a7226:
    sts 0x01a8, R24
a722a:
    ret 
a722c:
    push R4
    push R5
    push R6
    push R7
    push R8
    push R9
    push R10
    push R11
    push R12
    push R13
    push R14
    push R15
    push R16
    push R17
    push R28
    push R29
a724c:
    ldi R24, 0x04
a724e:
    sts 0x00e9, R24
a7252:
    lds R24, 0x00e8
a7256:
    sbrs R24, 2
a7258:
    rjmp a7708
a725a:
    cbi $05, 0
a725c:
    ldi R24, 0x64
a725e:
    ldi R25, 0x00
a7260:
    sts 0x01ab, R25
a7264:
    sts 0x01aa, R24
    rcall a71be
a726a:
    mov R17, R24
a726c:
    cpi R24, 0x45
a726e:
    brne a7290
a7270:
    ldi R24, 0x4c
    ldi R25, 0x1d
    sts 0x01ad, R25
    sts 0x01ac, R24
a727c:
    in R0, 0x37
a727e:
    sbrc R0, 0
a7280:
    rjmp a727c
a7282:
    sbic $1f, 1
a7284:
    rjmp a7282
a7286:
    ldi R24, 0x11
a7288:
    sts 0x0057, R24
a728c:
    spm 
a728e:
    rjmp a7296
a7290:
    cpi R24, 0x54
a7292:
    brne a729a
a7294:
    rcall a71be
a7296:
    ldi R24, 0x0d
a7298:
    rjmp a76b4
a729a:
    cpi R24, 0x4c
a729c:
    breq a7296
a729e:
    cpi R24, 0x50
a72a0:
    breq a7296
a72a2:
    cpi R24, 0x74
a72a4:
    brne a72ae
a72a6:
    ldi R24, 0x44
a72a8:
    rcall a71ee
a72aa:
    ldi R24, 0x00
a72ac:
    rjmp a76b4
a72ae:
    cpi R24, 0x61
a72b0:
    brne a72b6
a72b2:
    ldi R24, 0x59
a72b4:
    rjmp a76b4
a72b6:
    cpi R24, 0x41
a72b8:
    brne a72e6
a72ba:
    rcall a71be
a72bc:
    mov R17, R24
a72be:
    rcall a71be
a72c0:
    ldi R25, 0x00
a72c2:
    add R24, R24
a72c4:
    adc R25, R25
a72c6:
    eor R26, R26
a72c8:
    sbrc R25, 7
a72ca:
    com R26
a72cc:
    mov R27, R26
a72ce:
    mov R19, R17
a72d0:
    add R19, R19
a72d2:
    ldi R18, 0x00
a72d4:
    eor R20, R20
a72d6:
    sbrc R19, 7
a72d8:
    com R20
a72da: 
    mov R21, R20
a72dc:
    or  R24, R18
    or R25, R19
    or R26, R20
    or R27, R21
a72e4:
    rjmp a7656
a72e6:
    cpi R24, 0x70
a72e8:
    brne a72ee
a72ea:
    ldi R24, 0x53
a72ec:
    rjmp a76b4
a72ee:
    cpi R24, 0x53
a72f0:
    brne a7304
a72f2:
    ldi R28, 0x00
a72f4:
    ldi R29, 0x01
a72f6:
    ld R24, Y+
a72f8:
    rcall a71ee
a72fa:
    ldi R18, 0x01
a72fc:
    cpi R28, 0x07
a72fe:
    cpc R29, R18
a7300:
    brne a72f6
a7302:
    rjmp a76b6
a7304:
    cpi R24, 0x56
a7306:
    brne a7310
a7308:
    ldi R24, 0x31
a730a:
    rcall a71ee
a730c:
    ldi R24, 0x30
a730e:
    rjmp a76b4
a7310:
    cpi R24, 0x73
a7312:
    brne a7320
    ldi R24, 0x87
    rcall a71ee
    ldi R24, 0x95
    rcall a71ee
    ldi R24, 0x1e
a731e:
    rjmp a76b4
a7320:
    cpi R24, 0x65
a7322:
    brne a7352
a7324:
    ldi R30, 0x00
    ldi R31, 0x00
    ldi R25, 0x03
    ldi R24, 0x05
a732c:
    sts 0x0057, R25
    spm 
a7332:
    in R0, 0x37
    sbrc R0, 0
    rjmp a7332
    sts 0x0057, R24
    spm 
a733e:
    in R0, 0x37
a7340:
    sbrc R0, 0
a7342:
    rjmp a733e
a7344:
    subi    R30, 0x80
a7346:
    sbci    R31, 0xff
a7348:
    ldi R26, 0x70
a734a:
    cpi R30, 0x00
a734c:
    cpc R31, R26
a734e:
    brne    a732c
a7350:
    rjmp    a7296
a7352:
    cpi R24, 0x72
a7354:
    brne    a7364
a7356:
    ldi R30, 0x01
a7358:
    ldi R31, 0x00
a735a:
    ldi R24, 0x09
a735c:
    sts 0x0057, R24
a7360:
    lpm R24, Z
a7362:
    rjmp    a76b4
a7364:
    cpi R24, 0x46
a7366:
    brne    a7376
a7368:
    ldi R30, 0x00
    ldi R31, 0x00
    ldi R24, 0x09
    sts 0x0057, R24
a7372:
    lpm R24, Z
a7374:
    rjmp a76b4
a7376:
    cpi R24, 0x4e
a7378:
    brne a7388
a737a:
    ldi R30, 0x03
    ldi R31, 0x00
    ldi R24, 0x09
    sts 0x0057, R24
a7384:
    lpm R24, Z
a7386:
    rjmp a76b4
a7388:
    cpi R24, 0x51
a738a:
    brne a739a
a738c:
    ldi R30, 0x02
    ldi R31, 0x00
    ldi R24, 0x09
a7392:
    sts 0x0057, R24
a7396:
    lpm R24, Z
a7398:
    rjmp a76b4
a739a:
    cpi R24, 0x62
a739c:
    brne a73aa
a739e:
    ldi R24, 0x59
a73a0:
    rcall a71ee
a73a2:
    ldi R24, 0x00
a73a4:
    rcall a71ee
a73a6:
    ldi R24, 0x80
a73a8:
    rjmp a76b4
a73aa:
    cpi R24, 0x42
a73ac:
    breq a73b4
a73ae:
    cpi R24, 0x67
a73b0:
    breq a73b4
a73b2:
    rjmp a757e
a73b4:
    sts 0x01ad, R1
    sts 0x01ac, R1
a73bc:
  rcall   a71be
a73be:
  mov R16, R24
a73c0:
  rcall   a71be
a73c2:
  mov R15, R24
a73c4:
  rcall   a71be
a73c6:
  mov R6, R24
a73c8:
  subi    R24, 0x45
a73ca:
  cpi R24, 0x02
a73cc:
  brcs    a73d0
a73ce:
  rjmp    a76b2
a73d0:
  mov R25, R16
a73d2:
  ldi R24, 0x00
a73d4:
    mov R28, R15
a73d6:
    ldi R29, 0x00
    or R28, R24
    or R29, R25
a73dc:
    sts 0x006f, R1
a73e0:
    cpi R17, 0x67
a73e2:
    breq a73e6
a73e4:
    rjmp a747c
a73e6:
    ldi R24, 0x11
a73e8:
    sts 0x0057, R24
a73ec: 
    spm 
a73ee:
    eor R13, R13
a73f0:
    eor R12, R12
a73f2:
    inc R12
a73f4:
    rjmp a7474
a73f6:
    lds R14, 0x01ae
    lds R15, 0x01af
    lds R16, 0x01b0
    lds R17, 0x01b1
    ldi R27, 0x46
a7408:
    cp R6, R27
a740a:
    brne a7442
a740c:
    mov R30, R13
a740e:
    ldi R31, 0x00
a7410:
    or R30, R14
a7412:
  or  R31, R15
a7414:
  lpm R30, Z
a7416:
  mov R24, R30
a7418:
  rcall   a71ee
a741a:
  and R13, R13
a741c:
    breq a743e
a741e:
    ldi R24, 0x02
    ldi R25, 0x00
    ldi R26, 0x00
    ldi R27, 0x00
    add R14, R24
    adc R15, R25
    adc R16, R26
    adc R17, R27
a742e:
    sts 0x01ae, R14
    sts 0x01af, R15
    sts 0x01b0, R16
    sts 0x01b1, R17
a743e:
    eor R13, R12
a7440:
    rjmp a7472
a7442:
    movw R26, R16
a7444:
    movw R24, R14
a7446:
    lsr R27
a7448:
    ror R26
a744a:
    ror R25
a744c:
    ror R24
a744e:
    rcall a7ef6
a7450:
    rcall a71ee
a7452:
    ldi R24, 0x02
    ldi R25, 0x00
    ldi R26, 0x00
    ldi R27, 0x00
a745a:
    add R14, R24
    adc R15, R25
    adc R16, R26
    adc R17, R27
a7462:
    sts 0x01ae, R14
    sts 0x01af, R15
    sts 0x01b0, R16
    sts 0x01b1, R17
a7472:
    sbiw R28, 0x01
a7474:
    sbiw R28, 0x00
a7476:
    breq a747a
a7478:
    rjmp a73f6
a747a:
    rjmp a7576
a747c:
    lds R8, 0x01ae
    lds R9, 0x01af
    lds R10, 0x01b0
    lds R11, 0x01b1
    ldi R25, 0x46
a748e:
    cp  R6, R25
a7490:
    breq    a7494
a7492:
    rjmp    a754e
a7494:
    ldi R24, 0x03
a7496:
    movw    R30, R8
a7498:
    sts 0x0057, R24
a749c:
    spm 
a749e:
    in  R0, 0x37
a74a0:
    sbrc    R0, 0
a74a2:
    rjmp    a749e
a74a4:
    rjmp    a754e
a74a6:
    ldi R31, 0x46
a74a8:
    cp  R6, R31
a74aa:
    brne    a7504
a74ac:
    and R7, R7
a74ae:
    breq    a74fc
a74b0:
    lds R14, 0x01ae
    lds R15, 0x01af
    lds R16, 0x01b0
    lds R17, 0x01b1
a74c0:
    rcall   a71be
a74c2:
    mov R13, R24
a74c4:
    eor R12, R12
a74c6:
    mov R24, R5
a74c8:
    ldi R25, 0x00
    or R24, R12
    or R25, R13
    movw R30, R14
a74d0:
    movw R0, R24
a74d2:
    sts 0x0057, R4
a74d6:
    spm 
a74d8:
    eor R1, R1
a74da:
    ldi R24, 0x02
    ldi R25, 0x00
    ldi R26, 0x00
    ldi R27, 0x00
a74e2:
    add R14, R24
    adc R15, R25
    adc R16, R26
    adc R17, R27
a74ea:
    sts 0x01ae, R14
    sts 0x01af, R15
    sts 0x01b0, R16
    sts 0x01b1, R17
    rjmp a7500
a74fc:
    rcall a71be
    mov R5, R24
a7500:
    eor R7, R4
    rjmp a754a
a7504:
    lds R14, 0x01ae
    lds R15, 0x01af
    lds R16, 0x01b0
    lds R17, 0x01b1
a7514:
    lsr R17
a7516:
    ror R16
    ror R15
    ror R14
a751c:
    rcall   a71be
a751e:
    mov R22, R24
a7520:
    movw    R24, R14
a7522:
    rcall   a7f06
a7524:
    lds R24, 0x01ae
    lds R25, 0x01af
    lds R26, 0x01b0
    lds R27, 0x01b1
a7534:
    adiw    R24, 0x02
a7536:
    adc R26, R1
a7538:  adc R27, R1
    sts 0x01ae, R24
    sts 0x01af, R25
    sts 0x01b0, R26
    sts 0x01b1, R27
a754a:
    sbiw R28, 0x01
a754c:
    rjmp a7556
a754e:
    eor R5, R5
    eor R7, R7
    eor R4, R4
    inc R4
a7556:
    sbiw R28, 0x00
a7558:
    breq a755c
a755a:
    rjmp a74a6
a755c:
    ldi R25, 0x46
a755e:
    cp R6, R25
a7560:
    brne a7572
a7562:
    ldi R24, 0x05
a7564:
    movw R30, R8
a7566:
    sts 0x0057, R24
a756a:
    spm 
a756c:
    in R0, 0x37
a756e:
    sbrc R0, 0
a7570:
    rjmp a756c
a7572:
    ldi R24, 0x0d
a7574:
    rcall a71ee
a7576:
    ldi R24, 0x02
a7578:
    sts 0x006f, R24
a757c:
    rjmp a76b6
a757e:
    cpi R24, 0x43
a7580:
    brne a759e
a7582:
    lds R16, 0x01ae
a7586:
    lds R17, 0x01af
a758a:
    rcall a71be
a758c:
    ldi R25, 0x00
a758e:
    ldi R18, 0x01
    movw R30, R16
    movw R0, R24
    sts 0x0057, R18
    spm 
a759a:
    eor R1, R1
a759c:
    rjmp a7296
a759e:
    cpi R24, 0x63
a75a0:
    brne a75e8
a75a2:
    lds R14, 0x01ae
    lds R15, 0x01af
    lds R16, 0x01b0
    lds R17, 0x01b1
a75b2:
    rcall a71be
a75b4:
    movw R30, R14
    ori R30, 0x01
    ldi R25, 0x00
    ldi R18, 0x01
    movw R0, R24
a75be:
    sts 0x0057, R18
a75c2:
    spm 
a75c4:
    eor R1, R1
a75c6:
    ldi R24, 0x02
    ldi R25, 0x00
    ldi R26, 0x00
    ldi R27, 0x00
a75ce:
    add R14, R24
    adc R15, R25
    adc R16, R26
    adc R17, R27
a75d6:
    sts 0x01ae, R14
    sts 0x01af, R15
    sts 0x01b0, R16
    sts 0x01b1, R17
a75e6:
    rjmp a7296
a75e8:
    cpi R24, 0x6d
a75ea:
    brne a7604
    lds R30, 0x01ae
    lds R31, 0x01af
    ldi R24, 0x05
a75f6:
    sts 0x0057, R24
a75fa:
    spm 
a75fc:
    in R0, 0x37
a75fe:
    sbrc R0, 0
a7600:
    rjmp a75fc
a7602:
    rjmp a7296
a7604:
    cpi R24, 0x52
a7606:  brne    a761c
    lds R30, 0x01ae
    lds R31, 0x01af
    lpm R16, Z+
    lpm R17, Z
a7614:
    mov R24, R17
a7616:
    rcall a71ee
a7618:
    mov R24, R16
a761a:
    rjmp a76b4
a761c:
    cpi R24, 0x44
a761e:
    brne a7668
a7620:
    lds R14, 0x01ae
    lds R15, 0x01af
    lds R16, 0x01b0
    lds R17, 0x01b1
a7630:
    lsr R17
a7632:
    ror R16
    ror R15
    ror R14
a7638:
    rcall a71be
a763a:
    mov R22, R24
a763c:
    movw R24, R14
a763e:
    rcall a7f06
    lds R24, 0x01ae
    lds R25, 0x01af
    lds R26, 0x01b0
    lds R27, 0x01b1
    adiw R24, 0x02
a7652:
    adc R26, R1
a7654:
    adc R27, R1
a7656:
    sts 0x01ae, R24
    sts 0x01af, R25
    sts 0x01b0, R26
    sts 0x01b1, R27
a7666:
    rjmp a7296
a7668:
    cpi R24, 0x64
a766a:
    brne a76ae
a766c:
    lds R14, 0x01ae
    lds R15, 0x01af
    lds R16, 0x01b0
    lds R17, 0x01b1
a767c:
    movw R26, R16
a767e:
    movw R24, R14
    lsr R27
    ror R26
    ror R25
    ror R24
a7688:
    rcall a7ef6
a768a:
    rcall a71ee
a768c:
    ldi R24, 0x02
    ldi R25, 0x00
    ldi R26, 0x00
    ldi R27, 0x00
a7694:
    add R14, R24
    adc R15, R25
    adc R16, R26
    adc R17, R27
a769c:
    sts 0x01ae, R14
    sts 0x01af, R15
    sts 0x01b0, R16
    sts 0x01b1, R17
a76ac:
    rjmp a76b6
a76ae:
    cpi R24, 0x1b
a76b0:
    breq a76b6
a76b2:
    ldi R24, 0x3f
a76b4:
    rcall a71ee
a76b6:
    ldi R24, 0x03
a76b8:
    sts 0x00e9, R24
    lds R25, 0x00e8
    lds R24, 0x00e8
    andi    R24, 0x7e
a76c6:
    sts 0x00e8, R24
a76ca:
    sbrs R25, 5
a76cc:
    rjmp a76d6
a76ce:
    rjmp a76f0
a76d0:
    in R24, 0x1e
a76d2:
    and R24, R24
a76d4:
    breq a7708
a76d6:  lds R24, 0x00e8
a76da:  sbrs    R24, 0
a76dc:  rjmp    a76d0
a76de:  lds R24, 0x00e8
a76e2:  andi    R24, 0x7e
a76e4:  sts 0x00e8, R24
a76e8:  rjmp    a76f0
a76ea:  in  R24, 0x1e
a76ec:  and R24, R24
a76ee:  breq    a7708
a76f0:  lds R24, 0x00e8
a76f4:  sbrs    R24, 0
a76f6:  rjmp    a76ea
a76f8:  ldi R24, 0x04
a76fa:  sts 0x00e9, R24
a76fe:  lds R24, 0x00e8
a7702:  andi    R24, 0x7b
a7704:
    sts 0x00e8, R24
a7708:
    pop R29
    pop R28
    pop R17
    pop R16
    pop R15
    pop R14
    pop R13
    pop R12
    pop R11
    pop R10
    pop R9
    pop R8
    pop R7
    pop R6
    pop R5
    pop R4
a7728:  ret 
a772a:  lds R25, 0x01b6
a772e:  mov R24, R25
a7730:  andi    R24, 0x7f
a7732:  cpi R24, 0x21
a7734:  brne    a7788
a7736:  lds R24, 0x01b7
a773a:  cpi R24, 0x20
a773c:  breq    a7766
a773e:  cpi R24, 0x21
a7740:  brne    a7788
a7742:  cpi R25, 0xa1
a7744:  brne    a7788
a7746:  lds R24, 0x00e8
a774a:  andi    R24, 0xf7
a774c:  sts 0x00e8, R24
a7750:
    ldi R24, 0x0d
    ldi R25, 0x01
    ldi R22, 0x07
    ldi R23, 0x00
a7758:  rcall   a7b64
a775a:  lds R24, 0x00e8
a775e:  andi    R24, 0x7b
a7760:  sts 0x00e8, R24
a7764:  ret 
a7766:  cpi R25, 0x21
a7768:  brne    a7788
a776a:  lds R24, 0x00e8
a776e:  andi    R24, 0xf7
a7770:  sts 0x00e8, R24
a7774:
    ldi R24, 0x0d
    ldi R25, 0x01
    ldi R22, 0x07
    ldi R23, 0x00
a777c:  rcall   a7c2c
a777e:  lds R24, 0x00e8
a7782:  andi    R24, 0x7e
a7784:  sts 0x00e8, R24
a7788:  ret 
a778a:
    ldi R24, 0x02
    ldi R22, 0xc1
    ldi R20, 0x02
a7790:
    rcall a78f4
a7792:
    ldi R24, 0x03
    ldi R22, 0x81
    ldi R20, 0x12
a7798:
    rcall a78f4
a779a:
    ldi R24, 0x04
    ldi R22, 0x80
    ldi R20, 0x12
a77a0:  rjmp    a78f4
a77a2:  in  R24, 0x34
a77a4:  andi    R24, 0xf7
a77a6:  out $34, R24
a77a8:  ldi R24, 0x18
a77aa:  in  R0, 0x3f
a77ac:  cli 
a77ae:  sts 0x0060, R24
a77b2:  sts 0x0060, R1
a77b6:  out $3f, R0
a77b8:
    ldi R18, 0x80
    ldi R24, 0x00
    ldi R25, 0x00
    in  R0, 0x3f
a77c0:
    cli 
    sts 0x0061, R18
    sts 0x0061, R24
    out $3f, R0
    ldi R24, 0x01
    out $35, R24
    ldi R25, 0x02
    out $35, R25
a77d4:
    sbi $07, 7
    sbi $04, 0
    sbi $0a, 5
a77da:
    ldi R30, 0x61
a77dc:
    ldi R31, 0x00
a77de:
    st Z, R18
a77e0:
    st Z, R1
a77e2:
    cbi $08, 7
    sbi $0b, 5
    sbi $05, 0
    sts 0x0089, R1
    ldi R24, 0xfa
    sts 0x0088, R24
    sts 0x006f, R25
    ldi R24, 0x03
    sts 0x0081, R24
a77fc:
    rjmp a79d6
a77fe:
    lds R20, 0x0800
a7802:
    lds R21, 0x0801
    sts 0x0801, R1
    sts 0x0800, R1
    in  R18, 0x34
a7810:  out $34, R1
a7812:  ldi R24, 0x18
a7814:  in  R0, 0x3f
a7816:  cli 
a7818:  sts 0x0060, R24
a781c:  sts 0x0060, R1
a7820:  out $3f, R0
a7822:  mov R24, R18
a7824:  ldi R25, 0x00
a7826:  movw R30, R24
a7828:  andi R30, 0x02
a782a:  andi R31, 0x00
a782c:  sbrc R18, 1
a782e:  rjmp a7858
a7830:  cpi R18, 0x01
a7832:  brne a783a
a7834:  lpm R24, Z+
a7836:  lpm R25, Z
a7838:  rjmp a7850
a783a:  cpi R18, 0x08
a783c:  brne    a7858
a783e:  lds R24, 0x0109
a7842:  lds R25, 0x010a
a7846:  cp  R20, R24
a7848:
    cpc R21, R25
    breq a7858
    lpm R24, Z+
    lpm R25, Z
a7850:
    subi R24, 0xff
a7852:  sbci R25, 0xff
a7854:  breq a7858
a7856:  rcall a70e4
a7858:  rcall a77a2
a785a:  sei 
a785c:  sts 0x01ad, R1
a7860:  sts 0x01ac, R1
a7864:  rjmp a787e
a7866:  rcall a722c
a7868:  rcall a7ed2
a786a:  lds R24, 0x01ac
a786e:  lds R25, 0x01ad
a7872:  subi R24, 0x41
a7874:  sbci R25, 0x1f
a7876:  brcs a787c
a7878:  sts 0x0114, R1
a787c:  rcall a7108
a787e:  lds R24, 0x0114
a7882:  and R24, R24
a7884:  brne a7866
a7886:  lds R24, 0x00e0
a788a:  ori R24, 0x01
a788c:  sts 0x00e0, R24
a7890:
    rcall   a70e4
a7892:
    ldi R24, 0x00
a7894:
    ldi R25, 0x00
a7896:
    ret 
a7898:
    movw  R30, R20
a789a:
    cpi R25, 0x02
a789c:
    breq a78b0
a789e:
    cpi R25, 0x03
a78a0:
    breq a78ba
a78a2:
    cpi R25, 0x01
a78a4:
    brne a78e4
a78a6:
    ldi R24, 0x15
    ldi R25, 0x01
    ldi R18, 0x12
    ldi R19, 0x00
a78ae:
    rjmp a78ec
a78b0:
    ldi R24, 0x27
    ldi R25, 0x01
    ldi R18, 0x3e
    ldi R19, 0x00
a78b8:
    rjmp a78ec
a78ba:
    and R24, R24
a78bc:
    brne a78c8
a78be:
    ldi R24, 0x65
    ldi R25, 0x01
    ldi R18, 0x04
    ldi R19, 0x00
a78c6:
    rjmp    a78ec
a78c8:
    cpi R24, 0x01
a78ca:
    brne    a78d6
a78cc:
    ldi R24, 0x69
    ldi R25, 0x01
    ldi R18, 0x22
    ldi R19, 0x00
a78d4:  rjmp    a78ec
a78d6:  cpi R24, 0x02
a78d8:  brne    a78e4
a78da:
    ldi R24, 0x8d
    ldi R25, 0x01
    ldi R18, 0x18
    ldi R19, 0x00
a78e2:
    rjmp a78ec
a78e4:
    ldi R24, 0x00
    ldi R25, 0x00
    ldi R18, 0x00
    ldi R19, 0x00
a78ec:
    std Z+1, R25
a78ee:
    st Z, R24
a78f0:
    movw    R24, R18
a78f2:
    ret 
a78f4:
    sts 0x00e9, R24
a78f8:
    lds R24, 0x00eb
a78fc:
    ori R24, 0x01
    sts 0x00eb, R24
    sts 0x00ed, R1
    sts 0x00ec, R22
    sts 0x00ed, R20
a790e:
    lds R24, 0x00ee
    adc R24, R24
    eor R24, R24
    adc R24, R24
a7918:
  ret 
a791a:
  lds R24, 0x01b6
a791e:
  and R24, R24
a7920:
  brge    a7944
a7922:
  rjmp    a792a
a7924:
  in  R24, 0x1e
a7926:
  and R24, R24
a7928:
  breq    a7956
a792a:
  lds R24, 0x00e8
a792e:
  sbrs    R24, 2
a7930:
  rjmp    a7924
a7932:
  lds R24, 0x00e8
a7936:
  andi    R24, 0x7b
a7938:
  sts 0x00e8, R24
a793c:
  ret 
a793e:
  in  R24, 0x1e
a7940:
  and R24, R24
a7942:
  breq    a7956
a7944:
  lds R24, 0x00e8
a7948:
  sbrs    R24, 0
a794a:
  rjmp    a793e
a794c:
  lds R24, 0x00e8
a7950:
  andi    R24, 0x7e
a7952:
  sts 0x00e8, R24
a7956:
  ret 
a7958:
    push R14
    push R15
    push R16
    push R17
a7960:
    rcall a79ec
a7962:
    rcall a79fc
    ldi R16, 0xd8
    ldi R17, 0x00
    movw R30, R16
    ld  R24, Z
    andi R24, 0x7f
    st Z, R24
    ld R24, Z
    ori R24, 0x80
    st Z, R24
    ld R24, Z
    andi R24, 0xdf
    st Z, R24
a797c:
    out $29, R1
a797e:
    out $1e, R1
a7980:
    sts 0x01b4, R1
a7984:
    ldi R24, 0xe0
a7986:
    mov R14, R24
a7988:
    mov R15, R1
a798a:
    movw    R30, R14
a798c:  ld  R24, Z
a798e:  andi    R24, 0xfb
a7990:  st  Z, R24
a7992:  movw    R30, R16
a7994:  ld  R24, Z
a7996:  ori R24, 0x01
a7998:  st  Z, R24
a799a:
    ldi R24, 0x00
    ldi R22, 0x00
    ldi R20, 0x02
a79a0:
    rcall a78f4
a79a2:
    ldi R30, 0xe1
    ldi R31, 0x00
    ld  R24, Z
a79a8:  andi    R24, 0xfe
a79aa:  st  Z, R24
a79ac:  ldi R30, 0xe2
a79ae:  ldi R31, 0x00
a79b0:  ld  R24, Z
a79b2:  ori R24, 0x01
a79b4:  st  Z, R24
a79b6:  ld  R24, Z
a79b8:  ori R24, 0x08
a79ba:  st  Z, R24
a79bc:  movw    R30, R14
a79be:  ld  R24, Z
a79c0:  andi    R24, 0xfe
a79c2:  st  Z, R24
a79c4:  movw    R30, R16
a79c6:  ld  R24, Z
a79c8:  ori R24, 0x10
a79ca:  st  Z, R24
a79cc:
    pop R17
    pop R16
    pop R15
    pop R14
a79d4:
    ret 
a79d6:
    ldi R30, 0xd7
a79d8:
    ldi R31, 0x00
a79da:  ld  R24, Z
a79dc:  ori R24, 0x01
a79de:  st  Z, R24
a79e0:  ldi R24, 0x4a
a79e2:  out $32, R24
a79e4:  ldi R24, 0x01
a79e6:  sts 0x01b5, R24
a79ea:  rjmp    a7958
a79ec:  ldi R30, 0xd8
a79ee:  ldi R31, 0x00
a79f0:  ld  R24, Z
a79f2:  andi    R24, 0xfe
a79f4:  st  Z, R24
a79f6:  sts 0x00e2, R1
a79fa:  ret 
a79fc:  sts 0x00da, R1
a7a00:  sts 0x00e1, R1
a7a04:  ret 
a7a06:
    push R1
a7a08:
    push R0
a7a0a:
    in R0, 0x3f
a7a0c:
    push R0
a7a0e:
    eor R1, R1
a7a10:
    push R18
    push R19
    push R20
    push R21
    push R22
    push R23
    push R24
    push R25
    push R26
    push R27
    push R30
    push R31
a7a28:
    lds R24, 0x00da
a7a2c:
    sbrs    R24, 0
a7a2e:
    rjmp    a7a64
a7a30:
    lds R24, 0x00d8
a7a34:
    sbrs    R24, 0
a7a36:
    rjmp    a7a64
a7a38:
    lds R24, 0x00da
a7a3c:
    andi    R24, 0xfe
a7a3e:
    sts 0x00da, R24
a7a42:
    lds R24, 0x00d9
a7a46:  sbrs    R24, 0
a7a48:  rjmp    a7a5e
a7a4a:  out $29, R1
a7a4c:  ldi R24, 0x02
a7a4e:  out $29, R24
a7a50:  in  R0, 0x29
a7a52:  sbrs    R0, 0
a7a54:  rjmp    a7a50
a7a56:  ldi R24, 0x01
a7a58:  out $1e, R24
a7a5a:  rcall   a7ed0
a7a5c:  rjmp    a7a64
a7a5e:  out $29, R1
a7a60:  out $1e, R1
a7a62:  rcall   a7ed0
a7a64:  lds R24, 0x00e1
a7a68:  sbrs    R24, 0
a7a6a:  rjmp    a7a9a
a7a6c:  lds R24, 0x00e2
a7a70:  sbrs    R24, 0
a7a72:  rjmp    a7a9a
a7a74:  lds R24, 0x00e2
a7a78:  andi    R24, 0xfe
a7a7a:  sts 0x00e2, R24
a7a7e:
    lds R24, 0x00e2
    ori R24, 0x10
    sts 0x00e2, R24
    lds R24, 0x00d8
    ori R24, 0x20
    sts 0x00d8, R24
    out $29, R1
    ldi R24, 0x05
a7a96:  out $1e, R24
a7a98:  rcall   a7ed0
a7a9a:  lds R24, 0x00e1
a7a9e:  sbrs    R24, 4
a7aa0:  rjmp    a7af8
a7aa2:  lds R24, 0x00e2
a7aa6:  sbrs    R24, 4
a7aa8:  rjmp    a7af8
a7aaa:  out $29, R1
a7aac:  ldi R24, 0x02
a7aae:  out $29, R24
a7ab0:  in  R0, 0x29
a7ab2:  sbrs    R0, 0
a7ab4:
    rjmp a7ab0
    lds R24, 0x00d8
    andi R24, 0xdf
    sts 0x00d8, R24
    lds R24, 0x00e1
    andi R24, 0xef
    sts 0x00e1, R24
a7aca:  lds R24, 0x00e2
a7ace:  andi    R24, 0xef
a7ad0:  sts 0x00e2, R24
a7ad4:  lds R24, 0x00e2
a7ad8:  ori R24, 0x01
a7ada:  sts 0x00e2, R24
a7ade:  lds R24, 0x01b4
a7ae2:  and R24, R24
a7ae4:  brne    a7af2
a7ae6:  lds R24, 0x00e3
a7aea:  sbrc    R24, 7
a7aec:  rjmp    a7af2
a7aee:  ldi R24, 0x01
a7af0:  rjmp    a7af4
a7af2:  ldi R24, 0x04
a7af4:  out $1e, R24
a7af6:  rcall   a7ed0
a7af8:  lds R24, 0x00e1
a7afc:  sbrs    R24, 3
a7afe:  rjmp    a7b42
a7b00:  lds R24, 0x00e2
a7b04:  sbrs    R24, 3
a7b06:  rjmp    a7b42
a7b08:  lds R24, 0x00e1
a7b0c:  andi    R24, 0xf7
a7b0e:  sts 0x00e1, R24
a7b12:  ldi R24, 0x02
a7b14:  out $1e, R24
a7b16:  sts 0x01b4, R1
a7b1a:  lds R24, 0x00e1
a7b1e:  andi    R24, 0xfe
a7b20:  sts 0x00e1, R24
a7b24:  lds R24, 0x00e2
a7b28:  andi    R24, 0xfe
a7b2a:  sts 0x00e2, R24
a7b2e:  lds R24, 0x00e2
a7b32:  ori R24, 0x10
a7b34:  sts 0x00e2, R24
a7b38:
    ldi R24, 0x00
    ldi R22, 0x00
    ldi R20, 0x02
a7b3e:
    rcall a78f4
a7b40:
    rcall a7ed0
a7b42:
    pop R31
    pop R30
    pop R27
    pop R26
    pop R25
    pop R24
    pop R23
    pop R22
    pop R21
    pop R20
    pop R19
    pop R18
    pop R0
    out $3f, R0
    pop R0
    pop R1
a7b62:
    reti    
a7b64:
    movw R18, R24
    lds R20, 0x01bc
    lds R21, 0x01bd
    cp R20, R22
    cpc R21, R23
a7b72:
    brcc a7b7a
a7b74:
    movw R30, R18
a7b76:
    ldi R25, 0x00
a7b78:
    rjmp    a7c02
a7b7a:
    cp  R22, R1
a7b7c:
    cpc R23, R1
a7b7e:
    breq a7b84
a7b80:
    movw R20, R22
a7b82:
    rjmp a7b74
a7b84:
    lds R24, 0x00e8
a7b88:
    andi R24, 0x7e
a7b8a:
    sts 0x00e8, R24
a7b8e:
    ldi R20, 0x00
a7b90:
    ldi R21, 0x00
a7b92:
    rjmp a7b74
a7b94:
    in  R24, 0x1e
a7b96:
    and R24, R24
a7b98:
    brne a7b9c
a7b9a:
    rjmp a7c24
a7b9c:
    cpi R24, 0x05
a7b9e:
    brne a7ba2
a7ba0:
    rjmp a7c28
a7ba2:
    lds R24, 0x00e8
    sbrs R24, 3
    rjmp a7bae
a7baa:
    ldi R24, 0x01
a7bac:
    ret 
a7bae:
    lds R24, 0x00e8
a7bb2:
    sbrc R24, 2
a7bb4:
    rjmp a7c18
a7bb6:
    lds R24, 0x00e8
a7bba:
    sbrs R24, 0
a7bbc:
    rjmp    a7c02
    lds R24, 0x00f3
    lds R25, 0x00f2
    mov R23, R24
    ldi R22, 0x00
a7bca:
    mov R18, R25
a7bcc:
    ldi R19, 0x00
a7bce:
    or  R18, R22
a7bd0:
    or  R19, R23
a7bd2:
    rjmp    a7be2
a7bd4:
    ld  R24, Z+
a7bd6:
    sts 0x00f1, R24
    subi R20, 0x01
    sbci R21, 0x00
    subi R18, 0xff
    sbci R19, 0xff
a7be2:
    cp  R20, R1
a7be4:
  cpc R21, R1
a7be6:
  breq    a7bee
a7be8:
  cpi R18, 0x08
a7bea:
  cpc R19, R1
a7bec:
  brcs    a7bd4
a7bee:
  ldi R25, 0x00
a7bf0:
  cpi R18, 0x08
a7bf2:
  cpc R19, R1
a7bf4:
  brne    a7bf8
a7bf6:
  ldi R25, 0x01
a7bf8:
  lds R24, 0x00e8
a7bfc:
  andi    R24, 0x7e
a7bfe:
    sts 0x00e8, R24
a7c02:
    cp  R20, R1
a7c04:
    cpc R21, R1
a7c06:
    brne    a7b94
a7c08:
    and R25, R25
a7c0a:
    brne    a7b94
a7c0c:
    rjmp    a7c18
a7c0e:
    in  R24, 0x1e
a7c10:
    and R24, R24
a7c12:
    breq    a7c24
a7c14:
    cpi R24, 0x05
a7c16:
    breq    a7c28
a7c18:
    lds R24, 0x00e8
a7c1c:
    sbrs    R24, 2
a7c1e:
    rjmp    a7c0e
a7c20:
    ldi R24, 0x00
a7c22:
    ret 
a7c24:
    ldi R24, 0x02
a7c26:
    ret 
a7c28:
    ldi R24, 0x03
a7c2a:
    ret 
a7c2c:
    movw R18, R24
a7c2e:
    cp  R22, R1
a7c30:
    cpc R23, R1
a7c32:
    brne a7c3e
a7c34:
    lds R24, 0x00e8
a7c38:
    andi R24, 0x7b
a7c3a:
    sts 0x00e8, R24
a7c3e:
    movw R30, R18
a7c40:
    rjmp a7c8e
a7c42: 
    in  R24, 0x1e
a7c44:
    and R24, R24
a7c46:
    breq a7cac
a7c48:
    cpi R24, 0x05
a7c4a:
    breq a7cb0
    lds R24, 0x00e8
    sbrs R24, 3
    rjmp a7c58
a7c54:
    ldi R24, 0x01
a7c56:
    ret 
a7c58:
    lds R24, 0x00e8
    sbrs R24, 2
    rjmp a7c42
    rjmp a7c6e
a7c62:
    lds R24, 0x00f1
a7c66:
    st  Z+, R24
a7c68:
    subi R22, 0x01
a7c6a:
    sbci R23, 0x00
a7c6c:
    breq a7c84
a7c6e:
    lds R18, 0x00f3
a7c72:
    lds R24, 0x00f2
a7c76:
    mov R19, R18
a7c78:
    ldi R18, 0x00
a7c7a:
    ldi R25, 0x00
a7c7c:
    or  R24, R18
    or R25, R19
    or R24, R25
a7c82:
    brne a7c62
a7c84:
    lds R24, 0x00e8
a7c88:
    andi R24, 0x7b
a7c8a:
    sts 0x00e8, R24
a7c8e:
    cp R22, R1
a7c90:
    cpc R23, R1
a7c92:
    brne a7c42
a7c94:
    rjmp a7ca0
a7c96:
    in R24, 0x1e
a7c98:
    and R24, R24
a7c9a:
    breq a7cac
a7c9c:
    cpi R24, 0x05
a7c9e:
    breq a7cb0
a7ca0:
    lds R24, 0x00e8
a7ca4:
    sbrs R24, 0
a7ca6:
    rjmp a7c96
a7ca8:
    ldi R24, 0x00
a7caa:
    ret 
a7cac:
    ldi R24, 0x02
a7cae:
    ret 
a7cb0:
    ldi R24, 0x03
    ret 
a7cb4:
    push R16
    push R17
    push R29
    push R28
a7cbc:
    rcall a7cbe
a7cbe:
    in R28, 0x3d
a7cc0:
    in R29, 0x3e
a7cc2:
    ldi R30, 0xb6
a7cc4:
    ldi R31, 0x01
a7cc6:
    lds R24, 0x00f1
a7cca:
    st Z+, R24
a7ccc:
    ldi R24, 0x01
a7cce:
    cpi R30, 0xbe
a7cd0:
    cpc R31, R24
a7cd2:
    brne a7cc6
a7cd4:
    rcall a772a
a7cd6:
    lds R24, 0x00e8
a7cda:
    sbrs R24, 3
a7cdc:
    rjmp a7ea6
a7cde:
    lds R24, 0x01b6
a7ce2:
    lds R25, 0x01b7
a7ce6:
    cpi R25, 0x05
a7ce8:
    brne    a7cec
a7cea:
    rjmp    a7dc6
a7cec:
    cpi R25, 0x06
a7cee:
    brcc    a7d00
a7cf0:
    cpi R25, 0x01
a7cf2:
    breq    a7d54
a7cf4:
    cpi R25, 0x01
a7cf6:
    brcs    a7d14
a7cf8:
    cpi R25, 0x03
a7cfa:
    breq    a7cfe
a7cfc:
    rjmp    a7ea6
a7cfe:
    rjmp    a7d54
a7d00:
    cpi R25, 0x08
a7d02:
    brne    a7d06
a7d04:
    rjmp    a7e4c
a7d06:
    cpi R25, 0x09
a7d08:
    brne    a7d0c
a7d0a:
    rjmp    a7e70
a7d0c:
    cpi R25, 0x06
a7d0e:
    breq    a7d12
a7d10:
    rjmp    a7ea6
a7d12:
    rjmp    a7e0c
a7d14:
    cpi R24, 0x80
a7d16:
    brne    a7d1a
a7d18:
    rjmp    a7ea6
a7d1a:
    cpi R24, 0x82
a7d1c:
     breq a7d20
a7d1e:
     rjmp a7ea6
a7d20:
    lds R24, 0x01ba
a7d24:
    andi R24, 0x07
a7d26:
    sts 0x00e9, R24
a7d2a:
    lds R24, 0x00eb
a7d2e:
    sts 0x00e9, R1
a7d32:
    lds R18, 0x00e8
a7d36:
    andi R18, 0xf7
a7d38:
    sts 0x00e8, R18
a7d3c:
    ldi R25, 0x00
a7d3e:
    ldi R18, 0x05
a7d40:
    lsr R25
a7d42:
    ror R24
a7d44:
    dec R18
a7d46:
    brne a7d40
a7d48:
    andi R24, 0x01
a7d4a:
    sts 0x00f1, R24
a7d4e:
    sts 0x00f1, R1
    rjmp a7e62
a7d54:
    and R24, R24
    breq a7d5e
a7d58:
    cpi R24, 0x02
a7d5a:
    breq    a7d5e
a7d5c:
    rjmp    a7ea6
a7d5e:
  andi    R24, 0x1f
a7d60:
  cpi R24, 0x02
a7d62:
  breq    a7d66
a7d64:
  rjmp    a7ea6
a7d66:
  lds R24, 0x01b8
a7d6a:
  and R24, R24
a7d6c:
  brne    a7dba
a7d6e:
  lds R18, 0x01ba
a7d72:
    andi R18, 0x07
    brne a7d78
    rjmp a7ea6
a7d78:
  sts 0x00e9, R18
a7d7c:
  lds R24, 0x00eb
a7d80:
  sbrs    R24, 0
a7d82:
  rjmp    a7dba
a7d84:
  cpi R25, 0x03
a7d86:
  brne    a7d90
a7d88:
  lds R24, 0x00eb
a7d8c:
  ori R24, 0x20
a7d8e:
  rjmp    a7db6
a7d90:
  lds R24, 0x00eb
a7d94:
  ori R24, 0x10
a7d96:
  sts 0x00eb, R24
a7d9a:
  ldi R24, 0x01
a7d9c:
  ldi R25, 0x00
a7d9e:
  rjmp    a7da4
a7da0:
  add R24, R24
a7da2:
  adc R25, R25
a7da4:
  dec R18
a7da6:
  brpl    a7da0
a7da8:
  sts 0x00ea, R24
a7dac:
  sts 0x00ea, R1
a7db0:
  lds R24, 0x00eb
a7db4:
  ori R24, 0x08
a7db6:
  sts 0x00eb, R24
a7dba:
  sts 0x00e9, R1
a7dbe:
  lds R24, 0x00e8
a7dc2:
  andi    R24, 0xf7
a7dc4:
  rjmp    a7e68
a7dc6:
  and R24, R24
a7dc8:
  breq    a7dcc
a7dca:
  rjmp    a7ea6
a7dcc:
  lds R17, 0x01b8
a7dd0:
  andi    R17, 0x7f
a7dd2:
  in  R16, 0x3f
a7dd4:
  cli 
a7dd6:
  lds R24, 0x00e8
a7dda:
  andi    R24, 0xf7
a7ddc:
  sts 0x00e8, R24
a7de0:
  rcall   a791a
a7de2:
  lds R24, 0x00e8
a7de6:
  sbrs    R24, 0
a7de8:
  rjmp    a7de2
a7dea:
  lds R24, 0x00e3
a7dee:
  andi    R24, 0x80
a7df0:
  or  R24, R17
a7df2:
  sts 0x00e3, R24
a7df6:
  ori R24, 0x80
a7df8:
  sts 0x00e3, R24
a7dfc:
  and R17, R17
a7dfe:
  brne    a7e04
a7e00:
  ldi R24, 0x02
a7e02:
  rjmp    a7e06
a7e04:
  ldi R24, 0x03
a7e06:
  out $1e, R24
a7e08:
  out $3f, R16
a7e0a:
  rjmp    a7ea6
a7e0c:
  subi    R24, 0x80
a7e0e:
  cpi R24, 0x02
a7e10:
  brcs    a7e14
a7e12:
  rjmp    a7ea6
a7e14:
    lds R24, 0x01b8
    lds R25, 0x01b9
    lds R22, 0x01ba
a7e20:
  movw    R20, R28
a7e22:
  subi    R20, 0xff
a7e24:
  sbci    R21, 0xff
a7e26:
  rcall   a7898
a7e28:
  movw    R22, R24
a7e2a:
  sbiw    R24, 0x00
a7e2c:
  brne    a7e30
a7e2e:
  rjmp    a7ea6
a7e30:
  lds R24, 0x00e8
a7e34:
  andi    R24, 0xf7
a7e36:
  sts 0x00e8, R24
a7e3a:
  ldd R24, Y+1
a7e3c:
  ldd R25, Y+2
a7e3e:
  rcall   a7b64
a7e40:
  lds R24, 0x00e8
a7e44:
  andi    R24, 0x7b
a7e46:
  sts 0x00e8, R24
a7e4a:
  rjmp    a7ea6
a7e4c:
  cpi R24, 0x80
a7e4e:
  brne    a7ea6
a7e50:
  lds R24, 0x00e8
a7e54:
  andi    R24, 0xf7
a7e56:
  sts 0x00e8, R24
a7e5a:
  lds R24, 0x01b4
a7e5e:
  sts 0x00f1, R24
a7e62:
  lds R24, 0x00e8
a7e66:
  andi    R24, 0x7e
a7e68:
  sts 0x00e8, R24
a7e6c:
  rcall   a791a
a7e6e:
  rjmp    a7ea6
a7e70:
  and R24, R24
a7e72:
  brne    a7ea6
a7e74:
  lds R25, 0x01b8
a7e78:
  cpi R25, 0x02
a7e7a:
  brcc    a7ea6
a7e7c:
  lds R24, 0x00e8
a7e80:
  andi    R24, 0xf7
a7e82:
  sts 0x00e8, R24
a7e86:
  sts 0x01b4, R25
a7e8a:
  rcall   a791a
a7e8c:
  lds R24, 0x01b4
a7e90:
  and R24, R24
a7e92:
    brne a7ea0
a7e94:
    lds R24, 0x00e3
a7e98:
    sbrc    R24, 7
a7e9a:
    rjmp    a7ea0
a7e9c:
    ldi R24, 0x01
a7e9e:
    rjmp    a7ea2
a7ea0:
    ldi R24, 0x04
a7ea2:
    out $1e, R24
a7ea4:
    rcall   a778a
a7ea6:
    lds R24, 0x00e8
a7eaa:
    sbrs    R24, 3
a7eac:
    rjmp    a7ec2
a7eae:
    lds R24, 0x00eb
a7eb2:
    ori R24, 0x20
a7eb4:
    sts 0x00eb, R24
a7eb8:
    lds R24, 0x00e8
a7ebc:
    andi R24, 0xf7
a7ebe:
    sts 0x00e8, R24
a7ec2:
    pop R0
    pop R0
    pop R28
    pop R29
    pop R17
    pop R16
a7ece:
    ret 
a7ed0:
    ret 
a7ed2:
    push R17
a7ed4:
    in R24, 0x1e
a7ed6:  and R24, R24
    breq a7ef2
a7eda:
    lds R17, 0x00e9
    sts 0x00e9, R1
    lds R24, 0x00e8
    sbrs R24, 3
    rjmp a7eec
a7eea:
    rcall a7cb4
a7eec:
    andi R17, 0x07
a7eee:
    sts 0x00e9, R17
a7ef2:
    pop R17
a7ef4:
    ret 
a7ef6:
    sbic $1f, 1
a7ef8:
    rjmp a7ef6
a7efa:
    out $22, R25
a7efc:
    out $21, R24
a7efe:
    sbi $1f, 0
a7f00:
    eor R25, R25
a7f02:
    in R24, 0x20
a7f04:
    ret 
a7f06:
    mov R18, R22
a7f08:
    sbic $1f, 1
    rjmp a7f08
a7f0c:
    out $1f, R1
    out $22, R25
    out $21, R24
    out $20, R18
a7f14:
    in R0, 0x3f
a7f16:
    cli 
a7f18:
    sbi $1f, 2
a7f1a:
    sbi $1f, 1
a7f1c:
    out $3f, R0
a7f1e:
    adiw    R24, 0x01
a7f20:
    ret 
a7f22:
    cli 
a7f24:
    rjmp a7f24
a7f26:
    sbci R20, 0x13
    sbci R21, 0x54
    sbci R21, 0x92
    sbci R20, 0x1e
    andi R16, 0x70
a7f30:
    .dw 0x0077
a7f32:
    .dw 0x0008
a7f34:
    nop 
a7f36:
    nop 
a7f38:
    sbc R0, R0
a7f3a:
    cpse R0, R17
a7f3c:
    cpse R0, R1
a7f3e:
    muls R16, R17
a7f40:
    nop 
a7f42:
    sbci    R16, 0x18
a7f44:
    cpi R18, 0x63
a7f46:
    movw    R0, R0
a7f48:
    muls    R16, R16
a7f4a:
    .dw 0x0001
a7f4c:
    sbc R16, R1
    cpi R16, 0xe2
    muls    R16, R16
    .dw 0x0001
a7f54:
    cpi R24, 0x20
a7f56:
    cpc R0, R9
a7f58:
    nop 
a7f5a:
    muls    R16, R17
a7f5c:
    movw    R0, R4
a7f5e:
    cpc R16, R0
a7f60:
    .dw 0x0024
a7f62:
    movw    R2, R0
a7f64:
    eor R0, R4
a7f66:
    cpc R0, R2
a7f68:
    eor R0, R5
a7f6a:
    .dw 0x0006
a7f6c:
    cpc R16, R17
a7f6e:
    std Z+5, R0
a7f70:
    sbc R0, R3
a7f72:
    sbrs R16, 0
a7f74:
    cpc R0, R9
a7f76:
    .dw 0x0001
a7f78:
    sbc R0, R18
    nop 
    cpc R16, R16
    cpc R0, R5
    cpse R0, R2
    movw R0, R0
    cpc R16, R7
a7f86:
    muls R24, R19
a7f88:
    .dw 0x0010
a7f8a:
    cpc R0, R1
a7f8c:
    sbc R16, R3
a7f8e:
    and R0, R20
a7f90:
    sbci R16, 0x13
a7f92:
    andi R16, 0x20
a7f94:
    ori R16, 0x40
a7f96:
    andi R16, 0x50
a7f98:
    ori R16, 0x90
a7f9a:
    ori R16, 0xe0
a7f9c:
    ori R16, 0xf0
a7f9e:
    and R0, R0
    sbci R16, 0xc0
    ori R16, 0x50
    ori R16, 0xf0
    ori R16, 0xe0
    ori R16, 0x10
    andi R16, 0x20
    ori R16, 0x40
    ori R16, 0xf0
    nop 
    sub R0, R0
    sbci R16, 0x13
    andi R16, 0x20
    ori R16, 0x40
    andi R16, 0x50
    ori R16, 0x90
    ori R16, 0xe0
    ori R16, 0xf0
    and R0, R0
    sbci R16, 0xc0
    sbci R16, 0xc0
    sbci R16, 0x30
    nop 
    nop


