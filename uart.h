#ifndef _UART_H_
#define _UART_H_
#include "types.h"

class UartBase
{
protected:
    volatile uint16_t * const _brr;
    volatile uint8_t * const udr;
    volatile uint8_t * const ucsra;
    volatile uint8_t * const ucsrb;

    static constexpr uint8_t MUDRE = 1<<5, MRXC = 1<<7, TXEn = 1<<3, RXEn = 1<<4,
        udrie = 1<<5,
        txcie = 1<<6,
        rxcie = 1<<7;
public:
    UartBase(volatile uint16_t *_brr, volatile uint8_t *udr, volatile uint8_t *ucsra,
        volatile uint8_t *ucsrb);

    void myPutc(char c) { while ((*ucsra & MUDRE) == 0); *udr = c; }
    virtual void onReceive() { }
    void inline enableTransmit() { *ucsrb |= TXEn; }
    void inline enableRead() { *ucsrb |= RXEn; }
    void inline enableReadInterrupt() { *ucsrb |= rxcie; asm volatile ("sei"); }
    uint8_t readByte() { while ((*ucsra & MRXC) == 0); return *udr; }
};

class Uart : public UartBase
{
private:
    static Uart *instance;
public:
    Uart();
    static Uart *getInstance() { return instance; }
    static const uint8_t MYTXEN0 = 3;
    static const uint8_t MYTXEN1 = 3;
};

class DefaultUart : public Uart
{
public:
    DefaultUart();
};
#endif



