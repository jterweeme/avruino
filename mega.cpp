#include "mega.h"
#include <avr/interrupt.h>
#include "uart.h"
#include "timer.h"

#if 0
Uart::Uart() : UartBase(p_ubrr9, p_udr9, p_ucsr9a, p_ucsr9b)
{
    instance = this;
}

DefaultUart::DefaultUart()
{
    *brr = 103;
    *ucsrb = (1<<MYTXEN0);
}
#endif

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

Timer1 *Timer1::instance;
Timer0 *Timer0::instance;
//Uart *Uart::instance;

extern "C" void __vector_13() __attribute__ ((signal, used, externally_visible));
extern "C" void __vector_18() __attribute__ ((signal, used, externally_visible));

void __vector_13() {     Timer1::getInstance()->onOverflow(); }
void __vector_18() {    Uart::getInstance()->onReceive(); }


