#ifndef _TIMER_H_
#define _TIMER_H_
#include "types.h"

template <class T> class Timer
{
protected:
    volatile T * const counter;
    volatile uint8_t * const tifr;
    volatile uint8_t * const timsk;
    volatile uint8_t * const tccra;
    volatile uint8_t * const tccrb;
public:
    static const uint8_t CS0 = 1 << 0;
    static const uint8_t CS1 = 1 << 1;
    static const uint8_t CS2 = 1 << 2;
    static const uint8_t CS_MASK = CS0 | CS1 | CS2;

    enum Prescaler
    {
        NO_CLOCK = 0,
        DIV1 = CS0,
        DIV8 = CS1,
        DIV64 = CS0 | CS1,
        DIV256 = CS2,
        DIV1024 = CS0 | CS2,
        EXT1_FALLING = CS1 | CS2,
        EXT1_RISING = CS0 | CS1 | CS2
    };

    Timer(T *counter, uint8_t *tifr, uint8_t *timsk, uint8_t *tccra, uint8_t *tccrb)
        : counter(counter), tifr(tifr), timsk(timsk), tccra(tccra), tccrb(tccrb)
    { }

    virtual void prescale(Prescaler ps) { *tccrb = (*tccrb & ~CS_MASK) | ps; }
    virtual void onOverflow() { }
};

class Timer0 : public Timer<uint8_t>
{
protected:
    static Timer0 *instance;
public:
    Timer0();
    static Timer0 *getInstance() { return instance; }
};

class Timer1 : public Timer<uint16_t>
{
protected:
    static Timer1 *instance;
public:
    Timer1();
    static Timer1 *getInstance() { return instance; }
};

#endif



