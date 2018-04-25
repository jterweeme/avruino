#ifndef _UART_H_
#define _UART_H_
#include "pinport.h"

class Terminal
{
public:
    virtual void myPutc(char c) { }
    virtual void puts(const char *s) { while (*s) myPutc(*s++); }
    void write(char c) { myPutc(c); }
    void write(const char *s) { puts(s); }
    virtual void printf(const char *format, ...);
    virtual uint8_t readByte() { return 0; }
    void operator<< (const char *s) { puts(s); }
};

class LCD : public Terminal
{
protected:
    Pin * const rs;
    Pin * const e;
    Pin * const d4;
    Pin * const d5;
    Pin * const d6;
    Pin * const d7;
    void lcd_write_4(uint8_t theByte);
public:
    static constexpr uint8_t
        LINEONE = 0x00,
        LINETWO = 0x40,
        CLEAR = 0x01,
        HOME = 0x02,
        ENTRYMODE = 0x06,
        OFF = 0x08,
        ON = 0x0c,
        RESET = 0x30,
        FOURBIT = 0x28,
        SETCURSOR = 0x80,
        CURSOR_HOME = 0x02;

    LCD(Pin *rs, Pin *e, Pin *d4, Pin *d5, Pin *d6, Pin *d7);
    void myPutc(char c);
    void puts(const char *s);
    void lcd_write_instruction_4d(uint8_t theInstruction);
    void home();
    void clear() { lcd_write_instruction_4d(CLEAR); }
};

class UartBase : public Terminal
{
protected:
    volatile uint16_t * const brr;
    volatile uint8_t * const udr;
    volatile uint8_t * const ucsra;
    volatile uint8_t * const ucsrb;
public:
    static constexpr uint8_t
        MUDRE = 1<<5,
        MRXC = 1<<7,
        TXEn = 1<<3,
        RXEn = 1<<4,
        udrie = 1<<5,
        txcie = 1<<6,
        rxcie = 1<<7;

    UartBase(volatile uint16_t *brr, volatile uint8_t *udr, volatile uint8_t *ucsra,
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



