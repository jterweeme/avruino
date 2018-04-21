#ifndef _ETHERNETSERVER_H_
#define _ETHERNETSERVER_H_
#include "types.h"

class EthernetClass;
class EthernetClient;

class EthernetServer //: public Server
{
private:
    EthernetClass * const _eth;
    uint16_t _port;
    void accept();
public:
    EthernetServer(EthernetClass * const eth, uint16_t port);
    EthernetClient available();
    //Client available2();
    virtual void begin();
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buf, size_t size);
};
#endif



