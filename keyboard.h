#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#include "pinport.h"

// moet nog naar RingBuf veranderd worden
class CircBuf
{
    static const uint8_t BUFFER_SIZE = 5;
    volatile uint8_t _buf[BUFFER_SIZE];
public:
    volatile uint8_t head = 0, tail = 0;
    void push(uint8_t v);
    bool empty() const { return head == tail; }
    uint8_t get(uint8_t pos);
    uint8_t get();
};

class MyBitset
{
    uint8_t _bitcount = 0, _incoming = 0;
public:
    inline void reset() { _bitcount = _incoming = 0; }
    void addBit(uint8_t val);
    uint8_t count() const { return _bitcount; }
    uint8_t incoming() const { return _incoming; }
};

class PS2Keyboard
{
private:
    MyBitset _bitset;
    CircBuf _buf;
    uint32_t _timeTicks = 0;
    uint32_t _prevTicks = 0;
    Pin * const _dat;
public:
    PS2Keyboard(Pin *dat);
    void timeTick() { _timeTicks++; }
    void isr();
    uint8_t get_scan_code() { return _buf.get(); }
};
#endif



