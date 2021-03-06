#include "enc28j60udp.h"
#include "misc.h"

uint8_t UIPUDP::begin(uint16_t port)
{
    if (!_uip_udp_conn)
        _uip_udp_conn = uip_udp_new(NULL, 0);

    if (_uip_udp_conn)
    {
        _uip_udp_conn->lport = htons(port);
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
        _uip_udp_conn->lport = 0;
        _uip_udp_conn->appstate = NULL;
        _uip_udp_conn=NULL;
        _eth->nw()->freeBlock(appdata.packet_in);
        _eth->nw()->freeBlock(appdata.packet_next);
        _eth->nw()->freeBlock(appdata.packet_out);
        my_memset(&appdata,0,sizeof(appdata));
    }
}

UIPUDP::UIPUDP(Enc28J60IP *eth) : _eth(eth)
{
    my_memset(&appdata, 0, sizeof(appdata));
}

int UIPUDP::beginPacket(uint32_t ip, uint16_t port)
{
    _eth->tick();

    if (ip && port)
    {
        uint16_t ripaddr[2];
        ripaddr[0] = (uint16_t)(ip & 0xffff);
        ripaddr[1] = (uint16_t)(ip >> 16 & 0xffff);

        if (_uip_udp_conn)
        {
            _uip_udp_conn->rport = htons(port);
            _uip_udp_conn->ripaddr[0] = ripaddr[0];
            _uip_udp_conn->ripaddr[1] = ripaddr[1];
        }
        else
        {
            _uip_udp_conn = uip_udp_new(&ripaddr, htons(port));

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
            appdata.packet_out = _eth->nw()->allocBlock(UIP_UDP_MAXPACKETSIZE);
            appdata.out_pos = UIP_UDP_PHYH_LEN;

            if (appdata.packet_out != NOBLOCK)
                return 1;
        }
    }
    return 0;
}

int UIPUDP::endPacket()
{
    if (_uip_udp_conn && appdata.packet_out != NOBLOCK)
    {
        appdata.send = true;
        _eth->nw()->resizeBlock(appdata.packet_out,0,appdata.out_pos);
        uip_udp_conn = _uip_udp_conn;
        _eth->process(UIP_UDP_TIMER);

        if (uip_len > 0)
        {
            _eth->_send(&appdata);
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
        size_t ret = _eth->nw()->writePacket(appdata.packet_out,
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
    _eth->tick();
    _eth->nw()->freeBlock(appdata.packet_in);
    appdata.packet_in = appdata.packet_next;
    appdata.packet_next = NOBLOCK;
    int size = _eth->nw()->blockSize(appdata.packet_in);
    return size;
}

// Number of bytes remaining in the current packet
int UIPUDP::available()
{
    _eth->tick();
    return _eth->nw()->blockSize(appdata.packet_in);
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
int UIPUDP::read(uint8_t *buffer, size_t len)
{
    _eth->tick();

    if (appdata.packet_in != NOBLOCK)
    {
        memaddress read = _eth->nw()->readPacket(appdata.packet_in,0,buffer,len);

        if (read == _eth->nw()->blockSize(appdata.packet_in))
        {
            _eth->nw()->freeBlock(appdata.packet_in);
            appdata.packet_in = NOBLOCK;
        }
        else
        {
            _eth->nw()->resizeBlock(appdata.packet_in,read);
        }

        return read;
    }
  return 0;
}

// Return the next byte from the current packet without moving on to the next byte
int UIPUDP::peek()
{
    _eth->tick();

    if (appdata.packet_in != NOBLOCK)
    {
        uint8_t c;

        if (_eth->nw()->readPacket(appdata.packet_in,0,&c,1) == 1)
            return c;
    }

    return -1;
}

// Finish reading the current packet
void UIPUDP::flush()
{
    _eth->tick();
    _eth->nw()->freeBlock(appdata.packet_in);
    appdata.packet_in = NOBLOCK;
}

uint32_t UIPUDP::remoteIP()
{
    return _uip_udp_conn ? *((uint32_t *)_uip_udp_conn->ripaddr) : 0;
}


