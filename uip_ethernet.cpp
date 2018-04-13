#include "uip_ethernet.h"
#include "uip.h"
#include "arp.h"
#include "uip_client.h"

#define ETH_HDR ((struct uip_eth_hdr *)&uip_buf[0])

#define uip_input()        uip_process(UIP_DATA)

static uint32_t g_millis = 0;

uint32_t millis()
{
    return g_millis;
}

UIPEthernetClass *UIPEthernetClass::instance;
memhandle UIPEthernetClass::in_packet(NOBLOCK);
memhandle UIPEthernetClass::uip_packet(NOBLOCK);
uint8_t UIPEthernetClass::uip_hdrlen(0);
uint8_t UIPEthernetClass::packetstate(0);
IPAddrezz UIPEthernetClass::_dnsServerAddress;
DhcpClass* UIPEthernetClass::_dhcp(NULL);
unsigned long UIPEthernetClass::periodic_timer;

UIPEthernetClass::UIPEthernetClass()
{
    instance = this;
}

static DhcpClass s_dhcp;

int UIPEthernetClass::begin(const uint8_t* mac)
{
    _dhcp = &s_dhcp;
    init(mac);
    int ret = _dhcp->beginWithDHCP((uint8_t*)mac);

    if(ret == 1)
    {
        configure(_dhcp->getLocalIp(),_dhcp->getDnsServerIp(),_dhcp->getGatewayIp(),
            _dhcp->getSubnetMask());
    }
    return ret;
}

#define uip_sethostaddr(addr) uip_ipaddr_copy(uip_hostaddr, (addr))
#define uip_setdraddr(addr) uip_ipaddr_copy(uip_draddr, (addr))
#define uip_setnetmask(addr) uip_ipaddr_copy(uip_netmask, (addr))
#define uip_getdraddr(addr) uip_ipaddr_copy((addr), uip_draddr)
#define uip_getnetmask(addr) uip_ipaddr_copy((addr), uip_netmask)

void UIPEthernetClass::tick2()
{
    g_millis++;
}

void UIPEthernetClass::begin(const uint8_t* mac, IPAddrezz ip)
{
    IPAddrezz dns = ip;
    dns[3] = 1;
    begin(mac, ip, dns);
}

void UIPEthernetClass::begin(const uint8_t* mac, IPAddrezz ip, IPAddrezz dns)
{
    IPAddrezz gateway = ip;
    gateway[3] = 1;
    begin(mac, ip, dns, gateway);
}

void
UIPEthernetClass::begin(const uint8_t* mac, IPAddrezz ip, IPAddrezz dns, IPAddrezz gateway)
{
    IPAddrezz subnet(255, 255, 255, 0);
    begin(mac, ip, dns, gateway, subnet);
}

void
UIPEthernetClass::begin(const uint8_t* mac, IPAddrezz ip, IPAddrezz dns,
    IPAddrezz gateway, IPAddrezz subnet)
{
  init(mac);
  configure(ip,dns,gateway,subnet);
}

int UIPEthernetClass::maintain()
{
    tick();
    int rc = DHCP_CHECK_NONE;

    if(_dhcp != NULL)
    {
        rc = _dhcp->checkLease();

        switch ( rc )
        {
            case DHCP_CHECK_NONE:
                break;
            case DHCP_CHECK_RENEW_OK:
            case DHCP_CHECK_REBIND_OK:
                configure(_dhcp->getLocalIp(),
                    _dhcp->getDnsServerIp(),_dhcp->getGatewayIp(),_dhcp->getSubnetMask());
                break;
            default:
                break;
        }
    }
    return rc;
}

IPAddrezz UIPEthernetClass::localIP()
{
    IPAddrezz ret;
    uip_ipaddr_t a;
    uip_ipaddr_copy(a, uip_hostaddr);
    return ip_addr_uip(a);
}

IPAddrezz UIPEthernetClass::subnetMask()
{
    IPAddrezz ret;
    uip_ipaddr_t a;
    uip_getnetmask(a);
    return ip_addr_uip(a);
}

IPAddrezz UIPEthernetClass::gatewayIP()
{
    IPAddrezz ret;
    uip_ipaddr_t a;
    uip_getdraddr(a);
    return ip_addr_uip(a);
}

IPAddrezz UIPEthernetClass::dnsServerIP()
{
    return _dnsServerAddress;
}

void UIPEthernetClass::tick()
{
    if (in_packet == NOBLOCK)
        in_packet = _nw.receivePacket();

    if (in_packet != NOBLOCK)
    {
        packetstate = UIPETHERNET_FREEPACKET;
        uip_len = _nw.blockSize(in_packet);

        if (uip_len > 0)
        {
            Enc28J60Network::instance->readPacket(in_packet,0,(uint8_t*)uip_buf,UIP_BUFSIZE);

            if (ETH_HDR ->type == HTONS(UIP_ETHTYPE_IP))
            {
                uip_packet = in_packet; //required for upper_layer_checksum of in_packet!
                uip_arp_ipin();
                uip_input();

                if (uip_len > 0)
                {
                    uip_arp_out();
                    network_send();
                }
            }
            else if (ETH_HDR ->type == HTONS(UIP_ETHTYPE_ARP))
            {
                uip_arp_arpin();

                if (uip_len > 0)
                    network_send();
            }
        }

        if (in_packet != NOBLOCK && (packetstate & UIPETHERNET_FREEPACKET))
        {
          _nw.freePacket();
          in_packet = NOBLOCK;
        }
    }

  unsigned long now = millis();

#if UIP_CLIENT_TIMER >= 0
    bool periodic = (long)(now - periodic_timer) >= 0;

    for (int i = 0; i < UIP_CONNS; i++)
    {
#else
  if ((long)( now - periodic_timer ) >= 0)
    {
      periodic_timer = now + UIP_PERIODIC_TIMER;

      for (int i = 0; i < UIP_CONNS; i++)
        {
#endif
      uip_conn = &uip_conns[i];
#if UIP_CLIENT_TIMER >= 0
      if (periodic)
        {
#endif
          uip_process(UIP_TIMER);
#if UIP_CLIENT_TIMER >= 0
        }
      else
        {
          if ((long)( now - ((uip_userdata_t*)uip_conn->appstate)->timer) >= 0)
            uip_process(UIP_POLL_REQUEST);
          else
            continue;
        }
#endif
      if (uip_len > 0)
        {
          uip_arp_out();
          network_send();
        }
    }
#if UIP_CLIENT_TIMER >= 0
    if (periodic)
    {
        periodic_timer = now + UIP_PERIODIC_TIMER;
#endif
        for (int i = 0; i < UIP_UDP_CONNS; i++)
        {
            uip_udp_conn = &uip_udp_conns[i];
            uip_process(UIP_UDP_TIMER);

            if (uip_len > 0)
            {
                UIPUDP::_send((uip_udp_userdata_t *)(uip_udp_conns[i].appstate));
            }
        }
    }
}

bool UIPEthernetClass::network_send()
{
    if (packetstate & UIPETHERNET_SENDPACKET)
    {
        _nw.writePacket(uip_packet,0,uip_buf,uip_hdrlen);
        packetstate &= ~ UIPETHERNET_SENDPACKET;
        goto sendandfree;
    }

    uip_packet = _nw.allocBlock(uip_len);

    if (uip_packet != NOBLOCK)
    {
        _nw.writePacket(uip_packet,0,uip_buf,uip_len);
        goto sendandfree;
    }

    return false;
sendandfree:
    _nw.sendPacket(uip_packet);
    _nw.freeBlock(uip_packet);
    uip_packet = NOBLOCK;
    return true;
}

void UIPEthernetClass::init(const uint8_t* mac)
{
    periodic_timer = millis() + UIP_PERIODIC_TIMER;
    _nw.init((uint8_t*)mac);
    uip_seteth_addr(mac);
    uip_init();
    uip_arp_init();
}

void UIPEthernetClass::configure(IPAddrezz ip, IPAddrezz dns,
    IPAddrezz gateway, IPAddrezz subnet)
{
    uip_ipaddr_t ipaddr;
    uip_ip_addr(ipaddr, ip);
    uip_sethostaddr(ipaddr);
    uip_ip_addr(ipaddr, gateway);
    uip_setdraddr(ipaddr);
    uip_ip_addr(ipaddr, subnet);
    uip_setnetmask(ipaddr);
    _dnsServerAddress = dns;
}

UIPEthernetClass UIPEthernet;

uint16_t UIPEthernetClass::chksum(uint16_t sum, const uint8_t *data, uint16_t len)
{
  uint16_t t;
  const uint8_t *dataptr;
  const uint8_t *last_byte;

  dataptr = data;
  last_byte = data + len - 1;

  while(dataptr < last_byte) {  /* At least two more bytes */
    t = (dataptr[0] << 8) + dataptr[1];
    sum += t;
    if(sum < t) {
      sum++;            /* carry */
    }
    dataptr += 2;
  }

  if(dataptr == last_byte) {
    t = (dataptr[0] << 8) + 0;
    sum += t;
    if(sum < t) {
      sum++;            /* carry */
    }
  }

  /* Return sum in host byte order. */
  return sum;
}

uint16_t UIPEthernetClass::ipchksum(void)
{
    uint16_t sum = chksum(0, &uip_buf[UIP_LLH_LEN], UIP_IPH_LEN);
    return sum == 0 ? 0xffff : htons(sum);
}

uint16_t
#if UIP_UDP
UIPEthernetClass::upper_layer_chksum(uint8_t proto)
#else
uip_tcpchksum(void)
#endif
{
  uint16_t upper_layer_len;
  uint16_t sum;

#if UIP_CONF_IPV6
  upper_layer_len = (((uint16_t)(BUF->len[0]) << 8) + BUF->len[1]);
#else /* UIP_CONF_IPV6 */
  upper_layer_len = (((uint16_t)(BUF->len[0]) << 8) + BUF->len[1]) - UIP_IPH_LEN;
#endif /* UIP_CONF_IPV6 */

#if UIP_UDP
  sum = upper_layer_len + proto;
#else
  sum = upper_layer_len + UIP_PROTO_TCP;
#endif
  /* Sum IP source and destination addresses. */
  sum = UIPEthernetClass::chksum(sum, (uint8_t *)&BUF->srcipaddr[0], 2 * sizeof(uip_ipaddr_t));

  uint8_t upper_layer_memlen;
  switch(proto)
  {
  case UIP_PROTO_UDP:
    upper_layer_memlen = UIP_UDPH_LEN;
    break;
  default:
    upper_layer_memlen = (BUF->tcpoffset >> 4) << 2;
    break;
  }
  sum = UIPEthernetClass::chksum(sum, &uip_buf[UIP_IPH_LEN + UIP_LLH_LEN], upper_layer_memlen);

  if (upper_layer_memlen < upper_layer_len)
    {
      sum = _nw.chksum(sum, uip_packet,
            UIP_IPH_LEN + UIP_LLH_LEN + upper_layer_memlen,
            upper_layer_len - upper_layer_memlen);
    }
  return (sum == 0) ? 0xffff : htons(sum);
}

uint16_t uip_ipchksum()
{
    return UIPEthernet.ipchksum();
}

uint16_t uip_tcpchksum()
{
    uint16_t sum = UIPEthernet.upper_layer_chksum(UIP_PROTO_TCP);
    return sum;
}

uint16_t uip_udpchksum()
{
    uint16_t sum = UIPEthernet.upper_layer_chksum(UIP_PROTO_UDP);
    return sum;
}



