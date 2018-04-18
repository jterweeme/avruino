#ifndef _ETH_H_
#define _ETH_H_
#include "mempool.h"
#include "types.h"
#include "stream.h"

struct uip_userdata_t
{
    uint8_t state;
    memhandle packets_in[UIP_SOCKET_NUMPACKETS];
    memhandle packets_out[UIP_SOCKET_NUMPACKETS];
    memaddress out_pos;
    unsigned long timer;
};

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



