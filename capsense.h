#ifndef _CAPSENSE_H_
#define _CAPSENSE_H_
#include "misc.h"

class CapSense
{
private:
    volatile uint8_t * const _send_mode;
    volatile uint8_t const _send_bit = 4;
    volatile uint8_t * const _base;
    volatile uint8_t * const _in;
    volatile uint8_t * const _mode;
    volatile uint8_t * const _out;
    volatile uint8_t const _bit;
    const uint16_t _threshold = 0;
    uint16_t _loopTimingFactor = 310;
    int16_t _error = 1;
    uint32_t _leastTotal = 0x0fffffff;
    uint32_t _timeout;
    uint32_t _autocal = 20000;
    uint32_t _lastCal;
    uint32_t _total;
public:
    uint32_t senseRaw(uint8_t samples);
    int16_t senseOneCycle();
    uint16_t threshold() const { return _threshold; }
    CapSense(Pin send, Pin receive, uint16_t threshold = 0);
};

#endif



