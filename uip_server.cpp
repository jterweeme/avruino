#include "uip_server.h"

UIPClient UIPServer::available()
{
    _eth->tick();

    for (uip_userdata_t *data = &UIPEthernetClass::all_data[0];
        data < &UIPEthernetClass::all_data[UIP_CONNS];
        data++)
    {
        if (data->packets_in[0] != NOBLOCK && (((data->state & UIP_CLIENT_CONNECTED) &&
                uip_conns[data->state & UIP_CLIENT_SOCKETS].lport ==_port)
              || ((data->state & UIP_CLIENT_REMOTECLOSED) &&
            ((uip_userdata_closed_t *)data)->lport == _port)))
        {
            return UIPClient(_eth, data);
        }
    }

    return UIPClient(_eth);
}

void UIPServer::begin()
{
    uip_listen(_port);
    _eth->tick();
}

size_t UIPServer::write(const uint8_t *buf, size_t size)
{
    size_t ret = 0;

    for (uip_userdata_t* data = &UIPEthernetClass::all_data[0];
        data < &UIPEthernetClass::all_data[UIP_CONNS];
        data++ )
    {
        if ((data->state & UIP_CLIENT_CONNECTED) &&
            uip_conns[data->state & UIP_CLIENT_SOCKETS].lport ==_port)
        {
            ret += _eth->_write(data,buf,size);
        }
    }
    return ret;
}



