#ifndef _MISC_H_
#define _MISC_H_
#include <stdlib.h>

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long int uint32_t;

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
};

inline void * operator new (size_t size) { return Utility::malloc(size); }
inline void * operator new[] (size_t size) { return Utility::malloc(size); }

template <size_t N> class bitset
{
public:
    uint8_t data;
    bitset() : data(0) { }
    bitset(uint8_t data) : data(data) { }
    bool test(size_t pos) const { return data & (1<<pos); }
    void set() { data = 0xff; }
    void set(size_t pos, bool val = true) { if (val) data |= 1 << pos; else data &= ~(1<<pos); }
    unsigned long to_ulong() const { return data; }
};

template <class T> class vector
{
protected:
    size_t capacity;
    size_t size;
    T *buffer;
public:
    typedef T *iterator;
    size_t getSize() const { return size; }
    vector(size_t capacity) : capacity(capacity), size(0) { buffer = new T[capacity]; }
    void push_back(const T &value) { buffer[size++] = value; }
    iterator begin() { return buffer; }
    iterator end() { return buffer + getSize(); }
    T & operator [] (unsigned index) { return buffer[index]; }
};

class string
{
    char *s;
    size_t _capacity;
public:
    string(const char *s);
    size_t inline capacity() { return _capacity; }
    size_t inline size() const { return Utility::strlen(s); }
    size_t inline length() { return size(); }
    const char *c_str() const { return s; }
    void reserve(size_t n) { _capacity += n; ::realloc(s, _capacity); }
    void append(const char *s);
    void append(const string &s);
};

enum Bits { BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7 };

class Port
{
protected:
    volatile uint8_t * const pbase;
    volatile uint8_t * const pin;
public:
    volatile uint8_t * const direction;
    volatile uint8_t * const out;
    Port(uint8_t *base) : pbase(base), pin(base), direction(base + 1), out(base + 2) { }
    inline void setBit(Bits bit) { *out |= 1<<bit; }
    inline void clearBit(Bits bit) { *out &= ~(1<<bit); }
    inline void toggleBit(Bits bit) { *out ^= 1<<bit; }
    uint8_t read() { return *pin; }
    void write(uint8_t data) { *out = data; }
};

struct PortB : public Port { PortB() : Port((uint8_t *)0x23) { } };
struct PortC : public Port { PortC() : Port((uint8_t *)0x26) { } };
struct PortD : public Port { PortD() : Port((uint8_t *)0x29) { } };

enum Direction { INPUT, OUTPUT };

struct Pin
{
    Port &port;
    const Bits bit;
    Pin(Port &port, Bits bit) : port(port), bit(bit) { }
    inline void set() { port.setBit(bit); }
    inline void clear() { port.clearBit(bit); }
    inline void set(bool value) { return value ? set() : clear(); }
    inline void toggle() { port.toggleBit(bit); }
    void direction(Direction dir);
    bool read() { return port.read() & (1<<bit); }
};

class Terminal
{
public:
    virtual void myPutc(char c) { }
    virtual void puts(const char *s) { while (*s) myPutc(*s++); }
    virtual void printf(const char *format, ...);
    virtual uint8_t readByte() { return 0; }
    void operator<< (const char *s) { puts(s); }
};

class UartBase : public Terminal
{
protected:
    volatile uint16_t * const brr;
    volatile uint8_t * const udr;
    volatile uint8_t * const ucsra;
    volatile uint8_t * const ucsrb;
public:
    enum sra
    {
        MUDRE = 1 << 5,
        MRXC = 1 << 7
    };

    enum srb
    {
        TXEn = 1 << 3,
        RXEn = 1 << 4,
        udrie = 1 << 5,
        txcie = 1 << 6,
        rxcie = 1 << 7
    };

    UartBase(uint16_t *brr, uint8_t *udr, uint8_t *ucsra, uint8_t *ucsrb);
    void myPutc(char c) { while (!(*ucsra & sra::MUDRE)) { } *udr = c; }
    virtual void onReceive() { }
    void inline enableTransmit() { *ucsrb |= srb::TXEn; }
    void inline enableRead() { *ucsrb |= srb::RXEn; }
    void inline enableReadInterrupt() { *ucsrb |= srb::rxcie; asm volatile ("sei"); }
    uint8_t readByte() { while (!(*ucsra & sra::MRXC)) { } return *udr; }
};

class AnalogBase
{
protected:
    volatile uint8_t * const base;
    volatile uint8_t * const adcl;
    volatile uint8_t * const adch;
    volatile uint8_t * const adcsra;
    volatile uint8_t * const adcsrb;
    volatile uint8_t * const admux;
public:
    static const uint8_t BREFS0 = 6;
    static const uint8_t BREFS1 = 7;
    static const uint8_t BADPS0 = 0;
    static const uint8_t BADPS1 = 1;
    static const uint8_t BADPS2 = 2;
    static const uint8_t BADEN = 7;
    static const uint8_t BADATE = 5;
    static const uint8_t BADTS0 = 0;
    static const uint8_t BADTS1 = 1;
    static const uint8_t BADTS2 = 2;
    static const uint8_t BADSC = 6;
    static const uint8_t RREFS0 = 1 << BREFS0;
    AnalogBase(uint8_t *base);
    unsigned read() { return *adcl | ((unsigned)*adch << 8); }
};

class Analog : public AnalogBase
{
public:
    Analog();
};

class LCD : public Terminal
{
protected:
    Pin * const rs;
    Pin * const e;
    Pin * const d4;
    Pin * const d5;
    Pin * const d6;
    Pin * const d7;
    void lcd_write_4(uint8_t theByte);
public:
    static const uint8_t LINEONE = 0x00;
    static const uint8_t LINETWO = 0x40;
    static const uint8_t CLEAR = 0x01;
    static const uint8_t HOME = 0x02;
    static const uint8_t ENTRYMODE = 0x06;
    static const uint8_t OFF = 0x08;
    static const uint8_t ON = 0x0c;
    static const uint8_t RESET = 0x30;
    static const uint8_t FOURBIT = 0x28;
    static const uint8_t SETCURSOR = 0x80;
    static const uint8_t CURSOR_HOME = 0x02;
    LCD(Pin *rs, Pin *e, Pin *d4, Pin *d5, Pin *d6, Pin *d7);
    void myPutc(char c);
    void puts(const char *s);
    void lcd_write_instruction_4d(uint8_t theInstruction);
    void home();
    void clear() { lcd_write_instruction_4d(CLEAR); }
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

class I2CBus
{
protected:
    volatile uint8_t * const twbr;
    volatile uint8_t * const twsr;
    volatile uint8_t * const twar;
    volatile uint8_t * const twdr;
    volatile uint8_t * const twcr;
    static const uint8_t TW_START = 0x08;
    static const uint8_t BTWIE = 0;
    static const uint8_t BTWEN = 2;
    static const uint8_t BTWSTO = 4;
    static const uint8_t BTWSTA = 5;
    static const uint8_t BTWEA = 6;
    static const uint8_t BTWINT = 7;
    static const uint8_t RTWIE = 1 << BTWIE;
    static const uint8_t RTWEN = 1 << BTWEN;
    static const uint8_t RTWSTO = 1 << BTWSTO;
    static const uint8_t RTWSTA = 1 << BTWSTA;
    static const uint8_t RTWEA = 1 << BTWEA;
    static const uint8_t RTWINT = 1 << BTWINT;
public:
    vector<uint8_t> slaves;
    I2CBus(unsigned brr = 72);
    uint8_t start();
    uint8_t restart();
    void stop();
    uint8_t write(uint8_t data);
    uint8_t read(uint8_t ack);
    void scan();
};

class Uart : public UartBase
{
    static Uart *instance;
public:
    Uart();
    static Uart *getInstance() { return instance; }
    static const uint8_t MYTXEN0 = 3;
    static const uint8_t MYTXEN1 = 3;
};

class DefaultUart : public Uart
{
public:
    DefaultUart();
};

class RTC
{
    
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

struct PCF8563_Regs
{
    uint8_t control_status_1;
    uint8_t control_status_2;
    uint8_t vl_seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t days;
    uint8_t weekdays;
    uint8_t century_months;
    uint8_t years;
    uint8_t minute_alarm;
    uint8_t hour_alarm;
    uint8_t day_alarm;
    uint8_t weekday_alarm;
    uint8_t clkout_control;
    uint8_t timer_control;
    uint8_t timer;
} __attribute__ ((packed));

class PCF8563
{
    I2CBus * const bus;
    PCF8563_Regs regs;
public:
    static const uint8_t ADDR = 0x51;
    PCF8563(I2CBus * const bus) : bus(bus) { }
    void gettime(Terminal *uart);
    void gettime() { gettime(Uart::getInstance()); }
};

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

class DFKeyPad
{
    AnalogBase &adc;
public:
    static const uint8_t SELECT = 1;
    static const uint8_t UP = 2;
    static const uint8_t LEFT = 3;
    static const uint8_t RIGHT = 4;
    static const uint8_t DOWN = 5;
    DFKeyPad(AnalogBase &adc) : adc(adc) { }
    uint8_t poll();
};

class AT45DBXX
{
public:
    AT45DBXX() { }
};

class Wifi
{
    Uart * const uart;
public:
    Wifi(Uart *uart) : uart(uart) { }
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

class SD
{
    SPIBase *spiBus;
public:
    SD(SPIBase *spiBus) : spiBus(spiBus) { }
};

class LedMatrix
{
};

#endif


