#ifndef _LCD_H_
#define _LCD_H_
#include "types.h"

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

class Pin;

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
#endif



