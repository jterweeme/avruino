#ifndef IPAddress_h
#define IPAddress_h

class IPAddress
{
private:
	union {
		uint8_t a8[4];  // IPv4 address
		uint32_t a32;
	} _address;
	
    uint8_t* raw_address() { return _address.a8; }
public:
    IPAddress();
    IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
    IPAddress(uint32_t address);
    IPAddress(const uint8_t *address);
    operator uint32_t() { return _address.a32; }
    bool operator==(const IPAddress& addr) { return _address.a32 == addr._address.a32; };
    bool operator==(const uint8_t* addr);

    // Overloaded index operator to allow getting and setting individual octets of the address
    uint8_t operator[](int index) const { return _address.a8[index]; };
    uint8_t& operator[](int index) { return _address.a8[index]; };

    // Overloaded copy operators to allow initialisation of IPAddress objects from other types
    IPAddress& operator=(const uint8_t *address);
    IPAddress& operator=(uint32_t address);

    //virtual size_t printTo(Print& p) const;

    friend class EthernetClass;
    friend class UDP;
    friend class Client;
    friend class Server;
    friend class DhcpClass;
    friend class DNSClient;
};

const IPAddress INADDR_NONE(0,0,0,0);
#endif



