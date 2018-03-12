#ifndef _MISC_H_
#define _MISC_H_
#include <stdlib.h>
#include "types.h"
#include "storage.h"
#include "pinport.h"

inline void * operator new (size_t size) { return Utility::malloc(size); }
inline void * operator new[] (size_t size) { return Utility::malloc(size); }

static inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
}

static inline bool myIsspace(char c)
{
    return c == ' ';
}

static inline bool myIsprint(char c)
{
    return c >= 0x20 && c <= 0x7e;
}

class SPIBase
{
public:
    volatile uint8_t * const base;
    volatile uint8_t * const spcr;
    volatile uint8_t * const spsr;
    volatile uint8_t * const spdr;
    SPIBase(uint8_t *base) : base(base), spcr(base), spsr(base + 1), spdr(base + 2) { }
    uint8_t transfer(uint8_t data);
    uint8_t read() { return transfer(0xff); }

    enum spsr_bits
    {
        spi2x = 1 << 0,
        wcol = 1 << 6,
        spif = 1 << 7
    };

    enum spcr_bits
    {
        spr0 = 1 << 0,
        spr1 = 1 << 1,
        cpha = 1 << 2,
        cpol = 1 << 3,
        mstr = 1 << 4,
        dord = 1 << 5,
        spe = 1 << 6,
        spie = 1 << 7
    };
};

struct DS1302_Regs
{
    uint8_t seconds:4;
    uint8_t seconds10:3;
    uint8_t CH:1;
    uint8_t minutes:4;
    uint8_t minutes10:3;
    uint8_t reserved1:1;

    union
    {
        struct
        {
            uint8_t hour:4;
            uint8_t hour10:2;
            uint8_t reserved2:1;
            uint8_t hour_12_24:1;
        } h24;

        struct
        {
            uint8_t hour:4;
            uint8_t hour10:1;
            uint8_t am_pm:1;
            uint8_t reserved2:1;
            uint8_t hour_12_24:1;
        } h12;
    };

    uint8_t date:4;
    uint8_t date10:2;
    uint8_t reserved3:2;
    uint8_t month:4;
    uint8_t month10:1;
    uint8_t reserved4:3;
    uint8_t day:3;
    uint8_t reserved5:5;
    uint8_t year:4;
    uint8_t year10:4;
    uint8_t reserved6:7;
    uint8_t wp:1;
} __attribute__ ((packed));

class DS1302
{
    Pin *clk;
    Pin *dat;
    Pin *rst;
    DS1302_Regs regs;
public:
    DS1302(Pin *clk, Pin *dat, Pin *rst);
    void update();
    void toggleWrite(uint8_t data, uint8_t release);
    static const uint8_t CLOCK_BURST_READ = 0xbf;
    static const uint8_t CLOCK_BURST_WRITE = 0xbe;
    static const uint8_t ENABLE = 0x8e;
    static const uint8_t TRICKLE = 0x90;
    static const uint8_t READBIT = 0;
    void stop();
    void start();
    void clockBurstRead(uint8_t *p);
    void clockBurstWrite(uint8_t *p);
    void write(int address, uint8_t data);
    bitset<8> toggleReadBitset();
    uint8_t toggleRead() { return toggleReadBitset().data; }
    DS1302_Regs get() { return regs; }
};

class WaveShare8Seg
{
    Pin *ct1;
    Pin *ct2;
    Pin *ct3;
    Pin *ct4;
    Pin *pt;
    Pin *a;
    Pin *b;
    Pin *c;
    Pin *d;
    Pin *e;
    Pin *f;
    Pin *g;
    Pin *dot;
public:
    WaveShare8Seg() { }
};

#endif


