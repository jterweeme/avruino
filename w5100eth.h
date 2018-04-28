#ifndef _ETHERNET_H_
#define _ETHERNET_H_
#include "w5100.h"
#include "eth.h"

class EthernetClass : public Ethernet
{
private:
    W5100Class * const _nw;
public:
    W5100Class *nw() { return _nw; }
    EthernetClass(W5100Class *nw) : _nw(nw) { }
    static uint8_t _state[MAX_SOCK_NUM];
    static uint16_t _server_port[MAX_SOCK_NUM];
    uint32_t localIP();
    uint32_t subnetMask();
    uint32_t gatewayIP();
    int16_t recv(SOCKET s, uint8_t *buf, int16_t len);
    uint16_t peek(SOCKET s, uint8_t *buf);
    void close(SOCKET s);
    uint8_t socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag);
};
#endif



