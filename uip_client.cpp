#include "uip_client.h"
#include "dns.h"

#define UIP_TCP_PHYH_LEN UIP_LLH_LEN+UIP_IPTCPH_LEN

extern uint32_t millis();

IPAddrezz::IPAddrezz(uint8_t oct1, uint8_t oct2, uint8_t third_octet, uint8_t fourth_octet)
{
    _address.a8[0] = oct1;
    _address.a8[1] = oct2;
    _address.a8[2] = third_octet;
    _address.a8[3] = fourth_octet;
}


#define uip_restart()         do { uip_flags |= UIP_NEWDATA; \
                                   uip_conn->tcpstateflags &= ~UIP_STOPPED; \
                              } while(0)

#define uip_stop()          (uip_conn->tcpstateflags |= UIP_STOPPED)
#define uip_stopped(conn)   ((conn)->tcpstateflags & UIP_STOPPED)
#define uip_newdata()   (uip_flags & UIP_NEWDATA)

IPAddrezz& IPAddrezz::operator=(const uint8_t *address)
{
    memcpy(_address.a8, address, sizeof(_address));
    return *this;
}

IPAddrezz& IPAddrezz::operator=(uint32_t address)
{
    _address.a32=address;
    return *this;
}

bool IPAddrezz::operator==(const uint8_t* addr)
{
    return memcmp(addr, _address.a8, sizeof(_address)) == 0;
}

uip_userdata_t UIPClient::all_data[UIP_CONNS];

UIPClient::UIPClient(UIPEthernetClass * const eth) : _eth(eth), data(NULL) { }

UIPClient::UIPClient(UIPEthernetClass * const eth, uip_userdata_t *conn_data)
    : _eth(eth), data(conn_data)
{
}

int UIPClient::connect(IPAddrezz ip, uint16_t port)
{
    stop();
    uip_ipaddr_t ipaddr;
    uip_ip_addr(ipaddr, ip);
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

int UIPClient::connect(const char *host, uint16_t port)
{
    int ret = 0;
    DNSClient dns(_eth);
    IPAddrezz remote_addr;
    dns.begin(_eth->_dnsServerAddress);
    ret = dns.getHostByName(host, remote_addr);

    if (ret == 1)
        return connect(remote_addr, port);

    return ret;
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

size_t UIPClient::_write(uip_userdata_t* u, const uint8_t *buf, size_t size)
{
    int remain = size;
    uint16_t written;
#if UIP_ATTEMPTS_ON_WRITE > 0
    uint16_t attempts = UIP_ATTEMPTS_ON_WRITE;
#endif
repeat:
    UIPEthernetClass::instance->tick();

    if (u && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
    {
      uint8_t p = _currentBlock(&u->packets_out[0]);
      if (u->packets_out[p] == NOBLOCK)
        {
newpacket:
          u->packets_out[p] = Enc28J60Network::allocBlock(UIP_SOCKET_DATALEN);
          if (u->packets_out[p] == NOBLOCK)
            {
#if UIP_ATTEMPTS_ON_WRITE > 0
              if ((--attempts)>0)
#endif
#if UIP_ATTEMPTS_ON_WRITE != 0
                goto repeat;
#endif
              goto ready;
            }
          u->out_pos = 0;
        }
      written = Enc28J60Network::instance->writePacket(u->packets_out[p],
            u->out_pos,(uint8_t*)buf+size-remain,remain);

      remain -= written;
      u->out_pos+=written;
      if (remain > 0)
        {
          if (p == UIP_SOCKET_NUMPACKETS-1)
            {
#if UIP_ATTEMPTS_ON_WRITE > 0
              if ((--attempts)>0)
#endif
#if UIP_ATTEMPTS_ON_WRITE != 0
                goto repeat;
#endif
              goto ready;
            }
          p++;
          goto newpacket;
        }
ready:
#if  1
      u->timer = millis()+UIP_CLIENT_TIMER;
#endif
      return size-remain;
    }
  return -1;
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
                _eatBlock(&data->packets_in[0]);

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


static void uipclient_appcall2()
{
    uint16_t send_len = 0;
    uip_userdata_t *u = (uip_userdata_t*)uip_conn->appstate;

    if (!u && uip_connected())
    {
        u = (uip_userdata_t*) UIPClient::_allocateData();

        if (u)
            uip_conn->appstate = u;
    }

    if (u)
    {
        if (uip_newdata())
        {
            if (uip_len && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
            {
                for (uint8_t i=0; i < UIP_SOCKET_NUMPACKETS; i++)
                {
                    if (u->packets_in[i] == NOBLOCK)
                    {
                        u->packets_in[i] = Enc28J60Network::allocBlock(uip_len);

                        if (u->packets_in[i] != NOBLOCK)
                        {
                          Enc28J60Network::instance->copyPacket(u->packets_in[i],
                            0, UIPEthernetClass::instance->in_packet,
                            ((uint8_t*)uip_appdata)-uip_buf,uip_len);

                          if (i == UIP_SOCKET_NUMPACKETS-1)
                                uip_stop();

                          goto finish_newdata;
                        }
                    }
                }
              UIPEthernetClass::instance->packetstate &= ~UIPETHERNET_FREEPACKET;
              uip_stop();
            }
        }
finish_newdata:
      if (u->state & UIP_CLIENT_RESTART)
        {
          u->state &= ~UIP_CLIENT_RESTART;
          uip_restart();
        }

      // If the connection has been closed, save received but unread data.
      if ((uip_flags & UIP_CLOSE) || uip_timedout())
        {
          // drop outgoing packets not sent yet:
          UIPEthernetClass::instance->_flushBlocks(&u->packets_out[0]);
          if (u->packets_in[0] != NOBLOCK)
            {
              ((uip_userdata_closed_t *)u)->lport = uip_conn->lport;
              u->state |= UIP_CLIENT_REMOTECLOSED;
            }
          else
            u->state = 0;
          uip_conn->appstate = NULL;
          goto finish;
        }
      if (uip_flags & UIP_ACKDATA)
        {
          UIPClient::_eatBlock(&u->packets_out[0]);
        }
      if ((uip_flags & UIP_POLL) || uip_rexmit())
        {
          if (u->packets_out[0] != NOBLOCK)
            {
              if (u->packets_out[1] == NOBLOCK)
                {
                  send_len = u->out_pos;
                  if (send_len > 0)
                    {
                      Enc28J60Network::resizeBlock(u->packets_out[0],0,send_len);
                    }
                }
              else
            {
                send_len = Enc28J60Network::instance->blockSize(u->packets_out[0]);
            }
              if (send_len > 0)
                {
                  UIPEthernetClass::instance->uip_hdrlen = ((uint8_t*)uip_appdata)-uip_buf;
                  UIPEthernetClass::instance->uip_packet = Enc28J60Network::allocBlock(
                    UIPEthernetClass::instance->uip_hdrlen+send_len);
                  if (UIPEthernetClass::instance->uip_packet != NOBLOCK)
                    {
                      Enc28J60Network::instance->copyPacket(
                            UIPEthernetClass::instance->uip_packet,
                            UIPEthernetClass::instance->uip_hdrlen,u->packets_out[0],0,send_len);

                      UIPEthernetClass::instance->packetstate |= UIPETHERNET_SENDPACKET;
                    }
                }
              goto finish;
            }
        }
      // don't close connection unless all outgoing packets are sent
      if (u->state & UIP_CLIENT_CLOSE)
        {
          if (u->packets_out[0] == NOBLOCK)
            {
                u->state = 0;
                uip_conn->appstate = NULL;
                uip_flags = UIP_CLOSE;
            }
          else
            {
              uip_stop();
            }
        }
    }
finish:
    uip_send(uip_appdata,send_len);
    uip_len = send_len;
}

void uipclient_appcall()
{
    uipclient_appcall2();
}

uip_userdata_t *UIPClient::_allocateData()
{
    for (uint8_t sock = 0; sock < UIP_CONNS; sock++)
    {
        uip_userdata_t* data = &UIPClient::all_data[sock];

        if (!data->state)
        {
            data->state = sock | UIP_CLIENT_CONNECTED;
            memset(&data->packets_in[0], 0, sizeof(uip_userdata_t) - sizeof(data->state));
            return data;
        }
    }
    return NULL;
}

uint8_t UIPClient::_currentBlock(memhandle* block)
{
    for (uint8_t i = 1; i < UIP_SOCKET_NUMPACKETS; i++)
        if (block[i] == NOBLOCK)
            return i - 1;

    return UIP_SOCKET_NUMPACKETS-1;
}

void UIPClient::_eatBlock(memhandle* block)
{
    Enc28J60Network::freeBlock(block[0]);

    for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS-1; i++)
        block[i] = block[i+1];
    
    block[UIP_SOCKET_NUMPACKETS-1] = NOBLOCK;
}










