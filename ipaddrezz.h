#ifndef _IPADDREZZ_H_
#define _IPADDREZZ_H_
#include <string.h>
#include "types.h"

static constexpr uint32_t htonl(uint32_t x)
{
    return (x<<24 & 0xff000000) | (x<<8 & 0xff0000) | (x>>8 & 0xff00) | (x>>24 & 0xff);
}

static constexpr uint32_t ntohl(uint32_t x) { return htonl(x); }

class IPAddrezz
{
private:
    union {
        uint8_t a8[4];  // IPv4 address
        uint32_t a32;
    } _address;
public:
    uint8_t* raw_address() { return _address.a8; }
    IPAddrezz() { memset(_address.a8, 0, sizeof(_address)); }
    IPAddrezz(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
    IPAddrezz(uint32_t address) { _address.a32 = address; }
    IPAddrezz(const uint8_t *address) { memcpy(_address.a8, address, sizeof(_address)); }
    uint32_t get32() const { return _address.a32; }
    operator uint32_t() { return _address.a32; }
    bool operator==(const IPAddrezz& addr) { return _address.a32 == addr._address.a32; };
    bool operator==(const uint8_t* addr);
    uint8_t operator[](int index) const { return _address.a8[index]; };
    uint8_t& operator[](int index) { return _address.a8[index]; };
    IPAddrezz& operator=(const uint8_t *address);
    IPAddrezz& operator=(uint32_t address);
};

#endif



