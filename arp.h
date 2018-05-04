#ifndef _UIP_ARP_H_
#define _UIP_ARP_H_
#include "enc28j60ip.h"

extern struct uip_eth_addr uip_ethaddr;

struct uip_eth_hdr
{
    struct uip_eth_addr dest;
    struct uip_eth_addr src;
    uint16_t type;
};

#define UIP_ETHTYPE_ARP 0x0806
#define UIP_ETHTYPE_IP  0x0800
#define UIP_ETHTYPE_IP6 0x86dd

void uip_arp_init();
void uip_arp_ipin();
void uip_arp_arpin();
void uip_arp_out();
void uip_arp_timer();
#endif



