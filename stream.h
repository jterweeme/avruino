#ifndef _STREAM_H_
#define _STREAM_H_
#include "misc.h"

#if defined (__AVR_ATmega32U4__)
#include "cdc.h"
#endif

class ostream
{
public:
    virtual void write(char c) { }
    void write(const char *s) { while (*s) write(*s++); }
    virtual void writeString(const char *s) { while (*s) write(*s++); }
    virtual void flush() { }
};

class UartStream : public ostream
{
private:
    UartBase * const _serial;
public:
    UartStream(UartBase *s) : _serial(s) { }
    void write(char c) { _serial->write(c); }
};

#if defined (__AVR_ATmega32U4__)
class USBStream : public ostream
{
private:
    CDC * const _cdc;
public:
    USBStream(CDC *cdc) : _cdc(cdc) { }
    void write(char c) { _cdc->sendByte(c); }
    void flush() { _cdc->flush(); }
};
#endif

#endif



