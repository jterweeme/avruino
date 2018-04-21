#include "eth.h"
#include "stream.h"

void Ethernet::addresses(ostream &os)
{
    uint32_t ip = localIP();
    uint32_t subnet = subnetMask();
    uint32_t gw = gatewayIP();
    uint32_t dns = dnsServerIP();
    os << "IP:      ";
    hex32(ip, os);
    os << "\r\nSubnet:  ";
    hex32(subnet, os);
    os << "\r\nGateway: ";
    hex32(gw, os);
    os << "\r\nDNS:     ";
    hex32(dns, os);
    os << "\r\n";
}



