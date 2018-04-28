#ifndef _W5100UDP_H_
#define _W5100UDP_H_
#include "udp.h"

class EthernetClass;

#define UDP_TX_PACKET_MAX_SIZE 24

class EthernetUDP : public UDP
{
private:
    EthernetClass * const _eth;
    uint8_t _sock;  // socket ID for Wiz5100
    uint16_t _port; // local port to listen on
    uint32_t _remoteIP;
    uint16_t _remotePort; // remote port for the incoming packet whilst it's being processed
    uint16_t _offset; // offset into the packet being sent
    uint16_t _remaining; // remaining bytes of incoming packet yet to be processed
public:
    EthernetUDP(EthernetClass * const eth);
    virtual uint8_t begin(uint16_t);	// initializeo sockets available to use
    virtual void stop();  // Finish with the UDP socket
    virtual int beginPacket(uint32_t ip, uint16_t port);
    virtual int endPacket();
    virtual size_t write(const uint8_t *buffer, size_t size);
    virtual size_t write(uint8_t c) { return write(&c, 1); }
    virtual int parsePacket();
    virtual int available() { return _remaining; }
    virtual int read();
    virtual int read(uint8_t *buffer, size_t len);
    virtual int read(char* buffer, size_t len) { return read((unsigned char*)buffer, len); };
    virtual int peek();
    virtual void flush() { while (_remaining) { read(); } }
    virtual uint32_t remoteIP() { return _remoteIP; };
    virtual uint16_t remotePort() { return _remotePort; };
};
#endif



