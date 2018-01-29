#ifndef _CAPSENSE_H_
#define _CAPSENSE_H_

#include <stdint.h>

class CapSense
{
private:
    volatile uint8_t * const _base;
    volatile uint8_t * const _in;
    volatile uint8_t * const _mode;
    volatile uint8_t * const _out;
    volatile uint8_t const _bit;
    uint16_t _loopTimingFactor;
    int16_t _error;
    uint32_t _leastTotal;
    uint32_t _timeout;
    uint32_t _autocal;
    uint32_t _lastCal;
    uint32_t _total;
public:
    uint32_t senseRaw(uint8_t samples);
    int16_t senseOneCycle();
    CapSense(volatile uint8_t * const base, uint8_t bit);
};

#endif



