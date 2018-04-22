// Arduino DNS client for WizNet5100-based Ethernet shield
// (c) Copyright 2009-2010 MCQN Ltd.
// Released under Apache License, version 2.0

#ifndef _DNS2_H_
#define _DNS2_H_
#include "w5100udp.h"

class DNSClient
{
protected:
    uint16_t BuildRequest(const char* aName);
    uint16_t ProcessResponse(uint16_t aTimeout, uint32_t &aAddress);
    uint32_t iDNSServer;
    uint16_t iRequestId = 0;
    EthernetUDP *iUdp;
public:
    //DNSClient(EthernetClass *eth);
    DNSClient(EthernetUDP *udp);
    void begin(const uint32_t &aDNSServer);

    /** Convert a numeric IP address string into a four-byte IP address.
        @param aIPAddrString IP address to convert
        @param aResult IPAddress structure to store the returned IP address
        @result 1 if aIPAddrString was successfully converted to an IP address,
                else error code
    */
    int inet_aton(const char *aIPAddrString, uint32_t &aResult);

    /** Resolve the given hostname to an IP address.
        @param aHostname Name to be resolved
        @param aResult IPAddress structure to store the returned IP address
        @result 1 if aIPAddrString was successfully converted to an IP address,
                else error code
    */
    int getHostByName(const char* aHostname, uint32_t &aResult);
};
#endif



