#include "w5100.h"
#include "ethernet.h"
#include "dhcp2.h"

// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
uint8_t EthernetClass::_state[MAX_SOCK_NUM] = { 0, 0, 0, 0 };
uint16_t EthernetClass::_server_port[MAX_SOCK_NUM] = { 0, 0, 0, 0 };

int EthernetClass::begin(uint8_t *mac_address)
{
    _dhcp = &s_dhcp;
    _nw->init();
    _nw->setMACAddress(mac_address);
    _nw->setIPAddress(IPAddress(0,0,0,0).raw_address());
    int ret = _dhcp->beginWithDHCP(mac_address);

    if(ret == 1)
    {
        // We've successfully found a DHCP server and got our configuration info, so set things
        // accordingly
        _nw->setIPAddress(_dhcp->getLocalIp().raw_address());
        _nw->setGatewayIp(_dhcp->getGatewayIp().raw_address());
        _nw->setSubnetMask(_dhcp->getSubnetMask().raw_address());
        _dnsServerAddress = _dhcp->getDnsServerIp();
    }

    return ret;
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip)
{
    // Assume the DNS server will be the machine on the same network as the local IP
    // but with last octet being '1'
    IPAddress dns_server = local_ip;
    dns_server[3] = 1;
    begin(mac_address, local_ip, dns_server);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server)
{
    // Assume the gateway will be the machine on the same network as the local IP
    // but with last octet being '1'
    IPAddress gateway = local_ip;
    gateway[3] = 1;
    begin(mac_address, local_ip, dns_server, gateway);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
{
    IPAddress subnet(255, 255, 255, 0);
    begin(mac_address, local_ip, dns_server, gateway, subnet);
}

void EthernetClass::begin(uint8_t *mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
    _nw->init();
    _nw->setMACAddress(mac);
    _nw->setIPAddress(local_ip._address.a8);
    _nw->setGatewayIp(gateway._address.a8);
    _nw->setSubnetMask(subnet._address.a8);
    _dnsServerAddress = dns_server;
}

int EthernetClass::maintain()
{
    int rc = DHCP_CHECK_NONE;
    if(_dhcp != NULL){
        //we have a pointer to dhcp, use it
        rc = _dhcp->checkLease();
        switch ( rc ){
        case DHCP_CHECK_NONE:
            //nothing done
            break;
        case DHCP_CHECK_RENEW_OK:
        case DHCP_CHECK_REBIND_OK:
            //we might have got a new IP.
            _nw->setIPAddress(_dhcp->getLocalIp().raw_address());
            _nw->setGatewayIp(_dhcp->getGatewayIp().raw_address());
            _nw->setSubnetMask(_dhcp->getSubnetMask().raw_address());
            _dnsServerAddress = _dhcp->getDnsServerIp();
            break;
        default:
            //this is actually a error, it will retry though
            break;
        }
    }
    return rc;
}

uint32_t EthernetClass::localIP()
{
    IPAddress ret;
    _nw->getIPAddress(ret.raw_address());
    return ret.get32();
}

uint32_t EthernetClass::subnetMask()
{
    IPAddress ret;
    _nw->getSubnetMask(ret.raw_address());
    return ret.get32();
}

uint32_t EthernetClass::gatewayIP()
{
    IPAddress ret;
    _nw->getGatewayIp(ret.raw_address());
    return ret.get32();
}



