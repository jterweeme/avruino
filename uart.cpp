#include "uart.h"
#include "board.h"

UartBase::UartBase(volatile uint16_t *brr, volatile uint8_t *udr,
    volatile uint8_t *ucsra, volatile uint8_t *ucsrb)
  :
    _brr(brr),
    udr(udr),
    ucsra(ucsra),
    ucsrb(ucsrb)
{
}

int16_t UartBase::get(uint32_t timeout)
{
    for (uint32_t i = 0; i <= timeout; i++)
        if (*ucsra & 1<<rxc9)
            return *udr;

    return -1;
}

Uart::Uart() : UartBase(p_ubrr9, p_udr9, p_ucsr9a, p_ucsr9b)
{
    instance = this;
}

Uart *Uart::instance;

DefaultUart::DefaultUart()
{
    *_brr = 16;
    *p_ucsr9a |= 1<<u2x9;
    *p_ucsr9b = 1<<txen9;
}



