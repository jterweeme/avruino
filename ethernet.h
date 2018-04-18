#ifndef ethernet_h
#define ethernet_h

#include "types.h"
#include "IPAddress.h"
#include "dhcp2.h"
#include "w5100.h"

#define MAX_SOCK_NUM 4

class EthernetClass
{
private:
    W5100Class * const _nw;
    IPAddress _dnsServerAddress;
    DhcpClass s_dhcp;
    DhcpClass *_dhcp;
public:
    W5100Class *nw() { return _nw; }
    EthernetClass(W5100Class *nw) : _nw(nw), s_dhcp(this) { }
    static uint8_t _state[MAX_SOCK_NUM];
    static uint16_t _server_port[MAX_SOCK_NUM];
    int begin(uint8_t *mac_address);
    void begin(uint8_t *mac_address, IPAddress local_ip);
    void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server);
    void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
    void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);
    int maintain();
    IPAddress localIP();
    IPAddress subnetMask();
    IPAddress gatewayIP();
    IPAddress dnsServerIP();
    friend class EthernetClient;
    friend class EthernetServer;
};
#endif



