#ifndef _UIP_H_
#define _UIP_H_

#ifndef UIP_LITTLE_ENDIAN
#define UIP_LITTLE_ENDIAN  3412
#endif
#ifndef UIP_BIG_ENDIAN
#define UIP_BIG_ENDIAN     1234
#endif

#include "types.h"

#define UIP_SOCKET_NUMPACKETS    5
#define UIP_CONF_MAX_CONNECTIONS 4
#define UIP_CONF_UDP             1
#define UIP_CONF_BROADCAST       1
#define UIP_CONF_UDP_CONNS       4
#define UIP_ATTEMPTS_ON_WRITE    -1
#define UIP_CONNECT_TIMEOUT      -1
#define UIP_PERIODIC_TIMER       250
#define UIP_CLIENT_TIMER         10

typedef unsigned short uip_stats_t;

#define UIP_CONF_MAX_LISTENPORTS 4
#define UIP_CONF_BUFFER_SIZE     98
#define UIP_CONF_TCP_MSS 512
#define UIP_CONF_RECEIVE_WINDOW 512
#define UIP_CONF_BYTE_ORDER      LITTLE_ENDIAN
#define UIP_CONF_LOGGING         0
#define UIP_CONF_UDP_CHECKSUMS 1

typedef void* uip_tcp_appstate_t;
void uipclient_appcall(void);
typedef void* uip_udp_appstate_t;
void uipudp_appcall(void);

#define UIP_UDP_APPCALL uipudp_appcall
#define UIP_ARCH_CHKSUM 1

#ifdef UIP_CONF_PINGADDRCONF
#define UIP_PINGADDRCONF UIP_CONF_PINGADDRCONF
#else
#define UIP_PINGADDRCONF 0
#endif

#define UIP_UDP UIP_CONF_UDP

#ifdef UIP_CONF_UDP_CHECKSUMS
#define UIP_UDP_CHECKSUMS UIP_CONF_UDP_CHECKSUMS
#else
#define UIP_UDP_CHECKSUMS 0
#endif

#ifdef UIP_CONF_UDP_CONNS
#define UIP_UDP_CONNS UIP_CONF_UDP_CONNS
#else
#define UIP_UDP_CONNS    10
#endif

#define UIP_ACTIVE_OPEN 1

#ifndef UIP_CONF_MAX_CONNECTIONS
#define UIP_CONNS       10
#else
#define UIP_CONNS UIP_CONF_MAX_CONNECTIONS
#endif

#ifndef UIP_CONF_MAX_LISTENPORTS
#define UIP_LISTENPORTS 20
#else /* UIP_CONF_MAX_LISTENPORTS */
#define UIP_LISTENPORTS UIP_CONF_MAX_LISTENPORTS
#endif /* UIP_CONF_MAX_LISTENPORTS */

#ifndef UIP_CONF_TCP_MSS
#define UIP_TCP_MSS     (UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN)
#else
#define UIP_TCP_MSS UIP_CONF_TCP_MSS
#endif

#ifndef UIP_CONF_RECEIVE_WINDOW
#define UIP_RECEIVE_WINDOW UIP_TCP_MSS
#else
#define UIP_RECEIVE_WINDOW UIP_CONF_RECEIVE_WINDOW
#endif


#define UIP_TIME_WAIT_TIMEOUT 120

#ifndef UIP_CONF_BUFFER_SIZE
#define UIP_BUFSIZE     400
#else /* UIP_CONF_BUFFER_SIZE */
#define UIP_BUFSIZE UIP_CONF_BUFFER_SIZE
#endif /* UIP_CONF_BUFFER_SIZE */

#define UIP_CONF_STATISTICS 0
#ifndef UIP_CONF_STATISTICS
#define UIP_STATISTICS  0
#else /* UIP_CONF_STATISTICS */
#define UIP_STATISTICS UIP_CONF_STATISTICS
#endif /* UIP_CONF_STATISTICS */

#ifndef UIP_CONF_LOGGING
#define UIP_LOGGING     0
#else /* UIP_CONF_LOGGING */
#define UIP_LOGGING     UIP_CONF_LOGGING
#endif /* UIP_CONF_LOGGING */

#if UIP_UDP && UIP_CONF_BROADCAST
#define UIP_BROADCAST UIP_CONF_BROADCAST
#else /* UIP_CONF_BROADCAST */
#define UIP_BROADCAST 0
#endif /* UIP_CONF_BROADCAST */

#ifdef UIP_CONF_LLH_LEN
#define UIP_LLH_LEN UIP_CONF_LLH_LEN
#else
#define UIP_LLH_LEN     14
#endif


#ifdef UIP_CONF_BYTE_ORDER
#define UIP_BYTE_ORDER     UIP_CONF_BYTE_ORDER
#else
#define UIP_BYTE_ORDER     UIP_LITTLE_ENDIAN
#endif

#define UIP_URGDATA      0
#define UIP_FIXEDADDR    0

typedef uint16_t uip_ip4addr_t[2];
typedef uint16_t uip_ip6addr_t[8];
typedef uip_ip4addr_t uip_ipaddr_t;
void uip_init(void);
void uip_setipid(uint16_t id);
extern uint8_t uip_buf[UIP_BUFSIZE+2];
void uip_listen(uint16_t port);
void uip_unlisten(uint16_t port);
struct uip_conn *uip_connect(uip_ipaddr_t *ripaddr, uint16_t port);
void uip_send(const void *data, int len);


#define uip_udpconnection() (uip_conn == NULL)
#define uip_newdata()   (uip_flags & UIP_NEWDATA)
#define uip_acked()   (uip_flags & UIP_ACKDATA)
#define uip_connected() (uip_flags & UIP_CONNECTED)
#define uip_timedout()    (uip_flags & UIP_TIMEDOUT)
#define uip_rexmit()     (uip_flags & UIP_REXMIT)
#define uip_poll()       (uip_flags & UIP_POLL)
#define uip_initialmss()             (uip_conn->initialmss)
#define uip_mss()             (uip_conn->mss)

struct uip_udp_conn *uip_udp_new(uip_ipaddr_t *ripaddr, uint16_t rport);

#define uip_udp_remove(conn) (conn)->lport = 0
#define uip_udp_bind(conn, port) (conn)->lport = port

#define uip_ipaddr(addr, addr0,addr1,addr2,addr3) do { \
                     ((uint16_t *)(addr))[0] = HTONS(((addr0) << 8) | (addr1)); \
                     ((uint16_t *)(addr))[1] = HTONS(((addr2) << 8) | (addr3)); \
                  } while(0)


#define uip_ipaddr_copy(dest, src) do { \
                     ((uint16_t *)dest)[0] = ((uint16_t *)src)[0]; \
                     ((uint16_t *)dest)[1] = ((uint16_t *)src)[1]; \
                  } while(0)

#define uip_ipaddr_cmp(addr1, addr2) (((uint16_t *)addr1)[0] == ((uint16_t *)addr2)[0] && \
				      ((uint16_t *)addr1)[1] == ((uint16_t *)addr2)[1])

#ifndef HTONS
#   if UIP_BYTE_ORDER == UIP_BIG_ENDIAN
#      define HTONS(n) (n)
#   else /* UIP_BYTE_ORDER == UIP_BIG_ENDIAN */
#      define HTONS(n) (uint16_t)((((uint16_t) (n)) << 8) | (((uint16_t) (n)) >> 8))
#   endif /* UIP_BYTE_ORDER == UIP_BIG_ENDIAN */
#else
#error "HTONS already defined!"
#endif /* HTONS */


#ifndef htons
uint16_t htons(uint16_t val);
#endif
#ifndef ntohs
#define ntohs htons
#endif

extern void *uip_appdata;

#if UIP_URGDATA > 0
extern void *uip_urgdata;
#endif /* UIP_URGDATA > 0 */



extern uint16_t uip_len;

#if UIP_URGDATA > 0
extern uint16_t uip_urglen, uip_surglen;
#endif

struct uip_conn {
    uip_ipaddr_t ripaddr;   /**< The IP address of the remote host. */
    uint16_t lport;        /**< The local TCP port, in network byte order. */
    uint16_t rport;
    uint8_t rcv_nxt[4];
    uint8_t snd_nxt[4];
    uint16_t len;          /**< Length of the data that was previously sent. */
    uint16_t mss;          /**< Current maximum segment size for the connection. */
    uint16_t initialmss;   /**< Initial maximum segment size for the connection. */
    uint8_t sa;            /**< Retransmission time-out calculation state variable. */
    uint8_t sv;            /**< Retransmission time-out calculation state variable. */
    uint8_t rto;           /**< Retransmission time-out. */
    uint8_t tcpstateflags; /**< TCP state and flags. */
    uint8_t timer;         /**< The retransmission timer. */
    uint8_t nrtx;
    uip_tcp_appstate_t appstate;
};

extern struct uip_conn *uip_conn;
extern struct uip_conn uip_conns[UIP_CONNS];
extern uint8_t uip_acc32[4];

#if UIP_UDP
struct uip_udp_conn {
  uip_ipaddr_t ripaddr;   /**< The IP address of the remote peer. */
  uint16_t lport;        /**< The local port number in network byte order. */
  uint16_t rport;        /**< The remote port number in network byte order. */
  uint8_t  ttl;          /**< Default time-to-live. */

  uip_udp_appstate_t appstate;
};


extern struct uip_udp_conn *uip_udp_conn;
extern struct uip_udp_conn uip_udp_conns[UIP_UDP_CONNS];
#endif

extern uint8_t uip_flags;
void uip_process(uint8_t flag);

static constexpr uint8_t
    UIP_DATA = 1,
    UIP_TIMER = 2,
    UIP_POLL_REQUEST = 3,
    UIP_UDP_SEND_CONN = 4,
    UIP_UDP_TIMER = 5,
    UIP_CLOSED = 0,
    UIP_SYN_RCVD = 1,
    UIP_SYN_SENT = 2,
    UIP_ESTABLISHED = 3,
    UIP_FIN_WAIT_1 = 4,
    UIP_FIN_WAIT_2 = 5,
    UIP_CLOSING = 6,
    UIP_TIME_WAIT = 7,
    UIP_LAST_ACK = 8,
    UIP_TS_MASK = 15,
    UIP_STOPPED = 16,
    UIP_PROTO_TCP = 6,
    UIP_PROTO_UDP = 17,
    UIP_ACKDATA = 1,
    UIP_NEWDATA = 2,
    UIP_REXMIT  = 4,
    UIP_POLL    = 8,
    UIP_CLOSE   = 16,
    UIP_ABORT   = 32,
    UIP_CONNECTED = 64,
    UIP_TIMEDOUT = 128;

struct uip_tcpip_hdr {

  uint8_t vhl, tos,
    len[2],
    ipid[2],
    ipoffset[2],
    ttl,
    proto;
  uint16_t ipchksum;
  uint16_t srcipaddr[2],
    destipaddr[2];
  uint16_t srcport, destport;
  uint8_t seqno[4], ackno[4], tcpoffset, flags, wnd[2];
  uint16_t tcpchksum;
  uint8_t urgp[2];
  uint8_t optdata[4];
};

struct uip_udpip_hdr
{
    uint8_t vhl, tos, len[2], ipid[2], ipoffset[2], ttl, proto;
    uint16_t ipchksum;
    uint16_t srcipaddr[2], destipaddr[2];
    uint16_t srcport, destport, udplen, udpchksum;
};

#define UIP_IPH_LEN    20
#define UIP_UDPH_LEN    8
#define UIP_TCPH_LEN   20
#define UIP_IPUDPH_LEN (UIP_UDPH_LEN + UIP_IPH_LEN)
#define UIP_IPTCPH_LEN (UIP_TCPH_LEN + UIP_IPH_LEN)
#define UIP_TCPIP_HLEN UIP_IPTCPH_LEN

#if UIP_FIXEDADDR
extern const uip_ipaddr_t uip_hostaddr, uip_netmask, uip_draddr;
#else
extern uip_ipaddr_t uip_hostaddr, uip_netmask, uip_draddr;
#endif

struct uip_eth_addr { uint8_t addr[6]; };
uint16_t uip_ipchksum();
uint16_t uip_tcpchksum();
uint16_t uip_udpchksum();
#endif


