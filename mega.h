#ifndef _MEGA_H_
#define _MEGA_H_
#include "misc.h"

#define F_CPU 16000000UL

struct PortA : public Port { PortA() : Port((uint8_t *)0x20) { } };
struct PortE : public Port { PortE() : Port((uint8_t *)0x2c) { } };
struct PortF : public Port { PortF() : Port((uint8_t *)0x2f) { } };
struct PortG : public Port { PortG() : Port((uint8_t *)0x32) { } };
struct PortH : public Port { PortH() : Port((uint8_t *)0x100) { } };
struct PortJ : public Port { PortJ() : Port((uint8_t *)0x103) { } };
struct PortK : public Port { PortK() : Port((uint8_t *)0x106) { } };
struct PortL : public Port { PortL() : Port((uint8_t *)0x109) { } };

class SPIBus : public SPIBase
{
public:
    SPIBus();
};

struct Board
{
    PortA portA;
    PortB portB;
    PortC portC;
    PortD portD;
    PortE portE;
    PortF portF;
    PortG portG;
    PortH portH;
    PortJ portJ;
    PortK portK;
    PortL portL;
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
    Pin pin14;
    Pin pin15;
    Pin pin16;
    Pin pin17;
    Pin pin18;
    Pin pin19;
    Pin pin20;
    Pin pin21;
    Pin pin22;
    Pin pin23;
    Pin pin24;
    Pin pin25;
    Pin pin26;
    Pin pin27;
    Pin pin28;
    Pin pin29;
    Pin pin30;
    Pin pin31;
    Pin pin32;
    Pin pin33;
    Pin pin34;
    Pin pin35;
    Pin pin36;
    Pin pin37;
    Pin pin38;
    Pin pin39;
    Pin pin40;
    Pin pin41;
    Pin pin42;
    Pin pin43;
    Pin pin44;
    Pin pin45;
    Pin pin46;
    Pin pin47;
    Pin pin48;
    Pin pin49;
    Pin pin50;
    Pin pin51;
    Pin pin52;
    Pin pin53;
    Pin pinA0;
    Pin pinA1;
    Pin pinA2;
    Pin pinA3;
    Pin pinA4;
    Pin pinA5;
    Pin pinA6;
    Pin pinA7;
    Pin pinA8;
    Pin pinA9;
    Pin pinA10;
    Pin pinA11;
    Pin pinA12;
    Pin pinA13;
    Pin pinA14;
    Pin pinA15;

    Board()
      :
        pin0(portE, BIT0),
        pin1(portE, BIT1),
        pin2(portE, BIT4),
        pin3(portE, BIT5),
        pin4(portG, BIT5),
        pin5(portE, BIT3),
        pin6(portH, BIT3),
        pin7(portH, BIT4),
        pin8(portH, BIT5),
        pin9(portH, BIT6),
        pin10(portB, BIT4),
        pin11(portB, BIT5),
        pin12(portB, BIT6),
        pin13(portB, BIT7),
        pin14(portJ, BIT1),
        pin15(portJ, BIT0),
        pin16(portH, BIT1),
        pin17(portH, BIT0),
        pin18(portD, BIT3),
        pin19(portD, BIT2),
        pin20(portD, BIT1),
        pin21(portD, BIT0),
        pin22(portA, BIT0),
        pin23(portA, BIT1),
        pin24(portA, BIT2),
        pin25(portA, BIT3),
        pin26(portA, BIT4),
        pin27(portA, BIT5),
        pin28(portA, BIT6),
        pin29(portA, BIT7),
        pin30(portC, BIT7),
        pin31(portC, BIT6),
        pin32(portC, BIT5),
        pin33(portC, BIT4),
        pin34(portC, BIT3),
        pin35(portC, BIT2),
        pin36(portC, BIT1),
        pin37(portC, BIT0),
        pin38(portD, BIT7),
        pin39(portG, BIT2),
        pin40(portG, BIT1),
        pin41(portG, BIT0),
        pin42(portL, BIT7),
        pin43(portL, BIT6),
        pin44(portL, BIT5),
        pin45(portL, BIT4),
        pin46(portL, BIT3),
        pin47(portL, BIT2),
        pin48(portL, BIT1),
        pin49(portL, BIT0),
        pin50(portB, BIT3),
        pin51(portB, BIT2),
        pin52(portB, BIT1),
        pin53(portB, BIT0),
        pinA0(portF, BIT0),
        pinA1(portF, BIT1),
        pinA2(portF, BIT2),
        pinA3(portF, BIT3),
        pinA4(portF, BIT4),
        pinA5(portF, BIT5),
        pinA6(portF, BIT6),
        pinA7(portF, BIT7),
        pinA8(portK, BIT0),
        pinA9(portK, BIT1),
        pinA10(portK, BIT2),
        pinA11(portK, BIT3),
        pinA12(portK, BIT4),
        pinA13(portK, BIT5),
        pinA14(portK, BIT6),
        pinA15(portK, BIT7)
    { }
};
#endif


