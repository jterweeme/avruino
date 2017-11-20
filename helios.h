#ifndef _HELIOS_H_
#define _HELIOS_H_

#define F_CPU 8000000

#include "misc.h"

static constexpr uint8_t
    portb_base = 0x23,
        pinb = portb_base,
        pinb0 = 0, pinb1 = 1, pinb2 = 2, pinb3 = 3, pinb4 = 4, pinb5 = 5, pinb6 = 6, pinb7 = 7,
        ddrb = portb_base + 1,
        ddb0 = 0, ddb1 = 1, ddb2 = 2, ddb3 = 3, ddb4 = 4, ddb5 = 5, ddb6 = 6, ddb7 = 7,
        portb = portb_base + 2,
        pb0 = 0, pb1 = 1, pb2 = 2, pb3 = 3, pb4 = 4, pb5 = 5, pb6 = 6, pb7 = 7,
    portc_base = 0x26,
        pinc = portc_base,
        pinc0 = 0, pinc1 = 1, pinc2 = 2, pinc3 = 3, pinc4 = 4, pinc5 = 5, pinc6 = 6, pinc7 = 7,
        ddrc = portc_base + 1,
        ddc0 = 0, ddc1 = 1, ddc2 = 2,
        portc = portc_base + 2,
    portd_base = 0x29,
        pind = portd_base,
        ddrd = portd_base + 1,
        portd = portd_base + 2,
    porte_base = 0x2c,
        pine = porte_base,
        ddre = porte_base + 1,
        porte = porte_base + 2,
    portf_base = 0x2f,
        pinf = portf_base,
        ddrf = portf_base + 1,
        portf = portf_base + 2;
        
class Timer3 : public Timer<uint16_t>
{
protected:
    static Timer3 *instance;
public:
    Timer3();
    static Timer3 *getInstance() { return instance; }
};

class Timer4 : public Timer<uint16_t>
{
protected:
    static Timer4 *instance;
public:
    Timer4();
    static Timer4 *getInstance() { return instance; }
};

class SPIBus : public SPIBase
{
public:
    SPIBus();
};

struct Board
{
    Port
        portB { (uint8_t *)portb_base },
        portC { (uint8_t *)portc_base },
        portD { (uint8_t *)portd_base },
        portE { (uint8_t *)porte_base },
        portF { (uint8_t *)portf_base };

    Pin
        pin0 { portD, BIT2 },
        pin1 { portD, BIT3 },
        pin2 { portD, BIT1 },
        pin3 { portD, BIT0 },
        pin4 { portD, BIT4 },
        pin5 { portC, BIT6 },
        pin6 { portD, BIT7 },
        pin7 { portE, BIT6 },
        pin8 { portB, BIT4 },
        pin9 { portB, BIT5 },
        pin10 { portB, BIT6 },
        pin11 { portB, BIT7 },
        pin12 { portD, BIT6 },
        pin13 { portC, BIT7 },
        pinA0 { portF, BIT7 },
        pinA1 { portF, BIT6 },
        pinA2 { portF, BIT5 },
        pinA3 { portF, BIT4 },
        pinA4 { portF, BIT1 },
        pinA5 { portF, BIT0 };
};

#endif


