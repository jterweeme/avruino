#ifndef ethernet_h
#define ethernet_h

#include "IPAddress.h"
#include "dhcp2.h"
#include "w5100.h"
#include "eth.h"

#define MAX_SOCK_NUM 4

class EthernetClass : public Ethernet
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
    uint32_t localIP();
    uint32_t subnetMask();
    uint32_t gatewayIP();
    uint32_t dnsServerIP() { return _dnsServerAddress.get32(); }
    friend class EthernetClient;
    friend class EthernetServer;
};
#endif



