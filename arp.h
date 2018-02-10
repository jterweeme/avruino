#ifndef __UIP_ARP_H__
#define __UIP_ARP_H__
#include "uip.h"

extern struct uip_eth_addr uip_ethaddr;

struct uip_eth_hdr {
  struct uip_eth_addr dest;
  struct uip_eth_addr src;
  uint16_t type;
};

#define UIP_ETHTYPE_ARP 0x0806
#define UIP_ETHTYPE_IP  0x0800
#define UIP_ETHTYPE_IP6 0x86dd


void uip_arp_init(void);
void uip_arp_ipin(void);
void uip_arp_arpin(void);
void uip_arp_out(void);
void uip_arp_timer(void);

#define uip_setethaddr(eaddr) do {uip_ethaddr.addr[0] = eaddr.addr[0]; \
                              uip_ethaddr.addr[1] = eaddr.addr[1];\
                              uip_ethaddr.addr[2] = eaddr.addr[2];\
                              uip_ethaddr.addr[3] = eaddr.addr[3];\
                              uip_ethaddr.addr[4] = eaddr.addr[4];\
                              uip_ethaddr.addr[5] = eaddr.addr[5];} while(0)


#endif



