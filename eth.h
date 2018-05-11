#ifndef _ETH_H_
#define _ETH_H_
#include "types.h"

static constexpr uint16_t swap16(uint16_t n)
{
    return n << 8 | n >> 8;
}

static constexpr uint32_t swap32(uint32_t x)
{
    return (x<<24 & 0xff000000) | (x<<8 & 0xff0000) | (x>>8 & 0xff00) | (x>>24 & 0xff);
}

static constexpr uint16_t htons(uint16_t n)
{
    return swap16(n);
}

static constexpr uint32_t htonl(uint32_t x)
{
    return swap32(x);
}

static constexpr uint32_t ntohl(uint32_t x)
{
    return swap32(x);
}

class ostream;

class Ethernet
{
public:
    virtual uint32_t localIP() const { return 0; }
    virtual uint32_t subnetMask() const { return 0; }
    virtual uint32_t gatewayIP() const { return 0; }
    virtual uint32_t dnsServerIP() const { return 0; }
    void addresses(ostream &os);
};

#endif



