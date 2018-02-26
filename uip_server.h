#ifndef UIPSERVER_H
#define UIPSERVER_H

#include "uip_client.h"
#include "uip_ethernet.h"
#include <stddef.h>

class UIPServer
{
private:
    UIPEthernetClass * const _eth;
    const uint16_t _port;
public:
    UIPServer(UIPEthernetClass * const eth, uint16_t port) : _eth(eth), _port(htons(port)) { }
    UIPClient available();
    void begin();
    size_t write(uint8_t c) { return write(&c, 1); }
    size_t write(const uint8_t *buf, size_t size);

};

#endif


