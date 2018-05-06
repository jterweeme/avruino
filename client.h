#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "stream.h"
#include "misc.h"

class Client : public ostream
{
public:
    virtual int connect(uint32_t ip, uint16_t port) = 0;
    virtual size_t write(const uint8_t *buf, size_t n) = 0;
    void write(const char *buf, size_t n) { write((uint8_t *)buf, n); }
    void put(char c) { write((uint8_t *)&c, 1); }
    void operator<< (const char *s) { write((uint8_t *)s, my_strlen(s)); }
    virtual int available() = 0;
    virtual int read() = 0;
    virtual int read(uint8_t *buf, size_t size) = 0;
    virtual int peek() = 0;
    virtual void flush() = 0;
    virtual void stop() = 0;
    virtual uint8_t connected() = 0;
    virtual operator bool() = 0;
};
#endif



