#ifndef _MISC_H_
#define _MISC_H_
#include <stdlib.h>
#include "storage.h"
#include "pinport.h"

static inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
}

static inline void *my_memcpy(void *dst, void *src, size_t n)
{
    uint8_t *csrc = (uint8_t *)src;
    uint8_t *cdst = (uint8_t *)dst;

    for (size_t i = 0; i < n; i++)
        cdst[i] = csrc[i];

    return dst;
}

static inline int my_memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p1 = (uint8_t *)s1, *p2 = (uint8_t *)s2;

    while (n--)
    {
        if (*p1 != *p2)
            return *p1 - *p2;

        p1++, p2++;
    }

    return 0;
}

static inline bool my_isupper(char c)
{
    return c >= 'A' && c <= 'Z';
}

static inline bool my_islower(char c)
{
    return c >= 'a' && c <= 'z';
}

static inline int my_tolower(int c)
{
    return my_isupper(c) ? c + 32 : c;
}

static inline int my_toupper(int c)
{
    return my_islower(c) ? c - 32 : c;
}

static inline void *my_memset(void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *)s;
    
    while (n--)
        *p++ = (uint8_t)c;

    return s;
}

static inline int my_strcasecmp(const char *a, const char *b, size_t n)
{
    for (;; a++, b++)
    {
        int d = my_tolower(*a) - my_tolower(*b);

        if (d != 0 || !*a)
            return d;
    }

    return 0;
}

static inline int my_strncasecmp(const char *a, const char *b, size_t n)
{
    while (n--)
        if (my_tolower(*a++) != my_tolower(*b++))
            return my_tolower(*a) - my_tolower(*b);

    return 0;
}

static inline char *my_strchr(const char *s, int c)
{
    while (*s != (char)c)
        if (!*s++)
            return 0;

    return (char *)s;
}

static inline char *my_strncpy(char *dst, const char *src, size_t n)
{
    size_t i;

    for (i = 0; i < n && src[i] != '\0'; i++)
        dst[i] = src[i];

    for (; i < n; i++)
        dst[i] = '\0';

    return dst;
}

static inline bool my_isspace(char c)
{
    return c == ' ';
}

static inline int my_isdigit(int c)
{
    return c >= '0' && c <= '9' ? 1 : 0;
}

static inline bool my_isprint(char c)
{
    return c >= 0x20 && c <= 0x7e;
}

class Utility
{
public:
    static inline void delay(const uint32_t x) { for (volatile uint32_t i = 0; i < x; i++); }
    template <class T> static void swap(T& a, T& b) { T c(a); a = b; b = c; }
    static void reverse(char str[], int length);
    static char *itoa(int num, char *str, int base);
    static void *malloc(size_t size) { return ::malloc(size); }
};

inline void * operator new (size_t size) { return Utility::malloc(size); }
inline void * operator new[] (size_t size) { return Utility::malloc(size); }

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


