// Arduino DNS client for WizNet5100-based Ethernet shield
// (c) Copyright 2009-2010 MCQN Ltd.
// Released under Apache License, version 2.0

#ifndef _DNS2_H_
#define _DNS2_H_
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
    void begin(const uint32_t &aDNSServer);
    int inet_aton(const char *aIPAddrString, uint32_t &aResult);
    int getHostByName(const char* aHostname, uint32_t &aResult);
};
#endif



