#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "uno.h"
#include "misc.h"
#include "timer.h"
#include "uart.h"

Timer0::Timer0()
  :
    Timer((uint8_t *)p_tcnt0, (uint8_t *)0x35, (uint8_t *)0x6e, (uint8_t *)0x44, (uint8_t *)0x45)
{
    instance = this;
    *tifr = 0;
    *tccrb |= 1<<cs02 | 1<<cs00;
    *timsk |= 1<<toie0;
    asm volatile ("sei");
}

Timer1::Timer1()
  :
    Timer((uint16_t *)0x84, (uint8_t *)0x36, (uint8_t *)0x6f, (uint8_t *)0x80, (uint8_t *)0x81)
{
    instance = this;
    *tifr = 0;
    *tccrb |= 1<<cs12 | 1<<cs10;
    *timsk |= 1<<toie1;
    asm volatile ("sei");
}

Timer1 *Timer1::instance;
Timer0 *Timer0::instance;
//Uart *Uart::instance;

static volatile unsigned long timer0_millis = 0;



