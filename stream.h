#ifndef _STREAM_H_
#define _STREAM_H_
#include "uart.h"

#if defined (__AVR_ATmega32U4__)
#include "cdc.h"
#elif defined (__AVR_ATmega16U2__)
#include "cdc.h"
#endif

class ostream
{
public:
    virtual void put(char c) { }
    virtual void write(const char *s, size_t n);
    virtual void operator<< (const char *s) { while (*s) put(*s++); }
    virtual void flush() { }
};

static inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
}

static inline void hex8(uint8_t v, ostream &os)
{
    os.put(nibble(v >> 4 & 0xf));
    os.put(nibble(v >> 0 & 0xf));
}

static inline void hex16(uint16_t v, ostream &os)
{
    hex8(v >> 8 & 0xff, os);
    hex8(v >> 0 & 0xff, os);
}

static inline void hex32(uint32_t v, ostream &os)
{
    hex16(v >> 16 & 0xffff, os);
    hex16(v >> 0 & 0xffff, os);
}

class istream
{
protected:
    size_t _lastread = 0;
public:
    virtual int16_t peek() { return 0; }
    virtual int16_t get() { return 0; }
    virtual int16_t get(uint32_t timeout) { return 0; }
    virtual void read(char *s, size_t n);
    virtual size_t gcount() { return _lastread; }
};

class ifstream : public istream
{
public:
    virtual int16_t get() { return 0; }
};

class ofstream : public ostream
{
public:
    virtual void open(const char *fn) { }
    virtual void close() { }
};

class UartStream : public ostream
{
private:
    UartBase * const _serial;
public:
    UartStream(UartBase *s) : _serial(s) { }
    void put(char c) { _serial->myPutc(c); }
    void writeString(const char *s) { while (*s) put(*s++); } // deprecated
};

class UartIStream : public istream
{
private:
    UartBase * const _serial;
public:
    UartIStream(UartBase *s) : _serial(s) { }
    int get() { return _serial->readByte(); }
    int16_t get(uint32_t timeout) { return _serial->get(timeout); }
};

#if defined (__AVR_ATmega32U4__)
class USBStream : public ostream
{
private:
    CDC * const _cdc;
public:
    USBStream(CDC *cdc) : _cdc(cdc) { }
    void put(char c) { _cdc->sendByte(c); }
    void flush() { _cdc->flush(); }
};

class USBIStream : public istream
{
private:
    CDC * const _cdc;
public:
    USBIStream(CDC *cdc) : _cdc(cdc) { }
    int get() { return _cdc->receive(); }
};
#endif

#endif



