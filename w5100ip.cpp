#include "w5100ip.h"

// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
//uint8_t EthernetClass::_state[MAX_SOCK_NUM] = { 0, 0, 0, 0 };
//uint16_t EthernetClass::_server_port[MAX_SOCK_NUM] = { 0, 0, 0, 0 };

uint32_t EthernetClass::localIP() const
{
    uint32_t ip;
    _nw->getIPAddress((uint8_t *)&ip);
    return ip;
}

uint32_t EthernetClass::subnetMask() const
{
    uint32_t ip;
    _nw->getSubnetMask((uint8_t *)&ip);
    return ip;
}

uint32_t EthernetClass::gatewayIP() const
{
    uint32_t ip;
    _nw->getGatewayIp((uint8_t *)&ip);
    return ip;
}

int16_t EthernetClass::recv(SOCKET s, uint8_t *buf, int16_t len)
{
    // Check how much data is available
    int16_t ret = _nw->getRXReceivedSize(s);

    if (ret == 0)
    {
        // No data available.
        uint8_t status = _nw->readSnSR(s);

        if (status == SnSR::LISTEN || status == SnSR::CLOSED || status == SnSR::CLOSE_WAIT)
        {
            // The remote end has closed its side of the connection, so this is the eof state
            ret = 0;
        }
        else
        {
            ret = -1; // The connection is still up, but there's no data waiting to be read
        }
    }
    else if (ret > len)
    {
        ret = len;
    }

    if (ret > 0)
    {
        _nw->recv_data_processing(s, buf, ret);
        _nw->execCmdSn(s, Sock_RECV);
    }
    return ret;
}

static uint16_t local_port;

uint8_t EthernetClass::socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag)
{
    if ((protocol == SnMR::TCP) || (protocol == SnMR::UDP) ||
        (protocol == SnMR::IPRAW) || (protocol == SnMR::MACRAW) || (protocol == SnMR::PPPOE))
    {
        close(s);
        _nw->writeSnMR(s, protocol | flag);

        if (port != 0)
        {
            _nw->writeSnPORT(s, port);
        }
        else
        {
            local_port++; // if don't set the source port, set local_port number.
            _nw->writeSnPORT(s, local_port);
        }

        _nw->execCmdSn(s, Sock_OPEN);
        return 1;
    }

    return 0;
}

void EthernetClass::close(SOCKET s)
{
    _nw->execCmdSn(s, Sock_CLOSE);
    _nw->writeSnIR(s, 0xFF);
}

uint16_t EthernetClass::peek(SOCKET s, uint8_t *buf)
{
    _nw->recv_data_processing(s, buf, 1, 1);
    return 1;
}



