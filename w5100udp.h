#ifndef _W5100UDP_H_
#define _W5100UDP_H_
#include "udp.h"
#include "w5100ip.h"

class W5100UDP : public UDP
{
private:
    EthernetClass * const _eth;
    uint8_t _sock;  // socket ID for Wiz5100
    uint16_t _port; // local port to listen on
    uint32_t _remoteIP;
    uint16_t _remotePort; // remote port for the incoming packet whilst it's being processed
    uint16_t _offset; // offset into the packet being sent
    uint16_t _remaining; // remaining bytes of incoming packet yet to be processed
    int sendUDP(SOCKET s);
    int startUDP(SOCKET s, uint8_t *addr, uint16_t port);
    uint16_t bufferData(SOCKET s, uint16_t offset, const uint8_t *buf, uint16_t len);
public:
    W5100UDP(EthernetClass * const eth);
    uint8_t begin(uint16_t);	// initializeo sockets available to use
    void stop();  // Finish with the UDP socket
    int beginPacket(uint32_t ip, uint16_t port);
    int endPacket();
    size_t write(const uint8_t *buffer, size_t size);
    size_t write(uint8_t c) { return write(&c, 1); }
    int parsePacket();
    int available() { return _remaining; }
    int read();
    int read(uint8_t *buffer, size_t len);
    int read(char* buffer, size_t len) { return read((unsigned char*)buffer, len); };
    int peek();
    void flush() { while (_remaining) { read(); } }
    uint32_t remoteIP() { return _remoteIP; };
    uint16_t remotePort() { return _remotePort; };
};
#endif



