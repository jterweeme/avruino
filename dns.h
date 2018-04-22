#ifndef _DNSCLIENT_H_
#define _DNSCLIENT_H_
#include "uip_udp.h"

class DNSClient
{
protected:
    uint16_t BuildRequest(const char* aName);
    uint16_t ProcessResponse(uint16_t aTimeout, uint32_t &aAddress);
    IPAddrezz iDNSServer;
    uint16_t iRequestId;
    UIPUDP iUdp;
    UIPEthernetClass * const _eth;
public:
    DNSClient(UIPEthernetClass *eth) : iUdp(eth), _eth(eth) { }
    void begin(uint32_t ip);
    int inet_aton(const char *aIPAddrString, uint32_t &aResult);
    int getHostByName(const char* aHostname, uint32_t &aResult);
};
#endif



