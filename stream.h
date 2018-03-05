#ifndef _STREAM_H_
#define _STREAM_H_
#include "uart.h"

#if defined (__AVR_ATmega32U4__)
#include "cdc.h"
#endif

class ostream
{
public:
    virtual void put(char c) { }
    virtual void writeString(const char *s) { while (*s) put(*s++); }
    virtual void flush() { }
};

class istream
{
public:
    virtual int get() { return 0; }
};

class ifstream : public istream
{
public:
    virtual int get() { return 0; }
};

class UartStream : public ostream
{
private:
    UartBase * const _serial;
public:
    UartStream(UartBase *s) : _serial(s) { }
    void put(char c) { _serial->write(c); }
};

class UartIStream : public istream
{
private:
    UartBase * const _serial;
public:
    UartIStream(UartBase *s) : _serial(s) { }
    int get() { return _serial->readByte(); }
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



