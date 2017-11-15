#ifndef _UNO_H_
#define _UNO_H_
#include "misc.h"
#include <avr/interrupt.h>

#define F_CPU 16000000UL

class SPIBus : public SPIBase
{
public:
    SPIBus();
};

struct Board
{
    PortB portB;
    PortC portC;
    PortD portD;
    Pin pin0;
    Pin pin1;
    Pin pin2;
    Pin pin3;
    Pin pin4;
    Pin pin5;
    Pin pin6;
    Pin pin7;
    Pin pin8;
    Pin pin9;
    Pin pin10;
    Pin pin11;
    Pin pin12;
    Pin pin13;
    Pin pinA0;
    Pin pinA1;
    Pin pinA2;
    Pin pinA3;
    Pin pinA4;
    Pin pinA5;
    Board()
      :
        pin0(portD, BIT0),
        pin1(portD, BIT1),
        pin2(portD, BIT2),
        pin3(portD, BIT3),
        pin4(portD, BIT4),
        pin5(portD, BIT5),
        pin6(portD, BIT6),
        pin7(portD, BIT7),
        pin8(portB, BIT0),
        pin9(portB, BIT1),
        pin10(portB, BIT2),
        pin11(portB, BIT3),
        pin12(portB, BIT4),
        pin13(portB, BIT5),
        pinA0(portC, BIT0),
        pinA1(portC, BIT1),
        pinA2(portC, BIT2),
        pinA3(portC, BIT3),
        pinA4(portC, BIT4),
        pinA5(portC, BIT5)
    { }
};

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
static const uint8_t FRACT_MAX = 1000 >> 3;
static volatile unsigned long timer0_overflow_count = 0;

unsigned long millis();

#endif


