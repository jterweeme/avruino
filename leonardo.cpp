#include "board.h"
#include "misc.h"
#include <avr/io.h>
#include "uart.h"

#if 0
Timer0 *Timer0::instance;
Timer1 *Timer1::instance;

Timer0::Timer0()
  :
    Timer((uint8_t *)0x46, (uint8_t *)0x35, (uint8_t *)0x6e, (uint8_t *)0x44, (uint8_t *)0x45)
{
    instance = this;
    *tifr = 0;
    *tccrb |= (1<<CS02) | (1<<CS00);
    *timsk |= (1<<TOIE0);
    asm volatile ("sei");
}

Timer1::Timer1()
  :
    Timer((uint16_t *)0x84, (uint8_t *)0x36, (uint8_t *)0x6f, (uint8_t *)0x80, (uint8_t *)0x81)
{
    instance = this;
    *tifr = 0;
    *tccrb |= (1<<CS12) | (1<<CS10);
    *timsk |= (1<<TOIE1);
    asm volatile ("sei");
}

extern "C" void __vector_20() __attribute__ ((signal, used, externally_visible));
extern "C" void __vector_25() __attribute__ ((signal, used, externally_visible));

void __vector_20() { Timer1::getInstance()->onOverflow(); }
void __vector_25() { Uart::getInstance()->onReceive(); }
#endif



