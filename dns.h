#ifndef _DNS_H_
#define _DNS_H_
#include "types.h"

class UDP;

class DNSClient
{
protected:
    uint16_t BuildRequest(const char* aName);
    uint16_t ProcessResponse(uint16_t aTimeout, uint32_t &aAddress);
    uint32_t iDNSServer;
    uint16_t iRequestId = 0;
    UDP *iUdp;
public:
    DNSClient(UDP *udp);
    void begin(uint32_t ip);
    int inet_aton(const char *aIPAddrString, uint32_t &aResult);
    int getHostByName(const char* aHostname, uint32_t &aResult);
};
#endif



