#ifndef _UIPETHERNET_H_
#define _UIPETHERNET_H_
#include "ipaddrezz.h"
#include "dhcp.h"
#include "enc28j60.h"

static constexpr uint8_t
    UIPETHERNET_FREEPACKET = 1,
    UIPETHERNET_SENDPACKET = 2,
    UIPETHERNET_BUFFERREAD = 4;

#define uip_ip_addr(addr, ip) do { \
                     ((uint16_t *)(addr))[0] = HTONS(((ip[0]) << 8) | (ip[1])); \
                     ((uint16_t *)(addr))[1] = HTONS(((ip[2]) << 8) | (ip[3])); \
                  } while(0)

#define ip_addr_uip(a) IPAddrezz(a[0] & 0xFF, a[0] >> 8 , a[1] & 0xFF, a[1] >> 8)

#define uip_seteth_addr(eaddr) do {uip_ethaddr.addr[0] = eaddr[0]; \
                              uip_ethaddr.addr[1] = eaddr[1];\
                              uip_ethaddr.addr[2] = eaddr[2];\
                              uip_ethaddr.addr[3] = eaddr[3];\
                              uip_ethaddr.addr[4] = eaddr[4];\
                              uip_ethaddr.addr[5] = eaddr[5];} while(0)

#define BUF ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])

class UIPEthernetClass
{
private:
    Enc28J60Network _nw;
    static DhcpClass *_dhcp;
    static unsigned long periodic_timer;
    void init(const uint8_t* mac);
    void configure(IPAddrezz ip, IPAddrezz dns, IPAddrezz gateway, IPAddrezz subnet);
    uint16_t chksum(uint16_t sum, const uint8_t* data, uint16_t len);
public:
    uint16_t ipchksum();
    uint16_t upper_layer_chksum(uint8_t proto);
    static memhandle in_packet;
    static uint8_t uip_hdrlen;
    bool network_send();
    static memhandle uip_packet;
    static uint8_t packetstate;
    static IPAddrezz _dnsServerAddress;
    void tick();
    static UIPEthernetClass *instance;
    UIPEthernetClass() { instance = this; }
    int begin(const uint8_t *mac, DhcpClass *dhcp);
    void begin(const uint8_t *mac, uint32_t ip);
    void begin(const uint8_t *mac, uint32_t ip, IPAddrezz dns);
    void begin(const uint8_t *mac, uint32_t ip, IPAddrezz dns, IPAddrezz gw);
    void begin(const uint8_t *mac, uint32_t ip, IPAddrezz dns, IPAddrezz gw, IPAddrezz subnet);
    int maintain();
    IPAddrezz localIP();
    IPAddrezz subnetMask();
    IPAddrezz gatewayIP();
    IPAddrezz dnsServerIP();
    void tick2();
};
#endif



