#include "eth.h"
#include "stream.h"

void Ethernet::addresses(ostream &os)
{
    uint32_t ip = localIP();
    uint32_t subnet = subnetMask();
    uint32_t gw = gatewayIP();
    os << "IP:      ";
    hex32(ip, os);
    os << "\r\nSubnet:  ";
    hex32(subnet, os);
    os << "\r\nGateway: ";
    hex32(gw, os);
    os << "\r\n";
}



