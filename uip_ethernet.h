#ifndef UIPETHERNET_H
#define UIPETHERNET_H
#include "util.h"
#include "dhcp.h"
#include "network.h"
#include "uip_client.h"
#include "uip_server.h"
#include "uip_udp.h"

static const uint8_t UIPETHERNET_FREEPACKET = 1,
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
public:
    UIPEthernetClass();
    int begin(const uint8_t* mac);
    void begin(const uint8_t* mac, IPAddrezz ip);
    void begin(const uint8_t* mac, IPAddrezz ip, IPAddrezz dns);
    void begin(const uint8_t* mac, IPAddrezz ip, IPAddrezz dns, IPAddrezz gw);
    void begin(const uint8_t* mac, IPAddrezz ip, IPAddrezz dns, IPAddrezz gw, IPAddrezz subnet);
    int maintain();
    IPAddrezz localIP();
    IPAddrezz subnetMask();
    IPAddrezz gatewayIP();
    IPAddrezz dnsServerIP();
private:
    static memhandle in_packet;
    static memhandle uip_packet;
    static uint8_t uip_hdrlen;
    static uint8_t packetstate;
    static IPAddrezz _dnsServerAddress;
    static DhcpClass* _dhcp;
    static unsigned long periodic_timer;
    static void init(const uint8_t* mac);
    static void configure(IPAddrezz ip, IPAddrezz dns, IPAddrezz gateway, IPAddrezz subnet);
    static void tick();
    static bool network_send();
    friend class UIPServer;
    friend class UIPClient;
    friend class UIPUDP;
    static uint16_t chksum(uint16_t sum, const uint8_t* data, uint16_t len);
    static uint16_t ipchksum(void);
    static uint16_t upper_layer_chksum(uint8_t proto);
    friend uint16_t uip_ipchksum(void);
    friend uint16_t uip_tcpchksum(void);
    friend uint16_t uip_udpchksum(void);
    friend void uipclient_appcall(void);
    friend void uipudp_appcall(void);
};

extern UIPEthernetClass UIPEthernet;

#endif



