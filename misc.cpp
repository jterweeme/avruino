#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "board.h"
#include "misc.h"
#include <stdarg.h>
#include "analog.h"
#include "uart.h"

static inline void _delay_loop_1(uint8_t __count) __attribute__((always_inline));
static inline void _delay_loop_2(uint16_t __count) __attribute__((always_inline));

void _delay_loop_1(uint8_t __count)
{
    __asm__ volatile (
        "1: dec %0" "\n\t"
        "brne 1b"
        : "=r" (__count)
        : "0" (__count)
    );
}

void _delay_loop_2(uint16_t __count)
{
    __asm__ volatile (
        "1: sbiw %0,1" "\n\t"
        "brne 1b"
        : "=w" (__count)
        : "0" (__count)
    );
}

static inline void _delay_us(double __us) __attribute__((always_inline));
static inline void _delay_ms(double __ms) __attribute__((always_inline));

void _delay_ms(double __ms)
{
    double __tmp ;
    uint16_t __ticks;
    __tmp = ((F_CPU) / 4e3) * __ms;

    if (__tmp < 1.0)
    {
        __ticks = 1;
    }
    else if (__tmp > 65535)
    {
        __ticks = (uint16_t) (__ms * 10.0);

        while(__ticks)
        {
            _delay_loop_2(((F_CPU) / 4e3) / 10);
            __ticks --;
        }

        return;
    }
    else
    {
        __ticks = (uint16_t)__tmp;
    }
    _delay_loop_2(__ticks);
}

void _delay_us(double __us)
{
    double __tmp ;
#if __HAS_DELAY_CYCLES && defined(__OPTIMIZE__) && \
  !defined(__DELAY_BACKWARD_COMPATIBLE__) &&       \
  __STDC_HOSTED__
    uint32_t __ticks_dc;
    extern void __builtin_avr_delay_cycles(unsigned long);
    __tmp = ((F_CPU) / 1e6) * __us;

    #if defined(__DELAY_ROUND_DOWN__)
        __ticks_dc = (uint32_t)fabs(__tmp);

    #elif defined(__DELAY_ROUND_CLOSEST__)
        __ticks_dc = (uint32_t)(fabs(__tmp)+0.5);

    #else
        __ticks_dc = (uint32_t)(ceil(fabs(__tmp)));
    #endif

    __builtin_avr_delay_cycles(__ticks_dc);

#else
    uint8_t __ticks;
    double __tmp2 ;
    __tmp = ((F_CPU) / 3e6) * __us;
    __tmp2 = ((F_CPU) / 4e6) * __us;
    if (__tmp < 1.0)
        __ticks = 1;
    else if (__tmp2 > 65535)
    {
        _delay_ms(__us / 1000.0);
    }
    else if (__tmp > 255)
    {
        uint16_t __ticks=(uint16_t)__tmp2;
        _delay_loop_2(__ticks);
        return;
    }
    else
        __ticks = (uint8_t)__tmp;
    _delay_loop_1(__ticks);
#endif
}

char *Utility::itoa(int num, char *str, int base)
{
    int i = 0;
    bool isNegative = false;
 
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }
 
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0';
    reverse(str, i);
    return str;
}

void Utility::reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    
    while (start < end)
    {
        swap(*(str + start), *(str + end));
        start++;
        end--;
    }
}

string::string(const char *s)
{
    _capacity = my_strlen(s) + 1;
    this->s = new char[_capacity];
    my_strcpy(this->s, s);
}

void string::append(const string &s)
{
    my_strcpy(this->s + size(), s.c_str());
}

AnalogBase::AnalogBase(uint8_t *base)
  :
    base(base),
    adcl(base),
    adch(base + 1),
    adcsra(base + 2),
    adcsrb(base + 3),
    admux(base + 4)
{
}

Analog::Analog() : AnalogBase((uint8_t *)0x78)
{
    *admux = 0x00 | (1<<BREFS0);
    *adcsra = (1<<BADPS1) | (1<<BADPS0) | (1<<BADEN) | (1<<BADATE);
    *adcsrb &= (1<<BADTS2) | (1<<BADTS1) | (1<<BADTS0);
    *adcsra |= (1<<BADSC);
}

DS1302::DS1302(Pin *clk, Pin *dat, Pin *rst) : clk(clk), dat(dat), rst(rst)
{
    clk->direction(OUTPUT);
    dat->direction(OUTPUT);
    rst->direction(OUTPUT);
    write(ENABLE, 0);
    write(TRICKLE, 0);
}

void DS1302::write(int address, uint8_t data)
{
    address &= ~(1<<0);
    start();
    toggleWrite(address, false);
    toggleWrite(data, false);
    stop();
}

bitset<8> DS1302::toggleReadBitset()
{
    bitset<8> dataBitset;
    
    for (uint8_t i = 0; i <= 7; i++)
    {
        clk->set();
        _delay_us(1);
        clk->clear();
        _delay_us(1);
        dataBitset.set(i, dat->read());
        
    }
    return dataBitset;

}

void DS1302::toggleWrite(uint8_t data, uint8_t release)
{
    for (int i = 0; i <= 7; i++)
    {
        bitset<8> dataBitset(data);
        dat->set(dataBitset.test(i));
        _delay_us(1);
        clk->set();
        _delay_us(1);

        if (release && i == 7)
        {
            dat->direction(INPUT);
        }
            else
        {
            clk->clear();
            _delay_us(1);
        }
    }
}

void DS1302::update()
{
    clockBurstRead((uint8_t *)&regs);
}

void DS1302::clockBurstRead(uint8_t *p)
{
    start();
    toggleWrite(CLOCK_BURST_READ, true);
    
    for (int i = 0; i < 8; i++)
        *p++ = toggleRead();

    stop();
}

void DS1302::clockBurstWrite(uint8_t *p)
{
    start();
    toggleWrite(CLOCK_BURST_WRITE, false);

    for (int i = 0; i < 8; i++)
        toggleWrite(*p++, false);

    stop();
}

void DS1302::start()
{
    rst->clear();
    rst->direction(OUTPUT);
    clk->direction(OUTPUT);
    dat->direction(OUTPUT);
    rst->set();
    _delay_us(4);
}

void DS1302::stop()
{
    rst->clear();
    _delay_us(4);
}

uint8_t DFKeyPad::poll()
{
    unsigned x = adc.read();
    if (x < 100) return RIGHT;
    if (x < 250) return UP;
    if (x < 400) return DOWN;
    if (x < 550) return LEFT;
    if (x < 800) return SELECT;
    return 0;
}




