#ifndef _MISC_H_
#define _MISC_H_
#include <stdlib.h>
#include "types.h"

struct freelist
{
    size_t sz;
    freelist *nx;
};

class Utility
{
public:
    static inline void strcpy(char *d, const char *s) { while ((*d++ = *s++)); }
    static inline void delay(const uint32_t x) { for (volatile uint32_t i = 0; i < x; i++); }
    template <class T> static void swap(T& a, T& b) { T c(a); a = b; b = c; }
    static void reverse(char str[], int length);
    static int strcmp(const char *a, const char *b);
    static size_t strlen(const char *s);
    static char *itoa(int num, char *str, int base);
    static void *malloc(size_t size) { return ::malloc(size); }
    static int isdigit(int c) { return c >= '0' && c <= '9' ? 1 : 0; }
};

#include "storage.h"

inline void * operator new (size_t size) { return Utility::malloc(size); }
inline void * operator new[] (size_t size) { return Utility::malloc(size); }



enum Bits { BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7 };

struct Port
{
    volatile uint8_t * const pbase;
    volatile uint8_t * const pin;
    volatile uint8_t * const direction;
    volatile uint8_t * const out;
    Port(uint8_t *base) : pbase(base), pin(base), direction(base + 1), out(base + 2) { }
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


