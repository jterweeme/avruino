#ifndef _UIPUDP_H_
#define _UIPUDP_H_
#include "enc28j60ip.h"
#include "udp.h"

class UIPUDP : public UDP
{
private:
    struct uip_udp_conn_t *_uip_udp_conn = 0;
    uip_udp_userdata_t appdata;
    Enc28J60IP * const _eth;
public:
    UIPUDP(Enc28J60IP *eth);
    uint8_t begin(uint16_t);
    void stop();
    int beginPacket(uint32_t ip, uint16_t port);
    int beginPacket(const char *host, uint16_t port);
    int endPacket();
    size_t write(const uint8_t *buffer, size_t size);
    size_t write(uint8_t c) { return write(&c, 1); }
    int parsePacket();
    int available();
    int read();
    int read(uint8_t *buffer, size_t len);
    int read(char* buffer, size_t len) { return read((uint8_t *) buffer, len); }
    int peek();
    void flush();	// Finish reading the current packet
    uint32_t remoteIP();
    uint16_t remotePort() { return _uip_udp_conn ? htons(_uip_udp_conn->rport) : 0; }
};
#endif



