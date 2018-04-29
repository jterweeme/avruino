#ifndef _UIPUDP_H_
#define _UIPUDP_H_
#include "uip.h"
#include "udp.h"

static const uint16_t UIP_UDP_MAXDATALEN = 1500;
#define UIP_UDP_PHYH_LEN UIP_LLH_LEN+UIP_IPUDPH_LEN
#define UIP_UDP_MAXPACKETSIZE UIP_UDP_MAXDATALEN+UIP_UDP_PHYH_LEN

class UIPUDP : public UDP
{
private:
    struct uip_udp_conn *_uip_udp_conn = 0;
    uip_udp_userdata_t appdata;
    UIPEthernetClass * const _eth;
public:
    UIPUDP(UIPEthernetClass *eth);
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
    uint16_t remotePort() { return _uip_udp_conn ? ntohs(_uip_udp_conn->rport) : 0; }
};
#endif



