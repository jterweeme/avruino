#include "uip_ethernet.h"
#include "uip_udp.h"
#include "dns.h"
#include "uip.h"
#include "arp.h"

extern UIPEthernetClass UIPEthernet;    // !!!!!!!!!!!!!

uint8_t UIP_ARPHDRSIZE = 42;
#define UDPBUF ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])

uint8_t UIPUDP::begin(uint16_t port)
{
    if (!_uip_udp_conn)
        _uip_udp_conn = uip_udp_new(NULL, 0);

    if (_uip_udp_conn)
    {
        uip_udp_bind(_uip_udp_conn,htons(port));
        _uip_udp_conn->appstate = &appdata;
        return 1;
    }

    return 0;
}

// Finish with the UDP socket
void UIPUDP::stop()
{
    if (_uip_udp_conn)
    {
        uip_udp_remove(_uip_udp_conn);
        _uip_udp_conn->appstate = NULL;
        _uip_udp_conn=NULL;
        Enc28J60Network::freeBlock(appdata.packet_in);
        Enc28J60Network::freeBlock(appdata.packet_next);
        Enc28J60Network::freeBlock(appdata.packet_out);
        memset(&appdata,0,sizeof(appdata));
    }
}

int UIPUDP::beginPacket(uint32_t ip, uint16_t port)
{
    UIPEthernetClass::instance->tick();

    if (ip && port)
    {
        uip_ipaddr_t ripaddr;
        uip_ip_addr(&ripaddr, IPAddrezz(ip));

        if (_uip_udp_conn)
        {
            _uip_udp_conn->rport = htons(port);
            uip_ipaddr_copy(_uip_udp_conn->ripaddr, &ripaddr);
        }
        else
        {
            _uip_udp_conn = uip_udp_new(&ripaddr,htons(port));

            if (_uip_udp_conn)
                _uip_udp_conn->appstate = &appdata;
            else
                return 0;
        }
    }

    if (_uip_udp_conn)
    {
        if (appdata.packet_out == NOBLOCK)
        {
            appdata.packet_out = Enc28J60Network::allocBlock(UIP_UDP_MAXPACKETSIZE);
            appdata.out_pos = UIP_UDP_PHYH_LEN;

            if (appdata.packet_out != NOBLOCK)
                return 1;
        }
    }
    return 0;
}

int UIPUDP::beginPacket(const char *host, uint16_t port)
{
    // Look up the host first
    int ret = 0;
    DNSClient dns;
    IPAddrezz remote_addr;
    dns.begin(UIPEthernet.dnsServerIP());
    ret = dns.getHostByName(host, remote_addr);
    return ret == 1 ? beginPacket(remote_addr, port) : ret;
}

int UIPUDP::endPacket()
{
    if (_uip_udp_conn && appdata.packet_out != NOBLOCK)
    {
        appdata.send = true;
        Enc28J60Network::resizeBlock(appdata.packet_out,0,appdata.out_pos);
        uip_udp_periodic_conn(_uip_udp_conn);

        if (uip_len > 0)
        {
            _send(&appdata);
            return 1;
        }
    }
    return 0;
}

// Write size bytes from buffer into the packet
size_t UIPUDP::write(const uint8_t *buffer, size_t size)
{
    if (appdata.packet_out != NOBLOCK)
    {
        size_t ret = Enc28J60Network::instance->writePacket(appdata.packet_out,
            appdata.out_pos,(uint8_t*)buffer,size);

        appdata.out_pos += ret;
        return ret;
    }
    return 0;
}

// Start processing the next available incoming packet
// Returns the size of the packet in bytes, or 0 if no packets are available
int UIPUDP::parsePacket()
{
    UIPEthernetClass::instance->tick();
    Enc28J60Network::freeBlock(appdata.packet_in);
    appdata.packet_in = appdata.packet_next;
    appdata.packet_next = NOBLOCK;
    int size = Enc28J60Network::instance->blockSize(appdata.packet_in);
    return size;
}

// Number of bytes remaining in the current packet
int UIPUDP::available()
{
    UIPEthernetClass::instance->tick();
    return Enc28J60Network::instance->blockSize(appdata.packet_in);
}

// Read a single byte from the current packet
int UIPUDP::read()
{
    uint8_t c;

    if (read(&c,1) > 0)
        return c;
    
    return -1;
}

// Read up to len bytes from the current packet and place them into buffer
// Returns the number of bytes read, or 0 if none are available
int UIPUDP::read(unsigned char* buffer, size_t len)
{
    UIPEthernetClass::instance->tick();

    if (appdata.packet_in != NOBLOCK)
    {
        memaddress read = Enc28J60Network::instance->readPacket(appdata.packet_in,0,buffer,len);

        if (read == Enc28J60Network::instance->blockSize(appdata.packet_in))
        {
            Enc28J60Network::instance->freeBlock(appdata.packet_in);
            appdata.packet_in = NOBLOCK;
        }
        else
        {
            Enc28J60Network::instance->resizeBlock(appdata.packet_in,read);
        }

        return read;
    }
  return 0;
}

// Return the next byte from the current packet without moving on to the next byte
int UIPUDP::peek()
{
    UIPEthernetClass::instance->tick();

    if (appdata.packet_in != NOBLOCK)
    {
        uint8_t c;

        if (Enc28J60Network::instance->readPacket(appdata.packet_in,0,&c,1) == 1)
            return c;
    }

    return -1;
}

// Finish reading the current packet
void UIPUDP::flush()
{
    UIPEthernetClass::instance->tick();
    Enc28J60Network::freeBlock(appdata.packet_in);
    appdata.packet_in = NOBLOCK;
}

// Return the IP address of the host who sent the current incoming packet
IPAddrezz UIPUDP::remoteIP()
{
    //return _uip_udp_conn ? ip_addr_uip(_uip_udp_conn->ripaddr) : IPAddrezz();
    return _uip_udp_conn ? ip_addr_uip(_uip_udp_conn->ripaddr) : IPAddrezz();
}

// uIP callback function
void uipudp_appcall()
{
    if (uip_udp_userdata_t *data = (uip_udp_userdata_t *)(uip_udp_conn->appstate))
    {
        if (uip_newdata())
        {
            if (data->packet_next == NOBLOCK)
            {
                uip_udp_conn->rport = UDPBUF->srcport;
                uip_ipaddr_copy(uip_udp_conn->ripaddr,UDPBUF->srcipaddr);
                data->packet_next = Enc28J60Network::allocBlock(ntohs(UDPBUF->udplen)-UIP_UDPH_LEN);
                  //if we are unable to allocate memory the packet is dropped.
                    // udp doesn't guarantee packet delivery
                if (data->packet_next != NOBLOCK)
                {
                    //discard Linklevel and IP and udp-header and any trailing bytes:
                    Enc28J60Network::instance->copyPacket(
                        data->packet_next,0,UIPEthernetClass::instance->in_packet,
                        UIP_UDP_PHYH_LEN,
                        Enc28J60Network::instance->blockSize(data->packet_next));
                }
            }
        }

        if (uip_poll() && data->send)
        {
            //set uip_slen (uip private) by calling uip_udp_send
            UIPEthernetClass::instance->uip_packet = data->packet_out;
            UIPEthernetClass::uip_hdrlen = UIP_UDP_PHYH_LEN;
            uip_udp_send(data->out_pos - (UIP_UDP_PHYH_LEN));
        }
    }
}

void UIPUDP::_send(uip_udp_userdata_t *data)
{
    uip_arp_out(); //add arp

    if (uip_len == UIP_ARPHDRSIZE)
    {
        UIPEthernetClass::instance->uip_packet = NOBLOCK;
        UIPEthernetClass::packetstate &= ~UIPETHERNET_SENDPACKET;
    }
    else
    {
        data->send = false;
        data->packet_out = NOBLOCK;
        UIPEthernetClass::packetstate |= UIPETHERNET_SENDPACKET;
    }

    UIPEthernetClass::instance->network_send();
}


