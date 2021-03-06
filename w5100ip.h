#ifndef _ETHERNET_H_
#define _ETHERNET_H_
#include "w5100hw.h"
#include "eth.h"

class EthernetClass : public Ethernet
{
private:
    W5100Class * const _nw;
public:
    W5100Class *nw() { return _nw; }
    EthernetClass(W5100Class *nw) : _nw(nw) { }
    uint8_t _state[MAX_SOCK_NUM] = {0,0,0,0};
    uint16_t _server_port[MAX_SOCK_NUM] = {0,0,0,0};
    uint32_t localIP() const;
    uint32_t subnetMask() const;
    uint32_t gatewayIP() const;
    int16_t recv(SOCKET s, uint8_t *buf, int16_t len);
    uint16_t peek(SOCKET s, uint8_t *buf);
    void close(SOCKET s);
    uint8_t socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag);
};
#endif



