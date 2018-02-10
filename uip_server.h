#ifndef UIPSERVER_H
#define UIPSERVER_H

#include "uip_client.h"
#include "util.h"
#include <stddef.h>

class UIPServer
{
public:
    UIPServer(uint16_t port) : _port(htons(port)) { }
    UIPClient available();
    void begin();
    size_t write(uint8_t c) { return write(&c, 1); }
    size_t write(const uint8_t *buf, size_t size);
private:
    uint16_t _port;
};

#endif


