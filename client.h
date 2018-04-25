#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "stream2.h"

class Client : public Stream
{
public:
    virtual int connect(uint32_t ip, uint16_t port) { return 0; }
    virtual size_t write(const uint8_t *buf, size_t size) { return 0; }
    virtual size_t write(uint8_t b) { return write(&b, 1); }
    void put(char c) { write((uint8_t)c); }
    void operator<< (const char *s) { write((uint8_t *)s, strlen(s)); }
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



