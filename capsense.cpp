#include "capsense.h"
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include "board.h"
#include <util/delay.h>

CapSense::CapSense(Pin send, Pin receive, uint16_t threshold)
  :
    _send_mode(send.port.direction),
    _send_bit(send.bit),
    _base(receive.port.pbase),
    _in(receive.port.pin),
    _mode(receive.port.direction),
    _out(receive.port.out),
    _bit(receive.bit),
    _threshold(threshold)
{
    _timeout = 2000 * _loopTimingFactor;
    *_send_mode |= 1<<_send_bit;
    *_mode &= ~(1<<_bit);
    *_out &= ~(1<<_bit);
}

int16_t CapSense::senseOneCycle()
{
    cli();
    *_out &= ~(1<<_bit);
    *_mode &= ~(1<<_bit);
    *_mode |= ~(1<<_bit);
    *_out &= ~(1<<_bit);
    _delay_us(10);
    *_mode &= ~(1<<_bit);
    *_out |= 1<<_bit;
    sei();

    while ((*_in & 1<<_bit) == 0 && _total < _timeout)
        _total++;

    if (_total > _timeout)
        return -2;

    cli();
    *_out |= 1<<_bit;
    *_mode |= 1<<_bit;
    *_out |= 1<<_bit;
    *_mode &= ~(1<<_bit);
    *_out &= ~(1<<_bit);
    sei();

    while ((*_in & 1<<_bit) && _total < _timeout)
        _total++;

    return _total >= _timeout ? -1 : 1;
}

uint32_t CapSense::senseRaw(uint8_t samples)
{
    _total = 0;

    for (uint8_t i = 0; i < samples; i++)
        if (senseOneCycle() < 0)
            return -2;

    return _total;
}



