#ifndef _PINPORT_H_
#define _PINPORT_H_
#include "types.h"

enum Bits { BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7 };

struct Port
{
    volatile uint8_t * const pbase;
    volatile uint8_t * const pin;
    volatile uint8_t * const direction;
    volatile uint8_t * const out;
    Port(uint8_t *base) : pbase(base), pin(base), direction(base + 1), out(base + 2) { }
    Port(volatile uint8_t *base) : pbase(base), pin(base), direction(base + 1), out(base + 2) { }
    inline void setBit(Bits bit) { *out |= 1<<bit; }
    inline void clearBit(Bits bit) { *out &= ~(1<<bit); }
    inline void toggleBit(Bits bit) { *out ^= 1<<bit; }
    uint8_t read() { return *pin; }
    void write(uint8_t data) { *out = data; }
};

enum Direction { INPUT, OUTPUT };

struct Pin  // uses Port class
{
    Port &port;
    const Bits bit;
    Pin(Port &port, Bits bit) : port(port), bit(bit) { }
    inline void set() { port.setBit(bit); }
    inline void clear() { port.clearBit(bit); }
    inline void set(bool value) { return value ? set() : clear(); }
    inline void toggle() { port.toggleBit(bit); }
    void direction(Direction dir);
    bool read() { return port.read() & 1<<bit; }
};


#endif



