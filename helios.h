#ifndef _HELIOS_H_
#define _HELIOS_H_
#include "misc.h"

struct PortE : public Port { PortE() : Port((uint8_t *)0x2c) { } };
struct PortF : public Port { PortF() : Port((uint8_t *)0x2f) { } };

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
    PortB portB;
    PortC portC;
    PortD portD;
    PortE portE;
    PortF portF;
    Pin pin4;
    Pin pin13;
    Pin pinA0;
    Pin pinA1;
    Pin pinA2;
    Pin pinA3;
    Pin pinA4;
    Pin pinA5;

    Board()
      :
        pin4(portC, BIT7), // tijdelijk
        pin13(portC, BIT7),
        pinA0(portF, BIT7),
        pinA1(portF, BIT6),
        pinA2(portF, BIT5),
        pinA3(portF, BIT4),
        pinA4(portF, BIT1),
        pinA5(portF, BIT0)
    { }
};

#endif


