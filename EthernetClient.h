#ifndef _ETHERNETCLIENT_H_
#define _ETHERNETCLIENT_H_
#include "client.h"
#include "ethernet.h"
#include "IPAddress.h"

class EthernetClient : public Client
{
private:
    EthernetClass * const _eth;
    uint8_t _sock;
public:
    EthernetClient(EthernetClass * const eth) : _eth(eth), _sock(MAX_SOCK_NUM) { }
    EthernetClient(EthernetClass * const eth, uint8_t sock) : _eth(eth), _sock(sock) { }
    uint8_t status();
    virtual int connect(uint32_t ip, uint16_t port);
    virtual int connect(const char *host, uint16_t port);
    virtual size_t write(const uint8_t *buf, size_t size);
    virtual size_t write(uint8_t b) { return write(&b, 1); }
    virtual int available();
    virtual int read();
    virtual int read(uint8_t *buf, size_t size);
    virtual int peek();
    virtual void flush();
    virtual void stop();
    virtual uint8_t connected();
    virtual operator bool();
    friend class EthernetServer;
    using Print::write;
private:
    static uint16_t _srcport;
};
#endif


