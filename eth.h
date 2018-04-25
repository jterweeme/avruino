#ifndef _ETH_H_
#define _ETH_H_
#include "types.h"

class ostream;

class Ethernet
{
public:
    //virtual void tick() { }
    //virtual size_t _write(uip_userdata_t *u, const uint8_t *buf, size_t size) { return 0; }
    virtual uint32_t localIP() { return 0; }
    virtual uint32_t subnetMask() { return 0; }
    virtual uint32_t gatewayIP() { return 0; }
    virtual uint32_t dnsServerIP() { return 0; }
    void addresses(ostream &os);
};

#endif



