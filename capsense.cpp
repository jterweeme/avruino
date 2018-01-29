#include "capsense.h"
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include "board.h"
#include <util/delay.h>

CapSense::CapSense(volatile uint8_t * const base, uint8_t bit)
  :
    _base(base), _in(base), _mode(base + 1), _out(base + 2), _bit(bit)
{
    _error = 1;
    _loopTimingFactor = 310;
    _timeout = 2000 * _loopTimingFactor;
    _autocal = 20000;
    *p_ddrd |= 1<<4;
    *_mode &= ~(1<<_bit);
    *_out &= ~(1<<_bit);
    _leastTotal = 0x0fffffff;
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

    if (samples == 0)
        return 0;

    for (uint8_t i = 0; i < samples; i++)
        if (senseOneCycle() < 0)
            return -2;

    return _total;
}



