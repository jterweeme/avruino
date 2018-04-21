#ifndef _ETHERNETCLIENT_H_
#define _ETHERNETCLIENT_H_
#include "client.h"
#include "ethernet.h"

class EthernetClient : public Client
{
private:
    EthernetClass * const _eth;
    uint8_t _sock;
    static uint16_t _srcport;
public:
    EthernetClient(EthernetClass * const eth) : _eth(eth), _sock(MAX_SOCK_NUM) { }
    EthernetClient(EthernetClass * const eth, uint8_t sock) : _eth(eth), _sock(sock) { }
    uint8_t status();
    int connect(uint32_t ip, uint16_t port);
    int connect(const char *host, uint16_t port);
    size_t write(const uint8_t *buf, size_t size);
    int available();
    int read();
    int read(uint8_t *buf, size_t size);
    int peek();
    void flush();
    void stop();
    uint8_t connected();
    operator bool();
    friend class EthernetServer;
    using Print::write;
};
#endif


