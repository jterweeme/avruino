#include <string.h>
#include "arp.h"

static uint32_t g_millis = 0;

uint32_t millis()
{
    return g_millis;
}

UIPEthernetClass *UIPEthernetClass::instance;

void UIPEthernetClass::tick2()
{
    g_millis++;
}

void UIPEthernetClass::_flushBlocks(memhandle* block)
{
    for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++)
    {
        _nw.freeBlock(block[i]);
        block[i] = NOBLOCK;
    }
}

void UIPEthernetClass::uipclient_appcall()
{
    uint16_t send_len = 0;
    uip_userdata_t *u = (uip_userdata_t*)uip_conn->appstate;

    if (!u && (uip_flags & UIP_CONNECTED))
    {
        u = (uip_userdata_t*)_allocateData();

        if (u)
            uip_conn->appstate = u;
    }

    if (u)
    {
        if (uip_flags & UIP_NEWDATA)
        {
            if (uip_len && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
            {
                for (uint8_t i=0; i < UIP_SOCKET_NUMPACKETS; i++)
                {
                    if (u->packets_in[i] == NOBLOCK)
                    {
                        u->packets_in[i] = _nw.allocBlock(uip_len);

                        if (u->packets_in[i] != NOBLOCK)
                        {
                            _nw.copyPacket(u->packets_in[i], 0, in_packet,
                                ((uint8_t*)uip_appdata) - uip_buf,uip_len);

                            if (i == UIP_SOCKET_NUMPACKETS-1)
                                uip_conn->tcpstateflags |= UIP_STOPPED;

                            goto finish_newdata;
                        }
                    }
                }
                packetstate &= ~UIPETHERNET_FREEPACKET;
                uip_conn->tcpstateflags |= UIP_STOPPED;
            }
        }
finish_newdata:
        if (u->state & UIP_CLIENT_RESTART)
        {
            u->state &= ~UIP_CLIENT_RESTART;

            uip_flags |= UIP_NEWDATA;
            uip_conn->tcpstateflags &= ~UIP_STOPPED;

            //uip_restart();
        }

        // If the connection has been closed, save received but unread data.
        if ((uip_flags & UIP_CLOSE) || (uip_flags & UIP_TIMEDOUT))
        {
            // drop outgoing packets not sent yet:
            _flushBlocks(&u->packets_out[0]);

            if (u->packets_in[0] != NOBLOCK)
            {
                ((uip_userdata_closed_t *)u)->lport = uip_conn->lport;
                u->state |= UIP_CLIENT_REMOTECLOSED;
            }
            else
            {
                u->state = 0;
            }

            uip_conn->appstate = NULL;
            goto finish;
        }

        if (uip_flags & UIP_ACKDATA)
            _eatBlock(&u->packets_out[0]);

        if ((uip_flags & UIP_POLL) || (uip_flags & UIP_REXMIT))
        {
            if (u->packets_out[0] != NOBLOCK)
            {
                if (u->packets_out[1] == NOBLOCK)
                {
                    send_len = u->out_pos;

                    if (send_len > 0)
                        _nw.resizeBlock(u->packets_out[0],0,send_len);
                }
                else
                {
                    send_len = _nw.blockSize(u->packets_out[0]);
                }

                if (send_len > 0)
                {
                    uip_hdrlen = ((uint8_t*)uip_appdata)-uip_buf;
                    uip_packet = _nw.allocBlock(uip_hdrlen+send_len);

                    if (uip_packet != NOBLOCK)
                    {
                        _nw.copyPacket(uip_packet, uip_hdrlen,u->packets_out[0],0,send_len);
                        packetstate |= UIPETHERNET_SENDPACKET;
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
                uip_conn->tcpstateflags |= UIP_STOPPED;
            }
        }
    }
finish:
    uip_send(uip_appdata,send_len);
    uip_len = send_len;
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
            _nw.readPacket(in_packet, 0, (uint8_t*)uip_buf,UIP_BUFSIZE);

            if (((struct uip_eth_hdr *)&uip_buf[0])->type == HTONS(UIP_ETHTYPE_IP))
            {
                uip_packet = in_packet; //required for upper_layer_checksum of in_packet!
                uip_arp_ipin();
                process(UIP_DATA);

                if (uip_len > 0)
                {
                    uip_arp_out();
                    network_send();
                }
            }
            else if (((struct uip_eth_hdr *)&uip_buf[0])->type == HTONS(UIP_ETHTYPE_ARP))
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

    uint32_t now = millis();
    bool periodic = (long)(now - periodic_timer) >= 0;

    for (int i = 0; i < UIP_CONNS; i++)
    {
        uip_conn = &uip_conns[i];

        if (periodic)
        {
            process(UIP_TIMER);
        }
        else
        {
            if ((long)( now - ((uip_userdata_t*)uip_conn->appstate)->timer) >= 0)
                process(UIP_POLL_REQUEST);
            else
                continue;
        }

        if (uip_len > 0)
        {
            uip_arp_out();
            network_send();
        }
    }

    if (periodic)
    {
        periodic_timer = now + UIP_PERIODIC_TIMER;
        for (int i = 0; i < UIP_UDP_CONNS; i++)
        {
            uip_udp_conn = &uip_udp_conns[i];
            process(UIP_UDP_TIMER);

            if (uip_len > 0)
                _send((uip_udp_userdata_t *)(uip_udp_conns[i].appstate));
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

    for (uint8_t i = 0; i <= 5; i++)
        uip_ethaddr.addr[i] = mac[i];

    uip_init();
    uip_arp_init();
}

void UIPEthernetClass::configure(uint32_t ip, uint32_t dns, uint32_t gw, uint32_t subnet)
{
    uip_hostaddr[0] = (uint16_t)(ip & 0xffff);
    uip_hostaddr[1] = (uint16_t)(ip >> 16 & 0xffff);
    uip_draddr[0] = (uint16_t)(gw & 0xffff);
    uip_draddr[1] = (uint16_t)(gw >> 16 & 0xffff);
    uip_netmask[0] = (uint16_t)(subnet & 0xffff);
    uip_netmask[1] = (uint16_t)(subnet >> 16 & 0xffff);
    _dnsServerAddress = dns;
}

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

uint16_t UIPEthernetClass::ipchksum()
{
    uint16_t sum = chksum(0, &uip_buf[UIP_LLH_LEN], UIP_IPH_LEN);
    return sum == 0 ? 0xffff : htons(sum);
}

#define BUF ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])

uint16_t UIPEthernetClass::upper_layer_chksum(uint8_t proto)
{
    uint16_t sum;

    uint16_t upper_layer_len =
        (((uint16_t)(((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])->len[0]) << 8) +
        ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])->len[1]) - UIP_IPH_LEN;

    sum = upper_layer_len + proto;

    sum = chksum(sum,
        (uint8_t *)&((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])->srcipaddr[0],
        2 * sizeof(uip_ipaddr_t));

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
    sum = chksum(sum, &uip_buf[UIP_IPH_LEN + UIP_LLH_LEN], upper_layer_memlen);

    if (upper_layer_memlen < upper_layer_len)
    {
        sum = _nw.chksum(sum, uip_packet, UIP_IPH_LEN + UIP_LLH_LEN + upper_layer_memlen,
            upper_layer_len - upper_layer_memlen);
    }
    return (sum == 0) ? 0xffff : htons(sum);
}

static constexpr uint8_t
    UIP_PROTO_ICMP = 1, ICMP_ECHO_REPLY = 0, ICMP_ECHO = 8,
    UIP_RTO = 3, UIP_MAXRTX = 8, UIP_MAXSYNRTX = 5,
    UIP_TTL = 64, UIP_REASSEMBLY = 0, UIP_REASS_MAXAGE = 40,
    TCP_FIN = 0x01, TCP_SYN = 0x02, TCP_RST = 0x04, TCP_PSH = 0x08,
    TCP_ACK = 0x10, TCP_URG = 0x20, TCP_CTL = 0x3f,
    TCP_OPT_END = 0, TCP_OPT_NOOP = 1, TCP_OPT_MSS = 2,
    TCP_OPT_MSS_LEN = 4;

void uip_add32(uint8_t *op32, uint16_t op16);
uip_ipaddr_t uip_hostaddr, uip_draddr, uip_netmask;
static const uip_ipaddr_t all_zeroes_addr = {0x0000,0x0000};
struct uip_eth_addr uip_ethaddr = {{0,0,0,0,0,0}};

#ifndef UIP_CONF_EXTERNAL_BUFFER
uint8_t uip_buf[UIP_BUFSIZE + 2];
#endif

void *uip_appdata;
void *uip_sappdata;
uint16_t uip_len;
static uint16_t uip_slen;
uint8_t uip_flags;
struct uip_conn *uip_conn;   
struct uip_conn uip_conns[UIP_CONNS];
uint16_t uip_listenports[UIP_LISTENPORTS];
struct uip_udp_conn *uip_udp_conn;
struct uip_udp_conn uip_udp_conns[UIP_UDP_CONNS];
static uint16_t ipid;
void uip_setipid(uint16_t id) { ipid = id; }
static uint8_t iss[4];
static uint16_t lastport;  /* Keeps track of the last port used for a new connection. */
uint8_t uip_acc32[4];
static uint8_t c, opt;
static uint16_t tmp16;

struct uip_icmpip_hdr
{
    uint8_t vhl, tos, len[2], ipid[2], ipoffset[2], ttl, proto;
    uint16_t ipchksum;
    uint16_t srcipaddr[2], destipaddr[2];
    uint8_t type, icode;
    uint16_t icmpchksum;
    uint16_t id, seqno;
};

#define BUF ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define FBUF ((struct uip_tcpip_hdr *)&uip_reassbuf[0])
#define ICMPBUF ((struct uip_icmpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UDPBUF ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])

#if UIP_LOGGING == 1
#include <stdio.h>
void uip_log(char *msg);
#define UIP_LOG(m) uip_log(m)
#else
#define UIP_LOG(m)
#endif

void uip_add32(uint8_t *op32, uint16_t op16)
{
    uip_acc32[3] = op32[3] + (op16 & 0xff);
    uip_acc32[2] = op32[2] + (op16 >> 8);
    uip_acc32[1] = op32[1];
    uip_acc32[0] = op32[0];
  
    if(uip_acc32[2] < (op16 >> 8))
    {
        ++uip_acc32[1];

        if (uip_acc32[1] == 0)
            ++uip_acc32[0];
    }
  
  
    if(uip_acc32[3] < (op16 & 0xff))
    {
        ++uip_acc32[2];

        if (uip_acc32[2] == 0)
        {
            ++uip_acc32[1];

            if (uip_acc32[1] == 0)
	            ++uip_acc32[0];
        }
    }
}

void uip_init(void)
{
    for (c = 0; c < UIP_LISTENPORTS; ++c)
        uip_listenports[c] = 0;
  
    for (c = 0; c < UIP_CONNS; ++c)
        uip_conns[c].tcpstateflags = UIP_CLOSED;

    lastport = 1024;

    for (c = 0; c < UIP_UDP_CONNS; ++c)
        uip_udp_conns[c].lport = 0;
}

struct uip_conn *uip_connect(uip_ipaddr_t *ripaddr, uint16_t rport)
{
    struct uip_conn *conn, *cconn;
  
    // Find an unused local port.
again:
    ++lastport;

    if (lastport >= 32000)
        lastport = 4096;

    /* Check if this port is already in use, and if so try to find
     another one. */
    for (c = 0; c < UIP_CONNS; ++c)
    {
        conn = &uip_conns[c];

        if (conn->tcpstateflags != UIP_CLOSED && conn->lport == htons(lastport))
            goto again;
    }

    conn = 0;

    for(c = 0; c < UIP_CONNS; ++c)
    {
    cconn = &uip_conns[c];
    if(cconn->tcpstateflags == UIP_CLOSED) {
      conn = cconn;
      break;
    }
    if(cconn->tcpstateflags == UIP_TIME_WAIT) {
      if(conn == 0 ||
	 cconn->timer > conn->timer) {
	conn = cconn;
      }
    }
  }

    if (conn == 0)
        return 0;
  
    conn->tcpstateflags = UIP_SYN_SENT;
    conn->snd_nxt[0] = iss[0];
    conn->snd_nxt[1] = iss[1];
    conn->snd_nxt[2] = iss[2];
    conn->snd_nxt[3] = iss[3];
    conn->initialmss = conn->mss = UIP_TCP_MSS;
    conn->len = 1;   /* TCP length of the SYN is one. */
    conn->nrtx = 0;
    conn->timer = 1; /* Send the SYN next time around. */
    conn->rto = UIP_RTO;
    conn->sa = 0;
    conn->sv = 16;   /* Initial value of the RTT variance. */
    conn->lport = htons(lastport);
    conn->rport = rport;
    ((uint16_t *)&conn->ripaddr)[0] = ((uint16_t *)ripaddr)[0];
    ((uint16_t *)&conn->ripaddr)[1] = ((uint16_t *)ripaddr)[1];
    return conn;
}

struct uip_udp_conn *uip_udp_new(uip_ipaddr_t *ripaddr, uint16_t rport)
{
    struct uip_udp_conn *conn;
again:
    ++lastport;

    if (lastport >= 32000)
        lastport = 4096;
  
    for (c = 0; c < UIP_UDP_CONNS; ++c)
        if (uip_udp_conns[c].lport == htons(lastport))
            goto again;

    conn = 0;

    for (c = 0; c < UIP_UDP_CONNS; ++c)
    {
        if(uip_udp_conns[c].lport == 0)
        {
            conn = &uip_udp_conns[c];
            break;
        }
    }

    if(conn == 0)
        return 0;
  
    conn->lport = HTONS(lastport);
    conn->rport = rport;

    if (ripaddr == NULL)
    {
        memset(conn->ripaddr, 0, sizeof(uip_ipaddr_t));
    }
    else
    {
        conn->ripaddr[0] = (uint16_t)ripaddr[0];
        conn->ripaddr[1] = (uint16_t)ripaddr[1];
        //uip_ipaddr_copy(&conn->ripaddr, ripaddr);
    }
    
    conn->ttl = UIP_TTL;
    return conn;
}

void uip_listen(uint16_t port)
{
    for (c = 0; c < UIP_LISTENPORTS; ++c)
    {
        if (uip_listenports[c] == 0)
        {
            uip_listenports[c] = port;
            return;
        }
    }
}

static void uip_add_rcv_nxt(uint16_t n)
{
    uip_add32(uip_conn->rcv_nxt, n);
    uip_conn->rcv_nxt[0] = uip_acc32[0];
    uip_conn->rcv_nxt[1] = uip_acc32[1];
    uip_conn->rcv_nxt[2] = uip_acc32[2];
    uip_conn->rcv_nxt[3] = uip_acc32[3];
}

#define uip_outstanding(conn) ((conn)->len)
static const uip_ipaddr_t all_ones_addr = {0xffff,0xffff};

#if 0
static inline void uip_ipaddr_copy(uint16_t *dst, uint16_t *src)
{
    ((uint16_t *)dst)[0] = ((uint16_t *)src)[0];
    ((uint16_t *)dst)[1] = ((uint16_t *)src)[1];
}
#else
#define uip_ipaddr_copy(dest, src) do { \
                     ((uint16_t *)dest)[0] = ((uint16_t *)src)[0]; \
                     ((uint16_t *)dest)[1] = ((uint16_t *)src)[1]; \
                  } while(0)
#endif



void UIPEthernetClass::uip_process(uint8_t flag)
{
    register struct uip_conn *uip_connr = uip_conn;

    if (flag == UIP_UDP_SEND_CONN)
        goto udp_send;

    uip_sappdata = uip_appdata = &uip_buf[UIP_IPTCPH_LEN + UIP_LLH_LEN];

    /* Check if we were invoked because of a poll request for a
     particular connection. */
    if (flag == UIP_POLL_REQUEST)
    {
        if ((uip_connr->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED &&
            !uip_outstanding(uip_connr))
        {
            uip_flags = UIP_POLL;
            uipclient_appcall();
	        goto appsend;
        }
        goto drop;
    
    /* Check if we were invoked because of the perodic timer fireing. */
    }
    else if (flag == UIP_TIMER)
    {

    /* Increase the initial sequence number. */
    if(++iss[3] == 0) {
      if(++iss[2] == 0) {
	if(++iss[1] == 0) {
	  ++iss[0];
	}
      }
    }

    /* Reset the length variables. */
    uip_len = 0;
    uip_slen = 0;

    /* Check if the connection is in a state in which we simply wait
       for the connection to time out. If so, we increase the
       connection's timer and remove the connection if it times
       out. */
    if(uip_connr->tcpstateflags == UIP_TIME_WAIT ||
       uip_connr->tcpstateflags == UIP_FIN_WAIT_2) {
      ++(uip_connr->timer);
      if(uip_connr->timer == UIP_TIME_WAIT_TIMEOUT) {
	uip_connr->tcpstateflags = UIP_CLOSED;
      }
    } else if(uip_connr->tcpstateflags != UIP_CLOSED) {
      /* If the connection has outstanding data, we increase the
	 connection's timer and see if it has reached the RTO value
	 in which case we retransmit. */
      if(uip_outstanding(uip_connr)) {
	if(uip_connr->timer-- == 0) {
	  if(uip_connr->nrtx == UIP_MAXRTX ||
	     ((uip_connr->tcpstateflags == UIP_SYN_SENT ||
	       uip_connr->tcpstateflags == UIP_SYN_RCVD) &&
	      uip_connr->nrtx == UIP_MAXSYNRTX)) {
	    uip_connr->tcpstateflags = UIP_CLOSED;

	    /* We call UIP_APPCALL() with uip_flags set to
	       UIP_TIMEDOUT to inform the application that the
	       connection has timed out. */
	    uip_flags = UIP_TIMEDOUT;
        uipclient_appcall();

	    /* We also send a reset packet to the remote host. */
	    BUF->flags = TCP_RST | TCP_ACK;
	    goto tcp_send_nodata;
	  }

	  /* Exponential backoff. */
	  uip_connr->timer = UIP_RTO << (uip_connr->nrtx > 4?
					 4:
					 uip_connr->nrtx);
	  ++(uip_connr->nrtx);
	  
	  /* Ok, so we need to retransmit. We do this differently
	     depending on which state we are in. In ESTABLISHED, we
	     call upon the application so that it may prepare the
	     data for the retransmit. In SYN_RCVD, we resend the
	     SYNACK that we sent earlier and in LAST_ACK we have to
	     retransmit our FINACK. */
	  switch(uip_connr->tcpstateflags & UIP_TS_MASK) {
	  case UIP_SYN_RCVD:
	    /* In the SYN_RCVD state, we should retransmit our
               SYNACK. */
	    goto tcp_send_synack;
	    
	  case UIP_SYN_SENT:
	    BUF->flags = 0;
	    goto tcp_send_syn;
	    
	  case UIP_ESTABLISHED:
	    /* In the ESTABLISHED state, we call upon the application
               to do the actual retransmit after which we jump into
               the code for sending out the packet (the apprexmit
               label). */
	    uip_flags = UIP_REXMIT;
        uipclient_appcall();
	    goto apprexmit;
	    
	  case UIP_FIN_WAIT_1:
	  case UIP_CLOSING:
	  case UIP_LAST_ACK:
	    /* In all these states we should retransmit a FINACK. */
	    goto tcp_send_finack;
	    
	  }
	}
      } else if((uip_connr->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED) {
	/* If there was no need for a retransmission, we poll the
           application for new data. */
	uip_flags = UIP_POLL;
    uipclient_appcall();
	goto appsend;
      }
    }
    goto drop;
  }
  if(flag == UIP_UDP_TIMER) {
    if(uip_udp_conn->lport != 0) {
      uip_conn = NULL;
      uip_sappdata = uip_appdata = &uip_buf[UIP_LLH_LEN + UIP_IPUDPH_LEN];
      uip_len = uip_slen = 0;
      uip_flags = UIP_POLL;
        uipudp_appcall();
      //UIP_UDP_APPCALL();
      goto udp_send;
    } else {
      goto drop;
    }
  }

    /* This is where the input processing starts.
        Start of IP input header processing code. */
  
    if (BUF->vhl != 0x45)
        goto drop;  // IP version and header length
  
    if ((uint16_t)((BUF->len[0] << 8) + BUF->len[1]) <= uip_len)
    {
        uip_len = (BUF->len[0] << 8) + BUF->len[1];
    } else {
        UIP_LOG("ip: packet shorter than reported in IP header.");
        goto drop;
    }

#if !UIP_CONF_IPV6
    if((BUF->ipoffset[0] & 0x3f) != 0 || BUF->ipoffset[1] != 0)
    {
        goto drop;
    }
#endif

  if(uip_ipaddr_cmp(uip_hostaddr, all_zeroes_addr)) {
    /* If we are configured to use ping IP address configuration and
       hasn't been assigned an IP address yet, we accept all ICMP
       packets. */
#if UIP_PINGADDRCONF && !UIP_CONF_IPV6
    if(BUF->proto == UIP_PROTO_ICMP) {
      UIP_LOG("ip: possible ping config packet received.");
      goto icmp_input;
    } else {
      UIP_LOG("ip: packet dropped since no address assigned.");
      goto drop;
    }
#endif /* UIP_PINGADDRCONF */

  } else {
    /* If IP broadcast support is configured, we check for a broadcast
       UDP packet, which may be destined to us. */
#if UIP_BROADCAST
    if (BUF->proto == UIP_PROTO_UDP &&
       uip_ipaddr_cmp(BUF->destipaddr, all_ones_addr))
    {
        goto udp_input;
    }
#endif /* UIP_BROADCAST */
    
    /* Check if the packet is destined for our IP address. */
#if !UIP_CONF_IPV6
    if (!uip_ipaddr_cmp(BUF->destipaddr, uip_hostaddr))
    {
        goto drop;
    }
#else
    /* For IPv6, packet reception is a little trickier as we need to
       make sure that we listen to certain multicast addresses (all
       hosts multicast address, and the solicited-node multicast
       address) as well. However, we will cheat here and accept all
       multicast packets that are sent to the ff02::/16 addresses. */
    if(!uip_ipaddr_cmp(BUF->destipaddr, uip_hostaddr) &&
       BUF->destipaddr[0] != HTONS(0xff02))
    {
        goto drop;
    }
#endif /* UIP_CONF_IPV6 */
  }

  if(BUF->proto == UIP_PROTO_TCP) {
    goto tcp_input;
  }

  if(BUF->proto == UIP_PROTO_UDP) {
    goto udp_input;
  }

    if(BUF->proto != UIP_PROTO_ICMP) { /* We only allow ICMP packets from here. */
        UIP_LOG("ip: neither tcp nor icmp.");
        goto drop;
    }

#if UIP_PINGADDRCONF
icmp_input:
#endif
  /* ICMP echo (i.e., ping) processing. This is simple, we only change
     the ICMP type from ECHO to ECHO_REPLY and adjust the ICMP
     checksum before we return the packet. */
  if(ICMPBUF->type != ICMP_ECHO) {
    goto drop;
  }

  /* If we are configured to use ping IP address assignment, we use
     the destination IP address of this ping packet and assign it to
     ourself. */
#if UIP_PINGADDRCONF
    if((uip_hostaddr[0] | uip_hostaddr[1]) == 0)
    {
        uip_hostaddr[0] = BUF->destipaddr[0];
        uip_hostaddr[1] = BUF->destipaddr[1];
    }
#endif

  ICMPBUF->type = ICMP_ECHO_REPLY;

    if (ICMPBUF->icmpchksum >= HTONS(0xffff - (ICMP_ECHO << 8)))
    {
        ICMPBUF->icmpchksum += HTONS(ICMP_ECHO << 8) + 1;
    }
    else
    {
        ICMPBUF->icmpchksum += HTONS(ICMP_ECHO << 8);
    }

    uip_ipaddr_copy(BUF->destipaddr, BUF->srcipaddr);
    uip_ipaddr_copy(BUF->srcipaddr, uip_hostaddr);
    goto send;

#if UIP_UDP
    // UDP input processing.
udp_input:
  /* UDP processing is really just a hack. We don't do anything to the
     UDP/IP headers, but let the UDP application do all the hard
     work. If the application sets uip_slen, it has a packet to
     send. */
#if UIP_UDP_CHECKSUMS
    uip_len = uip_len - UIP_IPUDPH_LEN;
    uip_appdata = &uip_buf[UIP_LLH_LEN + UIP_IPUDPH_LEN];

    if (UDPBUF->udpchksum != 0 && upper_layer_chksum(UIP_PROTO_UDP) != 0xffff)
        goto drop;
#else
    uip_len = uip_len - UIP_IPUDPH_LEN;
#endif

    /* Demultiplex this UDP packet between the UDP "connections". */
    for(uip_udp_conn = &uip_udp_conns[0]; uip_udp_conn < &uip_udp_conns[UIP_UDP_CONNS];
      ++uip_udp_conn)
    {

        if (uip_udp_conn->lport != 0 && UDPBUF->destport == uip_udp_conn->lport &&
           (uip_udp_conn->rport == 0 || UDPBUF->srcport == uip_udp_conn->rport) &&
           (uip_ipaddr_cmp(uip_udp_conn->ripaddr, all_zeroes_addr) ||
            uip_ipaddr_cmp(uip_udp_conn->ripaddr, all_ones_addr) ||
            uip_ipaddr_cmp(BUF->srcipaddr, uip_udp_conn->ripaddr)))
        {
            goto udp_found;
        }
    }
  UIP_LOG("udp: no matching connection found");
  goto drop;
  
udp_found:
    uip_conn = NULL;
    uip_flags = UIP_NEWDATA;
    uip_sappdata = uip_appdata = &uip_buf[UIP_LLH_LEN + UIP_IPUDPH_LEN];
    uip_slen = 0;
    uipudp_appcall();
udp_send:
    if(uip_slen == 0)
        goto drop;

    uip_len = uip_slen + UIP_IPUDPH_LEN;
    BUF->len[0] = (uip_len >> 8);
    BUF->len[1] = (uip_len & 0xff);
    BUF->ttl = uip_udp_conn->ttl;
    BUF->proto = UIP_PROTO_UDP;
    UDPBUF->udplen = HTONS(uip_slen + UIP_UDPH_LEN);
    UDPBUF->udpchksum = 0;
    BUF->srcport  = uip_udp_conn->lport;
    BUF->destport = uip_udp_conn->rport;
    uip_ipaddr_copy(BUF->srcipaddr, uip_hostaddr);
    uip_ipaddr_copy(BUF->destipaddr, uip_udp_conn->ripaddr);
    uip_appdata = &uip_buf[UIP_LLH_LEN + UIP_IPTCPH_LEN];
#if UIP_UDP_CHECKSUMS
    /* Calculate UDP checksum. */
    UDPBUF->udpchksum = ~(upper_layer_chksum(UIP_PROTO_UDP));

    if(UDPBUF->udpchksum == 0)
        UDPBUF->udpchksum = 0xffff;
#endif /* UIP_UDP_CHECKSUMS */
  
  goto ip_send_nolen;
#endif /* UIP_UDP */
  
tcp_input:
  /* Start of TCP input header processing code. */
  
  if (upper_layer_chksum(UIP_PROTO_TCP) != 0xffff) { 
    goto drop;
  }
  
  /* Demultiplex this segment. */
  /* First check any active connections. */
  for(uip_connr = &uip_conns[0]; uip_connr <= &uip_conns[UIP_CONNS - 1];
      ++uip_connr) {
    if(uip_connr->tcpstateflags != UIP_CLOSED &&
       BUF->destport == uip_connr->lport &&
       BUF->srcport == uip_connr->rport &&
       uip_ipaddr_cmp(BUF->srcipaddr, uip_connr->ripaddr)) {
      goto found;
    }
  }

  /* If we didn't find and active connection that expected the packet,
     either this packet is an old duplicate, or this is a SYN packet
     destined for a connection in LISTEN. If the SYN flag isn't set,
     it is an old packet and we send a RST. */
  if((BUF->flags & TCP_CTL) != TCP_SYN) {
    goto reset;
  }
  
  tmp16 = BUF->destport;
  /* Next, check listening connections. */
  for(c = 0; c < UIP_LISTENPORTS; ++c) {
    if(tmp16 == uip_listenports[c])
      goto found_listen;
  }
  
  /* No matching connection found, so we send a RST packet. */
reset:
    // We do not send resets in response to resets.
    if (BUF->flags & TCP_RST)
        goto drop;

    BUF->flags = TCP_RST | TCP_ACK;
    uip_len = UIP_IPTCPH_LEN;
    BUF->tcpoffset = 5 << 4;

    // Flip the seqno and ackno fields in the TCP header.
    c = BUF->seqno[3];
    BUF->seqno[3] = BUF->ackno[3];
    BUF->ackno[3] = c;
    c = BUF->seqno[2];
    BUF->seqno[2] = BUF->ackno[2];
    BUF->ackno[2] = c;
    c = BUF->seqno[1];
    BUF->seqno[1] = BUF->ackno[1];
    BUF->ackno[1] = c;
    c = BUF->seqno[0];
    BUF->seqno[0] = BUF->ackno[0];
    BUF->ackno[0] = c;

    /* We also have to increase the sequence number we are
     acknowledging. If the least significant byte overflowed, we need
     to propagate the carry to the other bytes as well. */
    if(++BUF->ackno[3] == 0)
        if(++BUF->ackno[2] == 0)
            if(++BUF->ackno[1] == 0)
            	++BUF->ackno[0];
 
    // Swap port numbers.
    tmp16 = BUF->srcport;
    BUF->srcport = BUF->destport;
    BUF->destport = tmp16;
  
    // Swap IP addresses.
    uip_ipaddr_copy(BUF->destipaddr, BUF->srcipaddr);
    uip_ipaddr_copy(BUF->srcipaddr, uip_hostaddr);
  
    // And send out the RST packet!
    goto tcp_send_noconn;

  /* This label will be jumped to if we matched the incoming packet
     with a connection in LISTEN. In that case, we should create a new
     connection and send a SYNACK in return. */
found_listen:
  /* First we check if there are any connections avaliable. Unused
     connections are kept in the same table as used connections, but
     unused ones have the tcpstate set to CLOSED. Also, connections in
     TIME_WAIT are kept track of and we'll use the oldest one if no
     CLOSED connections are found. Thanks to Eddie C. Dost for a very
     nice algorithm for the TIME_WAIT search. */
  uip_connr = 0;
  for(c = 0; c < UIP_CONNS; ++c) {
    if(uip_conns[c].tcpstateflags == UIP_CLOSED) {
      uip_connr = &uip_conns[c];
      break;
    }
    if(uip_conns[c].tcpstateflags == UIP_TIME_WAIT) {
      if(uip_connr == 0 ||
	 uip_conns[c].timer > uip_connr->timer) {
	uip_connr = &uip_conns[c];
      }
    }
  }

  if(uip_connr == 0) {
    /* All connections are used already, we drop packet and hope that
       the remote end will retransmit the packet at a time when we
       have more spare connections. */
    UIP_LOG("tcp: found no unused connections.");
    goto drop;
  }
  uip_conn = uip_connr;
  
  /* Fill in the necessary fields for the new connection. */
  uip_connr->rto = uip_connr->timer = UIP_RTO;
  uip_connr->sa = 0;
  uip_connr->sv = 4;
  uip_connr->nrtx = 0;
  uip_connr->lport = BUF->destport;
  uip_connr->rport = BUF->srcport;
  uip_ipaddr_copy(uip_connr->ripaddr, BUF->srcipaddr);
  uip_connr->tcpstateflags = UIP_SYN_RCVD;

  uip_connr->snd_nxt[0] = iss[0];
  uip_connr->snd_nxt[1] = iss[1];
  uip_connr->snd_nxt[2] = iss[2];
  uip_connr->snd_nxt[3] = iss[3];
  uip_connr->len = 1;

  /* rcv_nxt should be the seqno from the incoming packet + 1. */
  uip_connr->rcv_nxt[3] = BUF->seqno[3];
  uip_connr->rcv_nxt[2] = BUF->seqno[2];
  uip_connr->rcv_nxt[1] = BUF->seqno[1];
  uip_connr->rcv_nxt[0] = BUF->seqno[0];
  uip_add_rcv_nxt(1);

  /* Parse the TCP MSS option, if present. */
  if((BUF->tcpoffset & 0xf0) > 0x50) {
    for(c = 0; c < ((BUF->tcpoffset >> 4) - 5) << 2 ;) {
      opt = uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN + c];
      if(opt == TCP_OPT_END) {
	/* End of options. */
	break;
      } else if(opt == TCP_OPT_NOOP) {
	++c;
	/* NOP option. */
      } else if(opt == TCP_OPT_MSS &&
		uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN + 1 + c] == TCP_OPT_MSS_LEN) {
	/* An MSS option with the right option length. */
	tmp16 = ((uint16_t)uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN + 2 + c] << 8) |
	  (uint16_t)uip_buf[UIP_IPTCPH_LEN + UIP_LLH_LEN + 3 + c];
	uip_connr->initialmss = uip_connr->mss =
	  tmp16 > UIP_TCP_MSS? UIP_TCP_MSS: tmp16;
	
	/* And we are done processing options. */
	break;
      } else {
	/* All other options have a length field, so that we easily
	   can skip past them. */
	if(uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN + 1 + c] == 0) {
	  /* If the length field is zero, the options are malformed
	     and we don't process them further. */
	  break;
	}
	c += uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN + 1 + c];
      }
    }
  }
  
tcp_send_synack:
    BUF->flags = TCP_ACK;
tcp_send_syn:
    BUF->flags |= TCP_SYN;
  
    // We send out the TCP Maximum Segment Size option with our SYNACK.
    BUF->optdata[0] = TCP_OPT_MSS;
    BUF->optdata[1] = TCP_OPT_MSS_LEN;
    BUF->optdata[2] = (UIP_TCP_MSS) / 256;
    BUF->optdata[3] = (UIP_TCP_MSS) & 255;
    uip_len = UIP_IPTCPH_LEN + TCP_OPT_MSS_LEN;
    BUF->tcpoffset = ((UIP_TCPH_LEN + TCP_OPT_MSS_LEN) / 4) << 4;
    goto tcp_send;
    // This label will be jumped to if we found an active connection.
found:
    uip_conn = uip_connr;
    uip_flags = 0;
    /* We do a very naive form of TCP reset processing; we just accept
     any RST and kill our connection. We should in fact check if the
     sequence number of this reset is wihtin our advertised window
     before we accept the reset. */
    if (BUF->flags & TCP_RST)
    {
        uip_connr->tcpstateflags = UIP_CLOSED;
        UIP_LOG("tcp: got reset, aborting connection.");
        uip_flags = UIP_ABORT;
        uipclient_appcall();
        goto drop;
    }
    /* Calculated the length of the data, if the application has sent
     any data to us. */
    c = (BUF->tcpoffset >> 4) << 2;
    /* uip_len will contain the length of the actual TCP data. This is
     calculated by subtracing the length of the TCP header (in
     c) and the length of the IP header (20 bytes). */
    uip_len = uip_len - c - UIP_IPH_LEN;

    if (!(((uip_connr->tcpstateflags & UIP_TS_MASK) == UIP_SYN_SENT) &&
       ((BUF->flags & TCP_CTL) == (TCP_SYN | TCP_ACK))))
    {
        if ((uip_len > 0 || ((BUF->flags & (TCP_SYN | TCP_FIN)) != 0)) &&
            (BUF->seqno[0] != uip_connr->rcv_nxt[0] ||
        	BUF->seqno[1] != uip_connr->rcv_nxt[1] ||
        	BUF->seqno[2] != uip_connr->rcv_nxt[2] ||
        	BUF->seqno[3] != uip_connr->rcv_nxt[3]))
        {
            goto tcp_send_ack;
        }
    }

    if ((BUF->flags & TCP_ACK) && uip_outstanding(uip_connr))
    {
        uip_add32(uip_connr->snd_nxt, uip_connr->len);

        if (BUF->ackno[0] == uip_acc32[0] && BUF->ackno[1] == uip_acc32[1] &&
            BUF->ackno[2] == uip_acc32[2] && BUF->ackno[3] == uip_acc32[3])
        {
            // Update sequence number.
            uip_connr->snd_nxt[0] = uip_acc32[0];
            uip_connr->snd_nxt[1] = uip_acc32[1];
            uip_connr->snd_nxt[2] = uip_acc32[2];
            uip_connr->snd_nxt[3] = uip_acc32[3];
	

      /* Do RTT estimation, unless we have done retransmissions. */
      if(uip_connr->nrtx == 0) {
	signed char m;
	m = uip_connr->rto - uip_connr->timer;
	/* This is taken directly from VJs original code in his paper */
	m = m - (uip_connr->sa >> 3);
	uip_connr->sa += m;
	if(m < 0) {
	  m = -m;
	}
	m = m - (uip_connr->sv >> 2);
	uip_connr->sv += m;
	uip_connr->rto = (uip_connr->sa >> 3) + uip_connr->sv;

      }
      /* Set the acknowledged flag. */
      uip_flags = UIP_ACKDATA;
      /* Reset the retransmission timer. */
      uip_connr->timer = uip_connr->rto;

      /* Reset length of outstanding data. */
      uip_connr->len = 0;
    }
    
  }

  /* Do different things depending on in what state the connection is. */
  switch(uip_connr->tcpstateflags & UIP_TS_MASK) {
    /* CLOSED and LISTEN are not handled here. CLOSE_WAIT is not
	implemented, since we force the application to close when the
	peer sends a FIN (hence the application goes directly from
	ESTABLISHED to LAST_ACK). */
  case UIP_SYN_RCVD:
    /* In SYN_RCVD we have sent out a SYNACK in response to a SYN, and
       we are waiting for an ACK that acknowledges the data we sent
       out the last time. Therefore, we want to have the UIP_ACKDATA
       flag set. If so, we enter the ESTABLISHED state. */
    if(uip_flags & UIP_ACKDATA) {
      uip_connr->tcpstateflags = UIP_ESTABLISHED;
      uip_flags = UIP_CONNECTED;
      uip_connr->len = 0;
      if(uip_len > 0) {
        uip_flags |= UIP_NEWDATA;
        uip_add_rcv_nxt(uip_len);
      }
      uip_slen = 0;
        uipclient_appcall();
      goto appsend;
    }
    goto drop;
  case UIP_SYN_SENT:
    /* In SYN_SENT, we wait for a SYNACK that is sent in response to
       our SYN. The rcv_nxt is set to sequence number in the SYNACK
       plus one, and we send an ACK. We move into the ESTABLISHED
       state. */
    if((uip_flags & UIP_ACKDATA) &&
       (BUF->flags & TCP_CTL) == (TCP_SYN | TCP_ACK)) {

      /* Parse the TCP MSS option, if present. */
      if((BUF->tcpoffset & 0xf0) > 0x50) {
	for(c = 0; c < ((BUF->tcpoffset >> 4) - 5) << 2 ;) {
	  opt = uip_buf[UIP_IPTCPH_LEN + UIP_LLH_LEN + c];
	  if(opt == TCP_OPT_END) {
	    /* End of options. */
	    break;
	  } else if(opt == TCP_OPT_NOOP) {
	    ++c;
	    /* NOP option. */
	  } else if(opt == TCP_OPT_MSS &&
		    uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN + 1 + c] == TCP_OPT_MSS_LEN) {
	    /* An MSS option with the right option length. */
	    tmp16 = (uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN + 2 + c] << 8) |
	      uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN + 3 + c];
	    uip_connr->initialmss =
	      uip_connr->mss = tmp16 > UIP_TCP_MSS? UIP_TCP_MSS: tmp16;

	    /* And we are done processing options. */
	    break;
	  } else {
	    /* All other options have a length field, so that we easily
	       can skip past them. */
	    if(uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN + 1 + c] == 0) {
	      /* If the length field is zero, the options are malformed
		 and we don't process them further. */
	      break;
	    }
	    c += uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN + 1 + c];
	  }
	}
      }
      uip_connr->tcpstateflags = UIP_ESTABLISHED;
      uip_connr->rcv_nxt[0] = BUF->seqno[0];
      uip_connr->rcv_nxt[1] = BUF->seqno[1];
      uip_connr->rcv_nxt[2] = BUF->seqno[2];
      uip_connr->rcv_nxt[3] = BUF->seqno[3];
      uip_add_rcv_nxt(1);
      uip_flags = UIP_CONNECTED | UIP_NEWDATA;
      uip_connr->len = 0;
      uip_len = 0;
      uip_slen = 0;
      uipclient_appcall();
      goto appsend;
    }
    /* Inform the application that the connection failed */
    uip_flags = UIP_ABORT;
    uipclient_appcall();
    /* The connection is closed after we send the RST */
    uip_conn->tcpstateflags = UIP_CLOSED;
    goto reset;
    
  case UIP_ESTABLISHED:
    /* In the ESTABLISHED state, we call upon the application to feed
    data into the uip_buf. If the UIP_ACKDATA flag is set, the
    application should put new data into the buffer, otherwise we are
    retransmitting an old segment, and the application should put that
    data into the buffer.

    If the incoming packet is a FIN, we should close the connection on
    this side as well, and we send out a FIN and enter the LAST_ACK
    state. We require that there is no outstanding data; otherwise the
    sequence numbers will be screwed up. */

    if(BUF->flags & TCP_FIN && !(uip_connr->tcpstateflags & UIP_STOPPED)) {
      if(uip_outstanding(uip_connr)) {
	goto drop;
      }
      uip_add_rcv_nxt(1 + uip_len);
      uip_flags |= UIP_CLOSE;
      if(uip_len > 0) {
	uip_flags |= UIP_NEWDATA;
      }
        uipclient_appcall();
      uip_connr->len = 1;
      uip_connr->tcpstateflags = UIP_LAST_ACK;
      uip_connr->nrtx = 0;
tcp_send_finack:
      BUF->flags = TCP_FIN | TCP_ACK;
      goto tcp_send_nodata;
    }

    /* Check the URG flag. If this is set, the segment carries urgent
       data that we must pass to the application. */
    if((BUF->flags & TCP_URG) != 0)
    {
      uip_appdata = ((char *)uip_appdata) + ((BUF->urgp[0] << 8) | BUF->urgp[1]);
      uip_len -= (BUF->urgp[0] << 8) | BUF->urgp[1];
    }

    if(uip_len > 0 && !(uip_connr->tcpstateflags & UIP_STOPPED)) {
      uip_flags |= UIP_NEWDATA;
      uip_add_rcv_nxt(uip_len);
    }

    tmp16 = ((uint16_t)BUF->wnd[0] << 8) + (uint16_t)BUF->wnd[1];

    if (tmp16 > uip_connr->initialmss || tmp16 == 0)
        tmp16 = uip_connr->initialmss;
    
    uip_connr->mss = tmp16;

    if (uip_flags & (UIP_NEWDATA | UIP_ACKDATA))
    {
        uip_slen = 0;
        uipclient_appcall();

appsend:
      
      if(uip_flags & UIP_ABORT) {
	uip_slen = 0;
	uip_connr->tcpstateflags = UIP_CLOSED;
	BUF->flags = TCP_RST | TCP_ACK;
	goto tcp_send_nodata;
      }

      if(uip_flags & UIP_CLOSE) {
	uip_slen = 0;
	uip_connr->len = 1;
	uip_connr->tcpstateflags = UIP_FIN_WAIT_1;
	uip_connr->nrtx = 0;
	BUF->flags = TCP_FIN | TCP_ACK;
	    goto tcp_send_nodata;
      }

      /* If uip_slen > 0, the application has data to be sent. */
      if(uip_slen > 0) {

	/* If the connection has acknowledged data, the contents of
	   the ->len variable should be discarded. */
	if((uip_flags & UIP_ACKDATA) != 0) {
	  uip_connr->len = 0;
	}

	/* If the ->len variable is non-zero the connection has
	   already data in transit and cannot send anymore right
	   now. */
	if(uip_connr->len == 0) {

	  /* The application cannot send more than what is allowed by
	     the mss (the minumum of the MSS and the available
	     window). */
	  if(uip_slen > uip_connr->mss) {
	    uip_slen = uip_connr->mss;
	  }

	  /* Remember how much data we send out now so that we know
	     when everything has been acknowledged. */
	  uip_connr->len = uip_slen;
	} else {

	  /* If the application already had unacknowledged data, we
	     make sure that the application does not send (i.e.,
	     retransmit) out more than it previously sent out. */
	  uip_slen = uip_connr->len;
	}
      }
      uip_connr->nrtx = 0;
    apprexmit:
      uip_appdata = uip_sappdata;
      
      /* If the application has data to be sent, or if the incoming
         packet had new data in it, we must send out a packet. */
      if(uip_slen > 0 && uip_connr->len > 0) {
	/* Add the length of the IP and TCP headers. */
	uip_len = uip_connr->len + UIP_TCPIP_HLEN;
	/* We always set the ACK flag in response packets. */
	BUF->flags = TCP_ACK | TCP_PSH;
	/* Send the packet. */
	goto tcp_send_noopts;
      }
      /* If there is no data to send, just send out a pure ACK if
	 there is newdata. */
      if(uip_flags & UIP_NEWDATA) {
	uip_len = UIP_TCPIP_HLEN;
	BUF->flags = TCP_ACK;
	goto tcp_send_noopts;
      }
    }
    goto drop;
  case UIP_LAST_ACK:
    /* We can close this connection if the peer has acknowledged our
       FIN. This is indicated by the UIP_ACKDATA flag. */
    if(uip_flags & UIP_ACKDATA) {
      uip_connr->tcpstateflags = UIP_CLOSED;
      uip_flags = UIP_CLOSE;
        uipclient_appcall();
    }
    break;
    
  case UIP_FIN_WAIT_1:
    /* The application has closed the connection, but the remote host
       hasn't closed its end yet. Thus we do nothing but wait for a
       FIN from the other side. */
    if(uip_len > 0) {
      uip_add_rcv_nxt(uip_len);
    }
    if(BUF->flags & TCP_FIN) {
      if(uip_flags & UIP_ACKDATA) {
	uip_connr->tcpstateflags = UIP_TIME_WAIT;
	uip_connr->timer = 0;
	uip_connr->len = 0;
      } else {
	uip_connr->tcpstateflags = UIP_CLOSING;
      }
      uip_add_rcv_nxt(1);
      uip_flags = UIP_CLOSE;
        uipclient_appcall();
      goto tcp_send_ack;
    } else if(uip_flags & UIP_ACKDATA) {
      uip_connr->tcpstateflags = UIP_FIN_WAIT_2;
      uip_connr->len = 0;
      goto drop;
    }
    if(uip_len > 0) {
      goto tcp_send_ack;
    }
    goto drop;
      
  case UIP_FIN_WAIT_2:
    if(uip_len > 0) {
      uip_add_rcv_nxt(uip_len);
    }
    if(BUF->flags & TCP_FIN) {
      uip_connr->tcpstateflags = UIP_TIME_WAIT;
      uip_connr->timer = 0;
      uip_add_rcv_nxt(1);
      uip_flags = UIP_CLOSE;
        uipclient_appcall();
      goto tcp_send_ack;
    }
    if(uip_len > 0) {
      goto tcp_send_ack;
    }
    goto drop;

  case UIP_TIME_WAIT:
    goto tcp_send_ack;
    
  case UIP_CLOSING:
    if(uip_flags & UIP_ACKDATA) {
      uip_connr->tcpstateflags = UIP_TIME_WAIT;
      uip_connr->timer = 0;
    }
  }
  goto drop;
  
tcp_send_ack:
    BUF->flags = TCP_ACK;
tcp_send_nodata:
    uip_len = UIP_IPTCPH_LEN;
tcp_send_noopts:
    BUF->tcpoffset = (UIP_TCPH_LEN / 4) << 4;
tcp_send:
    BUF->ackno[0] = uip_connr->rcv_nxt[0];
    BUF->ackno[1] = uip_connr->rcv_nxt[1];
    BUF->ackno[2] = uip_connr->rcv_nxt[2];
    BUF->ackno[3] = uip_connr->rcv_nxt[3];
    BUF->seqno[0] = uip_connr->snd_nxt[0];
    BUF->seqno[1] = uip_connr->snd_nxt[1];
    BUF->seqno[2] = uip_connr->snd_nxt[2];
    BUF->seqno[3] = uip_connr->snd_nxt[3];
    BUF->proto = UIP_PROTO_TCP;
    BUF->srcport  = uip_connr->lport;
    BUF->destport = uip_connr->rport;
    uip_ipaddr_copy(BUF->srcipaddr, uip_hostaddr);
    uip_ipaddr_copy(BUF->destipaddr, uip_connr->ripaddr);

    if(uip_connr->tcpstateflags & UIP_STOPPED) {
        BUF->wnd[0] = BUF->wnd[1] = 0;
    } else {
        BUF->wnd[0] = ((UIP_RECEIVE_WINDOW) >> 8);
        BUF->wnd[1] = ((UIP_RECEIVE_WINDOW) & 0xff);
    }

tcp_send_noconn:
    BUF->ttl = UIP_TTL;
    BUF->len[0] = (uip_len >> 8);
    BUF->len[1] = (uip_len & 0xff);
    BUF->urgp[0] = BUF->urgp[1] = 0;
    BUF->tcpchksum = 0;
    BUF->tcpchksum = ~(upper_layer_chksum(UIP_PROTO_TCP));
ip_send_nolen:
    BUF->vhl = 0x45;
    BUF->tos = 0;
    BUF->ipoffset[0] = BUF->ipoffset[1] = 0;
    ++ipid;
    BUF->ipid[0] = ipid >> 8;
    BUF->ipid[1] = ipid & 0xff;
    BUF->ipchksum = 0;
    BUF->ipchksum = ~(ipchksum());
send:
    uip_flags = 0;
    return;
drop:
    uip_len = 0;
    uip_flags = 0;
    return;
}

uint16_t htons(uint16_t val)
{
    return HTONS(val);
}

void uip_send(const void *data, int len)
{
    uip_slen = len;

    if (len > 0 && data != uip_sappdata)
        memcpy(uip_sappdata, (data), uip_slen);
}

void UIPEthernetClass::_send(uip_udp_userdata_t *data)
{
    uip_arp_out();

    if (uip_len == UIP_ARPHDRSIZE)
    {
        uip_packet = NOBLOCK;
        packetstate &= ~UIPETHERNET_SENDPACKET;
    }
    else
    {
        data->send = false;
        data->packet_out = NOBLOCK;
        packetstate |= UIPETHERNET_SENDPACKET;
    }

    network_send();
}

uint8_t UIPEthernetClass::_currentBlock(memhandle* block)
{
    for (uint8_t i = 1; i < UIP_SOCKET_NUMPACKETS; i++)
        if (block[i] == NOBLOCK)
            return i - 1;

    return UIP_SOCKET_NUMPACKETS-1;
}

void UIPEthernetClass::_eatBlock(memhandle* block)
{
    _nw.freeBlock(block[0]);

    for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS-1; i++)
        block[i] = block[i+1];

    block[UIP_SOCKET_NUMPACKETS-1] = NOBLOCK;
}

uip_userdata_t *UIPEthernetClass::_allocateData()
{
    for (uint8_t sock = 0; sock < UIP_CONNS; sock++)
    {
        uip_userdata_t* data = &UIPEthernetClass::all_data[sock];

        if (!data->state)
        {
            data->state = sock | UIP_CLIENT_CONNECTED;
            memset(&data->packets_in[0], 0, sizeof(uip_userdata_t) - sizeof(data->state));
            return data;
        }
    }
    return NULL;
}

#define UIP_SOCKET_DATALEN UIP_TCP_MSS

size_t UIPEthernetClass::_write(uip_userdata_t *u, const uint8_t *buf, size_t size)
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
            u->packets_out[p] = _nw.allocBlock(UIP_SOCKET_DATALEN);
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




