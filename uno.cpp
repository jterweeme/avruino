#define F_CPU 16000000UL
#include "uno.h"
#include "misc.h"
#include <avr/interrupt.h>

Uart::Uart()
  :
    UartBase((uint16_t *)ubrr0, (uint8_t *)udr0, (uint8_t *)ucsr0a, (uint8_t *)ucsr0b)
{
    instance = this;
}

DefaultUart::DefaultUart()
{
    *brr = 103;
    *ucsrb = (1<<MYTXEN0);
}

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
Uart *Uart::instance;

extern "C" void __vector_13() __attribute__ ((signal, used, externally_visible));
//extern "C" void __vector_16() __attribute__ ((signal, used, externally_visible));
extern "C" void __vector_18() __attribute__ ((signal, used, externally_visible));

void __vector_13() { Timer1::getInstance()->onOverflow(); }

static volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

unsigned long millis()
{
    unsigned long m;
    uint8_t oldSREG = SREG;
    cli();
    m = timer0_millis;
    SREG = oldSREG;
    return m;
}

#if 0
void __vector_16()
{
    unsigned long m = timer0_millis;
    unsigned char f = timer0_fract;
    m += MILLIS_INC;
    f += FRACT_INC;

    if (f >= FRACT_MAX)
    {
        f -= FRACT_MAX;
        m += 1;
    }

    timer0_fract = f;
    timer0_millis = m;
    timer0_overflow_count++;
    Timer0::getInstance()->onOverflow();
}
#endif

void __vector_18() { Uart::getInstance()->onReceive(); }

SPIBus::SPIBus() : SPIBase((uint8_t *)0x4c)
{
    PORTB |= 1 << 2;    // SS high
    DDRB |= 1 << 2;
    *spcr = spcr_bits::spe | spcr_bits::mstr | spcr_bits::spr1 | spcr_bits::spr0;
    *spsr &= ~spsr_bits::spi2x;


    
}

