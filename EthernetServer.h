#ifndef _ETHERNETSERVER_H_
#define _ETHERNETSERVER_H_
#include "EthernetClient.h"

class EthernetServer
{
private:
    EthernetClass * const _eth;
    uint16_t _port;
    void accept();
public:
    EthernetServer(EthernetClass * const eth, uint16_t port) : _eth(eth), _port(port) { }
    EthernetClient available();
    virtual void begin();
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buf, size_t size);
};
#endif



