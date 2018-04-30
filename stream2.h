#ifndef _STREAM2_H_
#define _STREAM2_H_
#include "misc.h"

class Stream
{
protected:
    uint32_t _timeout;
    uint32_t _startMillis;
    int timedRead();
    int timedPeek();
    int peekNextDigit();
public:
    virtual size_t write(uint8_t) { return 0; }

    size_t write(const char *str)
    {
        if (str == 0)
            return 0;
        return write((const uint8_t *)str, my_strlen(str)); 
    }

    virtual size_t write(const uint8_t *buffer, size_t size)
    {
        size_t n = 0;

        while (size--)
            n += write(*buffer++);
    
        return n;
    }

    virtual int available() = 0;
    virtual int read() = 0;
    virtual int peek() = 0;
    virtual void flush() = 0;
#if 0
    Stream() {_timeout=1000;}
    void setTimeout(unsigned long timeout);  // sets o wait for stream daulcond
    bool find(char *target);   // reads data from the stream until the tinfound
    bool find(char *target, size_t length);   // reads et string of giveis found
    size_t readBytes( char *buffer, size_t length); // read chars from sinto
    size_t readBytesUntil( char terminator, char *buffer, size_t length);
#endif
};
#endif



