#ifndef _DNSCLIENT_H_
#define _DNSCLIENT_H_
#include "uip_udp.h"

class DNSClient
{
public:
    void begin(const IPAddrezz& aDNSServer);
    int inet_aton(const char *aIPAddrString, IPAddrezz& aResult);
    int getHostByName(const char* aHostname, IPAddrezz& aResult);
protected:
    uint16_t BuildRequest(const char* aName);
    uint16_t ProcessResponse(uint16_t aTimeout, IPAddrezz& aAddress);
    IPAddrezz iDNSServer;
    uint16_t iRequestId;
    UIPUDP iUdp;
};

#endif
