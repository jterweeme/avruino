#include "w5100.h"
#include "ethernet.h"

// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
uint8_t EthernetClass::_state[MAX_SOCK_NUM] = { 0, 0, 0, 0 };
uint16_t EthernetClass::_server_port[MAX_SOCK_NUM] = { 0, 0, 0, 0 };

uint32_t EthernetClass::localIP()
{
    uint32_t ip;
    _nw->getIPAddress((uint8_t *)&ip);
    return ip;
}

uint32_t EthernetClass::subnetMask()
{
    uint32_t ip;
    _nw->getSubnetMask((uint8_t *)&ip);
    return ip;
}

uint32_t EthernetClass::gatewayIP()
{
    uint32_t ip;
    _nw->getGatewayIp((uint8_t *)&ip);
    return ip;
}



