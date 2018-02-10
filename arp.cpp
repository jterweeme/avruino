#include "arp.h"
#include <string.h>

#ifdef UIP_CONF_ARPTAB_SIZE
#define UIP_ARPTAB_SIZE UIP_CONF_ARPTAB_SIZE
#else
#define UIP_ARPTAB_SIZE 8
#endif

static const uint8_t UIP_ARP_MAXAGE = 120;

struct arp_hdr
{
    struct uip_eth_hdr ethhdr;
    uint16_t hwtype;
    uint16_t protocol;
    uint8_t hwlen;
    uint8_t protolen;
    uint16_t opcode;
    struct uip_eth_addr shwaddr;
    uint16_t sipaddr[2];
    struct uip_eth_addr dhwaddr;
    uint16_t dipaddr[2];
};

struct ethip_hdr
{
    struct uip_eth_hdr ethhdr;
    uint8_t vhl, tos, len[2], ipid[2], ipoffset[2], ttl, proto;
    uint16_t ipchksum;
    uint16_t srcipaddr[2], destipaddr[2];
};

static const uint8_t ARP_REQUEST = 1, ARP_REPLY = 2, ARP_HWTYPE_ETH = 1;

struct arp_entry
{
    uint16_t ipaddr[2];
    struct uip_eth_addr ethaddr;
    uint8_t time;
};

static const struct uip_eth_addr broadcast_ethaddr = {{0xff,0xff,0xff,0xff,0xff,0xff}};
static const uint16_t broadcast_ipaddr[2] = {0xffff,0xffff};

static struct arp_entry arp_table[UIP_ARPTAB_SIZE];
static uint16_t ipaddr[2];
static uint8_t i, c;

static uint8_t arptime;
static uint8_t tmpage;

#define BUF   ((struct arp_hdr *)&uip_buf[0])
#define IPBUF ((struct ethip_hdr *)&uip_buf[0])

void uip_arp_init(void)
{
    for (i = 0; i < UIP_ARPTAB_SIZE; ++i)
        memset(arp_table[i].ipaddr, 0, 4);
}

void uip_arp_timer(void)
{
  struct arp_entry *tabptr;
  
  ++arptime;
  for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {
    tabptr = &arp_table[i];
    if((tabptr->ipaddr[0] | tabptr->ipaddr[1]) != 0 &&
       arptime - tabptr->time >= UIP_ARP_MAXAGE) {
      memset(tabptr->ipaddr, 0, 4);
    }
  }

}

static void uip_arp_update(uint16_t *ipaddr, struct uip_eth_addr *ethaddr)
{
    register struct arp_entry *tabptr;
  
    for(i = 0; i < UIP_ARPTAB_SIZE; ++i)
    {
        tabptr = &arp_table[i];

        if(tabptr->ipaddr[0] != 0 && tabptr->ipaddr[1] != 0)
        {
            if(ipaddr[0] == tabptr->ipaddr[0] && ipaddr[1] == tabptr->ipaddr[1])
            {
	            memcpy(tabptr->ethaddr.addr, ethaddr->addr, 6);
            	tabptr->time = arptime;
            	return;
            }
        }
    }

    for (i = 0; i < UIP_ARPTAB_SIZE; ++i)
    {
        tabptr = &arp_table[i];

        if(tabptr->ipaddr[0] == 0 && tabptr->ipaddr[1] == 0)
        {
            break;
        }
    }

  /* If no unused entry is found, we try to find the oldest entry and
     throw it away. */
  if(i == UIP_ARPTAB_SIZE) {
    tmpage = 0;
    c = 0;
    for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {
      tabptr = &arp_table[i];
      if(arptime - tabptr->time > tmpage) {
	tmpage = arptime - tabptr->time;
	c = i;
      }
    }
    i = c;
    tabptr = &arp_table[i];
  }

  /* Now, i is the ARP table entry which we will fill with the new
     information. */
  memcpy(tabptr->ipaddr, ipaddr, 4);
  memcpy(tabptr->ethaddr.addr, ethaddr->addr, 6);
  tabptr->time = arptime;
}
/*-----------------------------------------------------------------------------------*/
/**
 * ARP processing for incoming IP packets
 *
 * This function should be called by the device driver when an IP
 * packet has been received. The function will check if the address is
 * in the ARP cache, and if so the ARP cache entry will be
 * refreshed. If no ARP cache entry was found, a new one is created.
 *
 * This function expects an IP packet with a prepended Ethernet header
 * in the uip_buf[] buffer, and the length of the packet in the global
 * variable uip_len.
 */
/*-----------------------------------------------------------------------------------*/
void uip_arp_ipin(void)
{
  uip_len -= sizeof(struct uip_eth_hdr);
	
  /* Only insert/update an entry if the source IP address of the
     incoming IP packet comes from a host on the local network. */
  if((IPBUF->srcipaddr[0] & uip_netmask[0]) !=
     (uip_hostaddr[0] & uip_netmask[0])) {
    return;
  }
  if((IPBUF->srcipaddr[1] & uip_netmask[1]) !=
     (uip_hostaddr[1] & uip_netmask[1])) {
    return;
  }
  uip_arp_update(IPBUF->srcipaddr, &(IPBUF->ethhdr.src));
  return;
}

void uip_arp_arpin(void)
{
  
  if(uip_len < sizeof(struct arp_hdr)) {
    uip_len = 0;
    return;
  }
  uip_len = 0;
  
  switch(BUF->opcode) {
  case HTONS(ARP_REQUEST):
    /* ARP request. If it asked for our address, we send out a
       reply. */
    if(uip_ipaddr_cmp(BUF->dipaddr, uip_hostaddr)) {
      /* First, we register the one who made the request in our ARP
	 table, since it is likely that we will do more communication
	 with this host in the future. */
      uip_arp_update(BUF->sipaddr, &BUF->shwaddr);
      BUF->opcode = HTONS(2);
      memcpy(BUF->dhwaddr.addr, BUF->shwaddr.addr, 6);
      memcpy(BUF->shwaddr.addr, uip_ethaddr.addr, 6);
      memcpy(BUF->ethhdr.src.addr, uip_ethaddr.addr, 6);
      memcpy(BUF->ethhdr.dest.addr, BUF->dhwaddr.addr, 6);
      BUF->dipaddr[0] = BUF->sipaddr[0];
      BUF->dipaddr[1] = BUF->sipaddr[1];
      BUF->sipaddr[0] = uip_hostaddr[0];
      BUF->sipaddr[1] = uip_hostaddr[1];
      BUF->ethhdr.type = HTONS(UIP_ETHTYPE_ARP);
      uip_len = sizeof(struct arp_hdr);
    }
    break;
  case HTONS(ARP_REPLY):
    /* ARP reply. We insert or update the ARP table if it was meant
       for us. */
    if(uip_ipaddr_cmp(BUF->dipaddr, uip_hostaddr)) {
      uip_arp_update(BUF->sipaddr, &BUF->shwaddr);
    }
    break;
  }

  return;
}
/*-----------------------------------------------------------------------------------*/
/**
 * Prepend Ethernet header to an outbound IP packet and see if we need
 * to send out an ARP request.
 *
 * This function should be called before sending out an IP packet. The
 * function checks the destination IP address of the IP packet to see
 * what Ethernet MAC address that should be used as a destination MAC
 * address on the Ethernet.
 *
 * If the destination IP address is in the local network (determined
 * by logical ANDing of netmask and our IP address), the function
 * checks the ARP cache to see if an entry for the destination IP
 * address is found. If so, an Ethernet header is prepended and the
 * function returns. If no ARP cache entry is found for the
 * destination IP address, the packet in the uip_buf[] is replaced by
 * an ARP request packet for the IP address. The IP packet is dropped
 * and it is assumed that they higher level protocols (e.g., TCP)
 * eventually will retransmit the dropped packet.
 *
 * If the destination IP address is not on the local network, the IP
 * address of the default router is used instead.
 *
 * When the function returns, a packet is present in the uip_buf[]
 * buffer, and the length of the packet is in the global variable
 * uip_len.
 */
/*-----------------------------------------------------------------------------------*/
void uip_arp_out(void)
{
  struct arp_entry *tabptr;
  
  /* Find the destination IP address in the ARP table and construct
     the Ethernet header. If the destination IP addres isn't on the
     local network, we use the default router's IP address instead.

     If not ARP table entry is found, we overwrite the original IP
     packet with an ARP request for the IP address. */

  /* First check if destination is a local broadcast. */
  if(uip_ipaddr_cmp(IPBUF->destipaddr, broadcast_ipaddr)) {
    memcpy(IPBUF->ethhdr.dest.addr, broadcast_ethaddr.addr, 6);
  } else {
    /* Check if the destination address is on the local network. */
    if(!uip_ipaddr_maskcmp(IPBUF->destipaddr, uip_hostaddr, uip_netmask)) {
      /* Destination address was not on the local network, so we need to
	 use the default router's IP address instead of the destination
	 address when determining the MAC address. */
      uip_ipaddr_copy(ipaddr, uip_draddr);
    } else {
      /* Else, we use the destination IP address. */
      uip_ipaddr_copy(ipaddr, IPBUF->destipaddr);
    }
      
    for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {
      tabptr = &arp_table[i];
      if(uip_ipaddr_cmp(ipaddr, tabptr->ipaddr)) {
	break;
      }
    }

    if(i == UIP_ARPTAB_SIZE) {
      /* The destination address was not in our ARP table, so we
	 overwrite the IP packet with an ARP request. */

      memset(BUF->ethhdr.dest.addr, 0xff, 6);
      memset(BUF->dhwaddr.addr, 0x00, 6);
      memcpy(BUF->ethhdr.src.addr, uip_ethaddr.addr, 6);
      memcpy(BUF->shwaddr.addr, uip_ethaddr.addr, 6);
    
      uip_ipaddr_copy(BUF->dipaddr, ipaddr);
      uip_ipaddr_copy(BUF->sipaddr, uip_hostaddr);
      BUF->opcode = HTONS(ARP_REQUEST); /* ARP request. */
      BUF->hwtype = HTONS(ARP_HWTYPE_ETH);
      BUF->protocol = HTONS(UIP_ETHTYPE_IP);
      BUF->hwlen = 6;
      BUF->protolen = 4;
      BUF->ethhdr.type = HTONS(UIP_ETHTYPE_ARP);

      uip_appdata = &uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN];
    
      uip_len = sizeof(struct arp_hdr);
      return;
    }

    /* Build an ethernet header. */
    memcpy(IPBUF->ethhdr.dest.addr, tabptr->ethaddr.addr, 6);
  }
  memcpy(IPBUF->ethhdr.src.addr, uip_ethaddr.addr, 6);
  
  IPBUF->ethhdr.type = HTONS(UIP_ETHTYPE_IP);

  uip_len += sizeof(struct uip_eth_hdr);
}



