#include "uip_client.h"

static constexpr uint32_t htonl(uint32_t x)
{
    return (x<<24 & 0xff000000) | (x<<8 & 0xff0000) | (x>>8 & 0xff00) | (x>>24 & 0xff);
}

static constexpr uint32_t ntohl(uint32_t x) { return htonl(x); }

class IPAddrezz
{
private:
    union {
        uint8_t a8[4];  // IPv4 address
        uint32_t a32;
    } _address;
public:
    uint8_t* raw_address() { return _address.a8; }
    IPAddrezz() { memset(_address.a8, 0, sizeof(_address)); }
    IPAddrezz(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
    IPAddrezz(uint32_t address) { _address.a32 = address; }
    IPAddrezz(const uint8_t *address) { memcpy(_address.a8, address, sizeof(_address)); }
    uint32_t get32() const { return _address.a32; }
    operator uint32_t() { return _address.a32; }
    bool operator==(const IPAddrezz& addr) { return _address.a32 == addr._address.a32; };
    bool operator==(const uint8_t* addr);
    uint8_t operator[](int index) const { return _address.a8[index]; };
    uint8_t& operator[](int index) { return _address.a8[index]; };
    IPAddrezz& operator=(const uint8_t *address);
    IPAddrezz& operator=(uint32_t address);
};

#define UIP_TCP_PHYH_LEN UIP_LLH_LEN+UIP_IPTCPH_LEN

extern uint32_t millis();

uip_userdata_t UIPEthernetClass::all_data[UIP_CONNS];


IPAddrezz::IPAddrezz(uint8_t oct1, uint8_t oct2, uint8_t third_octet, uint8_t fourth_octet)
{
    _address.a8[0] = oct1;
    _address.a8[1] = oct2;
    _address.a8[2] = third_octet;
    _address.a8[3] = fourth_octet;
}

#define uip_stopped(conn)   ((conn)->tcpstateflags & UIP_STOPPED)

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

UIPClient::UIPClient(UIPEthernetClass * const eth) : _eth(eth), data(NULL) { }

UIPClient::UIPClient(UIPEthernetClass * const eth, uip_userdata_t *conn_data) :
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

#if 0
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
#endif

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






