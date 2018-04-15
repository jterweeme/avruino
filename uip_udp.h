#ifndef _UIPUDP_H_
#define _UIPUDP_H_

//#include "uip_ethernet.h"
#include "uip.h"
#include "enc28j60.h"
#include "ipaddrezz.h"

static const uint16_t UIP_UDP_MAXDATALEN = 1500;
#define UIP_UDP_PHYH_LEN UIP_LLH_LEN+UIP_IPUDPH_LEN
#define UIP_UDP_MAXPACKETSIZE UIP_UDP_MAXDATALEN+UIP_UDP_PHYH_LEN

typedef struct
{
    memaddress out_pos;
    memhandle packet_next;
    memhandle packet_in;
    memhandle packet_out;
    bool send;
}
uip_udp_userdata_t;

class UIPEthernetClass;

class UIPUDP
{
private:
    struct uip_udp_conn *_uip_udp_conn;
    uip_udp_userdata_t appdata;
    UIPEthernetClass *_eth;
public:
    UIPUDP() : _uip_udp_conn(NULL) { memset(&appdata, 0, sizeof(appdata)); }

    UIPUDP(UIPEthernetClass *eth) : _uip_udp_conn(NULL), _eth(eth)
    { memset(&appdata, 0, sizeof(appdata)); }

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
    IPAddrezz remoteIP();
    uint16_t remotePort() { return _uip_udp_conn ? ntohs(_uip_udp_conn->rport) : 0; }
private:
    friend void uipudp_appcall(void);
    friend class UIPEthernetClass;
    static void _send(uip_udp_userdata_t *data);
};

#endif
