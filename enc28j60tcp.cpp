#include "enc28j60tcp.h"

static constexpr uint32_t htonl(uint32_t x)
{
    return (x<<24 & 0xff000000) | (x<<8 & 0xff0000) | (x>>8 & 0xff00) | (x>>24 & 0xff);
}

static constexpr uint32_t ntohl(uint32_t x) { return htonl(x); }

#define UIP_TCP_PHYH_LEN UIP_LLH_LEN+UIP_IPTCPH_LEN

extern uint32_t millis();

uip_userdata_t Enc28J60IP::all_data[UIP_CONNS];


#define uip_stopped(conn)   ((conn)->tcpstateflags & UIP_STOPPED)

UIPClient::UIPClient(Enc28J60IP * const eth) : _eth(eth), data(NULL) { }

UIPClient::UIPClient(Enc28J60IP * const eth, uip_userdata_t *conn_data) :
    _eth(eth), data(conn_data)
{
}

int UIPClient::connect(uint32_t ip, uint16_t port)
{
    stop();
    uint16_t ipaddr[2];
    ipaddr[0] = (uint16_t)(ip & 0xffff);
    ipaddr[1] = (uint16_t)(ip >> 16 & 0xffff);
    //uip_ip_addr(ipaddr, ip);
    struct uip_conn* conn = uip_connect(&ipaddr, htons(port));

    if (conn)
    {
#if 1
        int32_t timeout = millis() + 1000 * UIP_CONNECT_TIMEOUT;
#endif
        while((conn->tcpstateflags & UIP_TS_MASK) != UIP_CLOSED)
        {
            _eth->tick();

            if ((conn->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED)
            {
                data = (uip_userdata_t*) conn->appstate;
                return 1;
            }
#if 1
            if (((int32_t)(millis() - timeout)) > 0)
            {
                conn->tcpstateflags = UIP_CLOSED;
                break;
            }
#endif
        }
    }
    return 0;
}

void UIPClient::stop()
{
    if (data && data->state)
    {
        _eth->_flushBlocks(&data->packets_in[0]);

        if (data->state & UIP_CLIENT_REMOTECLOSED)
            data->state = 0;
        else
            data->state |= UIP_CLIENT_CLOSE;
    }

    data = NULL;
    _eth->tick();
}

uint8_t UIPClient::connected()
{
    return (data && (data->packets_in[0] != NOBLOCK ||
        (data->state & UIP_CLIENT_CONNECTED))) ? 1 : 0;
}

UIPClient::operator bool()
{
    _eth->tick();
    return data && (!(data->state & UIP_CLIENT_REMOTECLOSED) || data->packets_in[0] != NOBLOCK);
}

int UIPClient::available()
{
    if (*this)
        return _available(data);

    return 0;
}

int UIPClient::_available(uip_userdata_t *u)
{
    int len = 0;

    for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++)
        len += _eth->nw()->blockSize(u->packets_in[i]);
    
    return len;
}

int UIPClient::read(uint8_t *buf, size_t size)
{
    if (*this)
    {
        uint16_t remain = size;

        if (data->packets_in[0] == NOBLOCK)
            return 0;

        uint16_t read;

        do
        {
            read = _eth->nw()->readPacket(data->packets_in[0], 0,buf+size-remain,remain);

            if (read == _eth->nw()->blockSize(data->packets_in[0]))
            {
                remain -= read;
                _eth->_eatBlock(&data->packets_in[0]);

                if (uip_stopped(&uip_conns[data->state & UIP_CLIENT_SOCKETS]) &&
                    !(data->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
                {
                    data->state |= UIP_CLIENT_RESTART;
                }

                if (data->packets_in[0] == NOBLOCK)
                {
                    if (data->state & UIP_CLIENT_REMOTECLOSED)
                    {
                        data->state = 0;
                        data = NULL;
                    }
                    return size-remain;
                }
            }
            else
            {
                _eth->nw()->resizeBlock(data->packets_in[0],read);
                break;
            }
        }
      while(remain > 0);
      return size;
    }
  return -1;
}

int UIPClient::read()
{
    uint8_t c;

    if (read(&c, 1) < 0)
        return -1;

    return c;
}

int UIPClient::peek()
{
    if (*this)
    {
        if (data->packets_in[0] != NOBLOCK)
        {
            uint8_t c;
            _eth->nw()->readPacket(data->packets_in[0],0,&c,1);
            return c;
        }
    }
    return -1;
}

void UIPClient::flush()
{
    if (*this)
        _eth->_flushBlocks(&data->packets_in[0]);
}

UIPClient UIPServer::available()
{
    _eth->tick();

    for (uip_userdata_t *data = &Enc28J60IP::all_data[0];
        data < &Enc28J60IP::all_data[UIP_CONNS]; data++)
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

    for (uip_userdata_t* data = &Enc28J60IP::all_data[0]; data < &Enc28J60IP::all_data[UIP_CONNS];
        data++)
    {
        if ((data->state & UIP_CLIENT_CONNECTED) &&
            uip_conns[data->state & UIP_CLIENT_SOCKETS].lport ==_port)
        {
            ret += _eth->_write(data,buf,size);
        }
    }
    return ret;
}





