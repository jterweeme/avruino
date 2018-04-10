#ifndef IPAddress_h
#define IPAddress_h
#include "types.h"

class IPAddress
{
public:
	union {
		uint8_t a8[4];  // IPv4 address
		uint32_t a32;
	} _address;

    uint8_t* raw_address() { return _address.a8; }
    IPAddress();
    IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
    IPAddress(uint32_t address);
    IPAddress(const uint8_t *address);
    operator uint32_t() { return _address.a32; }
    bool operator==(const IPAddress& addr) { return _address.a32 == addr._address.a32; };
    bool operator==(const uint8_t* addr);
    uint8_t operator[](int index) const { return _address.a8[index]; };
    uint8_t& operator[](int index) { return _address.a8[index]; };
    IPAddress& operator=(const uint8_t *address);
    IPAddress& operator=(uint32_t address);
};
#endif



