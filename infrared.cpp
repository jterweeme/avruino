#include "infrared.h"
#include <avr/interrupt.h>

#define F_CPU 16000000UL

static constexpr uint32_t SYSCLOCK = F_CPU;
static constexpr uint8_t USECPERTICK = 50;
static constexpr uint8_t MARK = 0;
static constexpr uint8_t SPACE = 1;
static constexpr uint8_t MARK_EXCESS = 100;
static constexpr uint8_t RAWBUF = 101;

static constexpr uint8_t STATE_IDLE = 2;
static constexpr uint8_t STATE_MARK = 3;
static constexpr uint8_t STATE_SPACE = 4;
static constexpr uint8_t STATE_STOP = 5;
static constexpr uint8_t STATE_OVERFLOW = 6;

static constexpr uint8_t TOLERANCE = 25;
#define LTOL            (1.0 - (TOLERANCE/100.))
#define UTOL            (1.0 + (TOLERANCE/100.))


static const uint16_t _GAP = 5000;
static const uint32_t GAP_TICKS = _GAP/USECPERTICK;

static constexpr int TICKS_LOW(uint32_t us)
{
    return us * LTOL / USECPERTICK;
}

static constexpr int TICKS_HIGH(uint32_t us)
{
    return us * UTOL / USECPERTICK + 1;
}

typedef
    struct {
        uint8_t       rcvstate;        // State Machine state
        uint8_t       recvpin;         // Pin connected to IR data from detector
        uint8_t       blinkpin;
        uint8_t       blinkflag;       // true -> enable blinking of pin on IR processing
        uint8_t       rawlen;          // counter of entries in rawbuf
        unsigned int  timer;           // State timer, counts 50uS ticks.
        unsigned int  rawbuf[RAWBUF];  // raw data
        uint8_t       overflow;        // Raw buffer overflow occurred
    }
irparams_t;


volatile irparams_t irparams;

int  MATCH       (int measured, int desired) ;
int  MATCH_MARK  (int measured_ticks, int desired_us) ;
int  MATCH_SPACE (int measured_ticks, int desired_us) ;

int MATCH(int measured, int desired)
{
    return measured >= TICKS_LOW(desired) && measured <= TICKS_HIGH(desired);
}

int MATCH_MARK(int measured_ticks, int desired_us)
{
    bool passed = ((measured_ticks >= TICKS_LOW (desired_us + MARK_EXCESS))
                && (measured_ticks <= TICKS_HIGH(desired_us + MARK_EXCESS)));
    
    return passed;
}

int MATCH_SPACE(int measured_ticks,  int desired_us)
{
    bool passed = ((measured_ticks >= TICKS_LOW (desired_us - MARK_EXCESS))
                && (measured_ticks <= TICKS_HIGH(desired_us - MARK_EXCESS)));

    return passed;
}

ISR (TIMER1_COMPA_vect)
{
    uint8_t irdata = PINC & 1<<5 ? 1 : 0;
    irparams.timer++;

    if (irparams.rawlen >= RAWBUF)
        irparams.rcvstate = STATE_OVERFLOW;

    switch(irparams.rcvstate)
    {
        case STATE_IDLE:
            if (irdata == MARK)
            {
                if (irparams.timer < GAP_TICKS)
                {  // Not big enough to be a gap.
                    irparams.timer = 0;
                }
                else
                {
                    irparams.overflow                  = false;
                    irparams.rawlen                    = 0;
                    irparams.rawbuf[irparams.rawlen++] = irparams.timer;
                    irparams.timer                     = 0;
                    irparams.rcvstate                  = STATE_MARK;
                }
            }
            break;
        case STATE_MARK:  // Timing Mark
            if (irdata == SPACE) {   // Mark ended; Record time
                irparams.rawbuf[irparams.rawlen++] = irparams.timer;
                irparams.timer                     = 0;
                irparams.rcvstate                  = STATE_SPACE;
            }
            break; 
        case STATE_SPACE:  // Timing Space
            if (irdata == MARK) {  // Space just ended; Record time
                irparams.rawbuf[irparams.rawlen++] = irparams.timer;
                irparams.timer                     = 0;
                irparams.rcvstate                  = STATE_MARK;

            } else if (irparams.timer > GAP_TICKS) {  // Space
                    irparams.rcvstate = STATE_STOP;
            }
            break;
        case STATE_STOP:  // Waiting; Measuring Gap
            if (irdata == MARK)  irparams.timer = 0 ;  // Reset gap timer
            break;
        case STATE_OVERFLOW:  // Flag up a read overflow; Stop the State Machine
            irparams.overflow = true;
            irparams.rcvstate = STATE_STOP;
            break;
    }
}

int IRrecv::decode (decode_results *results)
{
    results->rawbuf   = irparams.rawbuf;
    results->rawlen   = irparams.rawlen;
    results->overflow = irparams.overflow;

    if (irparams.rcvstate != STATE_STOP)
        return false ;

    if (decodeHash(results))
        return true ;

    resume();
    return false;
}

IRrecv::IRrecv (int recvpin)
{
    irparams.recvpin = recvpin;
    irparams.blinkflag = 0;
}

IRrecv::IRrecv (int recvpin, int blinkpin)
{
    irparams.recvpin = recvpin;
    irparams.blinkpin = blinkpin;
    irparams.blinkflag = 0;
}

void IRrecv::enableIRIn()
{
    cli();
    TCCR1A = 0;
    TCCR1B = 1<<WGM12 | 1<<CS10;
    OCR1A  = SYSCLOCK * USECPERTICK / 1000000;
    TCNT1  = 0;
    TIMSK1 = 1<<OCIE1A;
    sei();
    irparams.rcvstate = STATE_IDLE;
    irparams.rawlen = 0;
    DDRC &= ~(1<<5);
}

bool IRrecv::isIdle()
{
    return irparams.rcvstate == STATE_IDLE || irparams.rcvstate == STATE_STOP ? true : false;
}

void  IRrecv::resume()
{
    irparams.rcvstate = STATE_IDLE;
    irparams.rawlen = 0;
}

int  IRrecv::compare(unsigned int oldval,  unsigned int newval)
{
    if      (newval < oldval * .8)  return 0 ;
    else if (oldval < newval * .8)  return 2 ;
    else                            return 1 ;
}

static constexpr uint32_t FNV_PRIME_32 = 16777619, FNV_BASIS_32 = 2166136261;

long IRrecv::decodeHash (decode_results *results)
{
    long hash = FNV_BASIS_32;

    if (results->rawlen < 6)  return false;

    for (int i = 1; (i + 2) < results->rawlen;  i++)
    {
        int value = compare(results->rawbuf[i], results->rawbuf[i+2]);
        hash = (hash * FNV_PRIME_32) ^ value;
    }

    results->value = hash;
    results->bits = 32;
    results->decode_type = UNKNOWN;

    return true;
}


