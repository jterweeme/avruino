/*
PS2 CLK: D2
*/

#include "keyboard.h"
#include "board.h"

void CircBuf::push(uint8_t v)
{
    uint8_t i = head + 1;

    if (i >= BUFFER_SIZE)
        i = 0;

    if (i != tail)
    {
        _buf[i] = v;
        head = i;
    }
}

uint8_t CircBuf::get(uint8_t pos)
{
    if (empty())
        return 0;

    tail = pos;
    return _buf[pos];
}

uint8_t CircBuf::get()
{
    uint8_t i = tail;

    if (++i >= BUFFER_SIZE)
        i = 0;

    return get(i);
}

void MyBitset::addBit(uint8_t val)
{
    if (_bitcount - 1 <= 7)
        _incoming |= val << (_bitcount - 1);

    _bitcount++;
}

PS2Keyboard::PS2Keyboard(Pin *dat) : _dat(dat)
{
    _dat->direction(INPUT);
    _dat->set();
    *p_int0_ddr &= ~(1<<int0_bit);
    *p_int0_port |= int0_bit;
}

void PS2Keyboard::isr()
{
    bool val = _dat->read();

    if (_timeTicks - _prevTicks > 10)
        _bitset.reset();

    _prevTicks = _timeTicks;
    _bitset.addBit(val);

    if (_bitset.count() == 11)
    {
        _buf.push(_bitset.incoming());
        _bitset.reset();
    }
}


