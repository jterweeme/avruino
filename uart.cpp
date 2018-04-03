#include "uart.h"
#include <stdarg.h>

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <util/delay.h>

void Terminal::printf(const char *format, ...)
{
    va_list argp;
    va_start(argp, format);

    for (const char *p = format; *p != '\0'; p++)
    {
        if (*p != '%')
        {
            myPutc(*p);
            continue;
        }

        switch (*++p)
        {
        case 'u':
            {
                unsigned u = va_arg(argp, unsigned);
                char s[40] = {0};
                puts(Utility::itoa(u, s, 10));
            }

            break;
        case 's':
            {
                char *s = va_arg(argp, char *);
                puts(s);
            }

            break;
        case 'x':
            {
                unsigned u = va_arg(argp, unsigned);
                const uint8_t foo = u & 0x0f;
                const uint8_t bar = u >> 4;
                char high = bar < 10 ? (char)(bar + 48) : (char)(bar + 55);
                char low = foo < 10 ? (char)(foo + 48) : (char)(foo + 55);
                myPutc(high);
                myPutc(low);
            }
            break;
        }
    }

    va_end(argp);
}

UartBase::UartBase(volatile uint16_t *brr, volatile uint8_t *udr,
    volatile uint8_t *ucsra, volatile uint8_t *ucsrb)
  :
    brr(brr),
    udr(udr),
    ucsra(ucsra),
    ucsrb(ucsrb)
{
}

LCD::LCD(Pin *rs, Pin *e, Pin *d4, Pin *d5, Pin *d6, Pin *d7)
  :
    rs(rs), e(e), d4(d4), d5(d5), d6(d6), d7(d7)
{
    rs->direction(OUTPUT);
    e->direction(OUTPUT);
    d4->direction(OUTPUT);
    d5->direction(OUTPUT);
    d6->direction(OUTPUT);
    d7->direction(OUTPUT);
    _delay_ms(100);
    rs->clear();
    e->clear();
    lcd_write_4(RESET);
    _delay_ms(10);
    lcd_write_4(RESET);
    _delay_us(200);
    lcd_write_4(RESET);
    _delay_us(200);
    lcd_write_4(FOURBIT);
    _delay_us(80);
    lcd_write_instruction_4d(FOURBIT);
    _delay_us(80);
    lcd_write_instruction_4d(OFF);
    _delay_us(80);
    lcd_write_instruction_4d(CLEAR);
    _delay_ms(4);
    lcd_write_instruction_4d(ENTRYMODE);
    _delay_us(80);
    lcd_write_instruction_4d(ON);
    _delay_us(80);
}

void LCD::home()
{
    lcd_write_instruction_4d(CURSOR_HOME);
    _delay_ms(2);
}

void LCD::lcd_write_4(uint8_t theByte)
{
    d7->set(theByte & 1<<7);
    d6->set(theByte & 1<<6);
    d5->set(theByte & 1<<5);
    d4->set(theByte & 1<<4);
    e->set();
    _delay_us(1);
    e->clear();
    _delay_us(1);
}

void LCD::myPutc(char c)
{
    if (c == '\r')
        return home();

    rs->set();
    e->clear();
    lcd_write_4(c);
    lcd_write_4(c << 4);
}

void LCD::puts(const char *s)
{
    while (*s)
    {
        myPutc(*s++);
        _delay_us(80);
    }
}

void LCD::lcd_write_instruction_4d(uint8_t theInstruction)
{
    rs->clear();
    e->clear();
    lcd_write_4(theInstruction);
    lcd_write_4(theInstruction << 4);
}




