#ifndef __UIP_H__
#define __UIP_H__

#ifndef UIP_LITTLE_ENDIAN
#define UIP_LITTLE_ENDIAN  3412
#endif /* UIP_LITTLE_ENDIAN */
#ifndef UIP_BIG_ENDIAN
#define UIP_BIG_ENDIAN     1234
#endif /* UIP_BIG_ENDIAN */

#include <inttypes.h>

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

#define UIP_APPCALL uipclient_appcall

typedef void* uip_udp_appstate_t;

void uipudp_appcall(void);

#define UIP_UDP_APPCALL uipudp_appcall

#define CC_REGISTER_ARG register

#define UIP_ARCH_CHKSUM 1


#ifdef UIP_CONF_PINGADDRCONF
#define UIP_PINGADDRCONF UIP_CONF_PINGADDRCONF
#else /* UIP_CONF_PINGADDRCONF */
#define UIP_PINGADDRCONF 0
#endif /* UIP_CONF_PINGADDRCONF */

#define UIP_UDP UIP_CONF_UDP

#ifdef UIP_CONF_UDP_CHECKSUMS
#define UIP_UDP_CHECKSUMS UIP_CONF_UDP_CHECKSUMS
#else
#define UIP_UDP_CHECKSUMS 0
#endif

#ifdef UIP_CONF_UDP_CONNS
#define UIP_UDP_CONNS UIP_CONF_UDP_CONNS
#else /* UIP_CONF_UDP_CONNS */
#define UIP_UDP_CONNS    10
#endif /* UIP_CONF_UDP_CONNS */

#define UIP_ACTIVE_OPEN 1

#ifndef UIP_CONF_MAX_CONNECTIONS
#define UIP_CONNS       10
#else /* UIP_CONF_MAX_CONNECTIONS */
#define UIP_CONNS UIP_CONF_MAX_CONNECTIONS
#endif /* UIP_CONF_MAX_CONNECTIONS */

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
#else /* UIP_CONF_LLH_LEN */
#define UIP_LLH_LEN     14
#endif /* UIP_CONF_LLH_LEN */


#ifdef UIP_CONF_BYTE_ORDER
#define UIP_BYTE_ORDER     UIP_CONF_BYTE_ORDER
#else /* UIP_CONF_BYTE_ORDER */
#define UIP_BYTE_ORDER     UIP_LITTLE_ENDIAN
#endif /* UIP_CONF_BYTE_ORDER */

#define UIP_URGDATA      0
#define UIP_FIXEDADDR    0

typedef uint16_t uip_ip4addr_t[2];
typedef uint16_t uip_ip6addr_t[8];
typedef uip_ip4addr_t uip_ipaddr_t;

#define uip_sethostaddr(addr) uip_ipaddr_copy(uip_hostaddr, (addr))
#define uip_gethostaddr(addr) uip_ipaddr_copy((addr), uip_hostaddr)
#define uip_setdraddr(addr) uip_ipaddr_copy(uip_draddr, (addr))
#define uip_setnetmask(addr) uip_ipaddr_copy(uip_netmask, (addr))
#define uip_getdraddr(addr) uip_ipaddr_copy((addr), uip_draddr)
#define uip_getnetmask(addr) uip_ipaddr_copy((addr), uip_netmask)

void uip_init(void);
void uip_setipid(uint16_t id);


#define uip_input()        uip_process(UIP_DATA)


#define uip_periodic(conn) do { uip_conn = &uip_conns[conn]; \
                                uip_process(UIP_TIMER); } while (0)


#define uip_conn_active(conn) (uip_conns[conn].tcpstateflags != UIP_CLOSED)


#define uip_periodic_conn(conn) do { uip_conn = conn; \
                                     uip_process(UIP_TIMER); } while (0)

#define uip_poll_conn(conn) do { uip_conn = conn; \
                                 uip_process(UIP_POLL_REQUEST); } while (0)


#define uip_udp_periodic(conn) do { uip_udp_conn = &uip_udp_conns[conn]; \
                                uip_process(UIP_UDP_TIMER); } while (0)

#define uip_udp_periodic_conn(conn) do { uip_udp_conn = conn; \
                                         uip_process(UIP_UDP_TIMER); } while (0)


extern uint8_t uip_buf[UIP_BUFSIZE+2];
void uip_listen(uint16_t port);
void uip_unlisten(uint16_t port);
struct uip_conn *uip_connect(uip_ipaddr_t *ripaddr, uint16_t port);


#define uip_outstanding(conn) ((conn)->len)


void uip_send(const void *data, int len);


#define uip_datalen()       uip_len
#define uip_urgdatalen()    uip_urglen
#define uip_close()         (uip_flags = UIP_CLOSE)
#define uip_abort()         (uip_flags = UIP_ABORT)
#define uip_stop()          (uip_conn->tcpstateflags |= UIP_STOPPED)
#define uip_stopped(conn)   ((conn)->tcpstateflags & UIP_STOPPED)

#define uip_restart()         do { uip_flags |= UIP_NEWDATA; \
                                   uip_conn->tcpstateflags &= ~UIP_STOPPED; \
                              } while(0)


#define uip_udpconnection() (uip_conn == NULL)
#define uip_newdata()   (uip_flags & UIP_NEWDATA)
#define uip_acked()   (uip_flags & UIP_ACKDATA)
#define uip_connected() (uip_flags & UIP_CONNECTED)
#define uip_closed()    (uip_flags & UIP_CLOSE)
#define uip_aborted()    (uip_flags & UIP_ABORT)

/**
 * Has the connection timed out?
 *
 * Non-zero if the current connection has been aborted due to too many
 * retransmissions.
 *
 * \hideinitializer
 */
#define uip_timedout()    (uip_flags & UIP_TIMEDOUT)

/**
 * Do we need to retransmit previously data?
 *
 * Reduces to non-zero if the previously sent data has been lost in
 * the network, and the application should retransmit it. The
 * application should send the exact same data as it did the last
 * time, using the uip_send() function.
 *
 * \hideinitializer
 */
#define uip_rexmit()     (uip_flags & UIP_REXMIT)
#define uip_poll()       (uip_flags & UIP_POLL)
#define uip_initialmss()             (uip_conn->initialmss)
#define uip_mss()             (uip_conn->mss)

struct uip_udp_conn *uip_udp_new(uip_ipaddr_t *ripaddr, uint16_t rport);


#define uip_udp_remove(conn) (conn)->lport = 0
#define uip_udp_bind(conn, port) (conn)->lport = port
#define uip_udp_send(len) uip_send((char *)uip_appdata, len)

#define uip_ipaddr(addr, addr0,addr1,addr2,addr3) do { \
                     ((uint16_t *)(addr))[0] = HTONS(((addr0) << 8) | (addr1)); \
                     ((uint16_t *)(addr))[1] = HTONS(((addr2) << 8) | (addr3)); \
                  } while(0)

/**
 * Construct an IPv6 address from eight 16-bit words.
 *
 * This function constructs an IPv6 address.
 *
 * \hideinitializer
 */
#define uip_ip6addr(addr, addr0,addr1,addr2,addr3,addr4,addr5,addr6,addr7) do { \
                     ((uint16_t *)(addr))[0] = HTONS((addr0)); \
                     ((uint16_t *)(addr))[1] = HTONS((addr1)); \
                     ((uint16_t *)(addr))[2] = HTONS((addr2)); \
                     ((uint16_t *)(addr))[3] = HTONS((addr3)); \
                     ((uint16_t *)(addr))[4] = HTONS((addr4)); \
                     ((uint16_t *)(addr))[5] = HTONS((addr5)); \
                     ((uint16_t *)(addr))[6] = HTONS((addr6)); \
                     ((uint16_t *)(addr))[7] = HTONS((addr7)); \
                  } while(0)

/**
 * Copy an IP address to another IP address.
 *
 * Copies an IP address from one place to another.
 *
 * Example:
 \code
 uip_ipaddr_t ipaddr1, ipaddr2;

 uip_ipaddr(&ipaddr1, 192,16,1,2);
 uip_ipaddr_copy(&ipaddr2, &ipaddr1);
 \endcode
 *
 * \param dest The destination for the copy.
 * \param src The source from where to copy.
 *
 * \hideinitializer
 */
#if !UIP_CONF_IPV6
#define uip_ipaddr_copy(dest, src) do { \
                     ((uint16_t *)dest)[0] = ((uint16_t *)src)[0]; \
                     ((uint16_t *)dest)[1] = ((uint16_t *)src)[1]; \
                  } while(0)
#else /* !UIP_CONF_IPV6 */
#define uip_ipaddr_copy(dest, src) memcpy(dest, src, sizeof(uip_ip6addr_t))
#endif /* !UIP_CONF_IPV6 */

#if !UIP_CONF_IPV6
#define uip_ipaddr_cmp(addr1, addr2) (((uint16_t *)addr1)[0] == ((uint16_t *)addr2)[0] && \
				      ((uint16_t *)addr1)[1] == ((uint16_t *)addr2)[1])
#else /* !UIP_CONF_IPV6 */
#define uip_ipaddr_cmp(addr1, addr2) (memcmp(addr1, addr2, sizeof(uip_ip6addr_t)) == 0)
#endif /* !UIP_CONF_IPV6 */

/**
 * Compare two IP addresses with netmasks
 *
 * Compares two IP addresses with netmasks. The masks are used to mask
 * out the bits that are to be compared.
 *
 * Example:
 \code
 uip_ipaddr_t ipaddr1, ipaddr2, mask;

 uip_ipaddr(&mask, 255,255,255,0);
 uip_ipaddr(&ipaddr1, 192,16,1,2);
 uip_ipaddr(&ipaddr2, 192,16,1,3);
 if(uip_ipaddr_maskcmp(&ipaddr1, &ipaddr2, &mask)) {
    printf("They are the same");
 }
 \endcode
 *
 * \param addr1 The first IP address.
 * \param addr2 The second IP address.
 * \param mask The netmask.
 *
 * \hideinitializer
 */
#define uip_ipaddr_maskcmp(addr1, addr2, mask) \
                          (((((uint16_t *)addr1)[0] & ((uint16_t *)mask)[0]) == \
                            (((uint16_t *)addr2)[0] & ((uint16_t *)mask)[0])) && \
                           ((((uint16_t *)addr1)[1] & ((uint16_t *)mask)[1]) == \
                            (((uint16_t *)addr2)[1] & ((uint16_t *)mask)[1])))


/**
 * Mask out the network part of an IP address.
 *
 * Masks out the network part of an IP address, given the address and
 * the netmask.
 *
 * Example:
 \code
 uip_ipaddr_t ipaddr1, ipaddr2, netmask;

 uip_ipaddr(&ipaddr1, 192,16,1,2);
 uip_ipaddr(&netmask, 255,255,255,0);
 uip_ipaddr_mask(&ipaddr2, &ipaddr1, &netmask);
 \endcode
 *
 * In the example above, the variable "ipaddr2" will contain the IP
 * address 192.168.1.0.
 *
 * \param dest Where the result is to be placed.
 * \param src The IP address.
 * \param mask The netmask.
 *
 * \hideinitializer
 */
#define uip_ipaddr_mask(dest, src, mask) do { \
                     ((uint16_t *)dest)[0] = ((uint16_t *)src)[0] & ((uint16_t *)mask)[0]; \
                     ((uint16_t *)dest)[1] = ((uint16_t *)src)[1] & ((uint16_t *)mask)[1]; \
                  } while(0)


#define uip_ipaddr1(addr) (htons(((uint16_t *)(addr))[0]) >> 8)
#define uip_ipaddr2(addr) (htons(((uint16_t *)(addr))[0]) & 0xff)
#define uip_ipaddr3(addr) (htons(((uint16_t *)(addr))[1]) >> 8)
#define uip_ipaddr4(addr) (htons(((uint16_t *)(addr))[1]) & 0xff)


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
#endif /* htons */
#ifndef ntohs
#define ntohs htons
#endif

/** @} */

/**
 * Pointer to the application data in the packet buffer.
 *
 * This pointer points to the application data when the application is
 * called. If the application wishes to send data, the application may
 * use this space to write the data into before calling uip_send().
 */
extern void *uip_appdata;

#if UIP_URGDATA > 0

extern void *uip_urgdata;
#endif /* UIP_URGDATA > 0 */



extern uint16_t uip_len;

/** @} */

#if UIP_URGDATA > 0
extern uint16_t uip_urglen, uip_surglen;
#endif /* UIP_URGDATA > 0 */

struct uip_conn {
  uip_ipaddr_t ripaddr;   /**< The IP address of the remote host. */
  
  uint16_t lport;        /**< The local TCP port, in network byte order. */
  uint16_t rport;
  
  uint8_t rcv_nxt[4];    /**< The sequence number that we expect to
			 receive next. */
  uint8_t snd_nxt[4];    /**< The sequence number that was last sent by
                         us. */
    uint16_t len;          /**< Length of the data that was previously sent. */
    uint16_t mss;          /**< Current maximum segment size for the connection. */
    uint16_t initialmss;   /**< Initial maximum segment size for the connection. */
    uint8_t sa;            /**< Retransmission time-out calculation state variable. */
    uint8_t sv;            /**< Retransmission time-out calculation state variable. */
    uint8_t rto;           /**< Retransmission time-out. */
    uint8_t tcpstateflags; /**< TCP state and flags. */
    uint8_t timer;         /**< The retransmission timer. */
    uint8_t nrtx;          /**< The number of retransmissions for the last
			 segment sent. */

  /** The application state. */
  uip_tcp_appstate_t appstate;
};


/**
 * Pointer to the current TCP connection.
 *
 * The uip_conn pointer can be used to access the current TCP
 * connection.
 */
extern struct uip_conn *uip_conn;
/* The array containing all uIP connections. */
extern struct uip_conn uip_conns[UIP_CONNS];
/**
 * \addtogroup uiparch
 * @{
 */

/**
 * 4-byte array used for the 32-bit sequence number calculations.
 */
extern uint8_t uip_acc32[4];

/** @} */


#if UIP_UDP
/**
 * Representation of a uIP UDP connection.
 */
struct uip_udp_conn {
  uip_ipaddr_t ripaddr;   /**< The IP address of the remote peer. */
  uint16_t lport;        /**< The local port number in network byte order. */
  uint16_t rport;        /**< The remote port number in network byte order. */
  uint8_t  ttl;          /**< Default time-to-live. */

  uip_udp_appstate_t appstate;
};

/**
 * The current UDP connection.
 */
extern struct uip_udp_conn *uip_udp_conn;
extern struct uip_udp_conn uip_udp_conns[UIP_UDP_CONNS];
#endif /* UIP_UDP */

/**
 * The structure holding the TCP/IP statistics that are gathered if
 * UIP_STATISTICS is set to 1.
 *
 */
struct uip_stats {
  struct {
    uip_stats_t drop;     /**< Number of dropped packets at the IP layer. */
    uip_stats_t recv;     /**< Number of received packets at the IP layer. */
    uip_stats_t sent;     /**< Number of sent packets at the IP layer. */
    uip_stats_t vhlerr;   
    uip_stats_t hblenerr; /**< Number of packets dropped due to wrong
			     IP length, high byte. */
    uip_stats_t lblenerr; /**< Number of packets dropped due to wrong
			     IP length, low byte. */
    uip_stats_t fragerr;  /**< Number of packets dropped since they
			     were IP fragments. */
    uip_stats_t chkerr;   /**< Number of packets dropped due to IP
			     checksum errors. */
    uip_stats_t protoerr; /**< Number of packets dropped since they
			     were neither ICMP, UDP nor TCP. */
  } ip;                   /**< IP statistics. */
  struct {
    uip_stats_t drop;     /**< Number of dropped ICMP packets. */
    uip_stats_t recv;     /**< Number of received ICMP packets. */
    uip_stats_t sent;     /**< Number of sent ICMP packets. */
    uip_stats_t typeerr;  /**< Number of ICMP packets with a wrong
			     type. */
  } icmp;                 /**< ICMP statistics. */
  struct {
    uip_stats_t drop;     /**< Number of dropped TCP segments. */
    uip_stats_t recv;     /**< Number of recived TCP segments. */
    uip_stats_t sent;     /**< Number of sent TCP segments. */
    uip_stats_t chkerr;   /**< Number of TCP segments with a bad
			     checksum. */
    uip_stats_t ackerr;   /**< Number of TCP segments with a bad ACK
			     number. */
    uip_stats_t rst;      /**< Number of recevied TCP RST (reset) segments. */
    uip_stats_t rexmit;   /**< Number of retransmitted TCP segments. */
    uip_stats_t syndrop;  /**< Number of dropped SYNs due to too few
			     connections was avaliable. */
    uip_stats_t synrst;   /**< Number of SYNs for closed ports,
			     triggering a RST. */
  } tcp;                  /**< TCP statistics. */
#if UIP_UDP
  struct {
    uip_stats_t drop;     /**< Number of dropped UDP segments. */
    uip_stats_t recv;     /**< Number of recived UDP segments. */
    uip_stats_t sent;     /**< Number of sent UDP segments. */
    uip_stats_t chkerr;   /**< Number of UDP segments with a bad
			     checksum. */
  } udp;                  /**< UDP statistics. */
#endif /* UIP_UDP */
};

/**
 * The uIP TCP/IP statistics.
 *
 * This is the variable in which the uIP TCP/IP statistics are gathered.
 */
extern struct uip_stats uip_stat;


extern uint8_t uip_flags;

#define UIP_ACKDATA   1
#define UIP_NEWDATA   2
#define UIP_REXMIT    4
#define UIP_POLL      8
#define UIP_CLOSE     16
#define UIP_ABORT     32
#define UIP_CONNECTED 64
#define UIP_TIMEDOUT  128   /* The connection has been aborted due to
			       too many retransmissions. */

/* uip_process(flag):
 *
 * The actual uIP function which does all the work.
 */
void uip_process(uint8_t flag);


#define UIP_DATA          1
#define UIP_TIMER         2     /* Tells uIP that the periodic timer
				   has fired. */
#define UIP_POLL_REQUEST  3     /* Tells uIP that a connection should
				   be polled. */
#define UIP_UDP_SEND_CONN 4     /* Tells uIP that a UDP datagram
				   should be constructed in the
				   uip_buf buffer. */
#if UIP_UDP
#define UIP_UDP_TIMER     5
#endif /* UIP_UDP */

static const uint8_t UIP_CLOSED = 0,
    UIP_SYN_RCVD = 1,
    UIP_SYN_SENT = 2,
    UIP_ESTABLISHED = 3,
    UIP_FIN_WAIT_1 = 4,
    UIP_FIN_WAIT_2 = 5,
    UIP_CLOSING = 6,
    UIP_TIME_WAIT = 7,
    UIP_LAST_ACK = 8,
    UIP_TS_MASK = 15;
  
#define UIP_STOPPED      16

/* The TCP and IP headers. */
struct uip_tcpip_hdr {
#if UIP_CONF_IPV6
  /* IPv6 header. */
  uint8_t vtc,
    tcflow;
  uint16_t flow;
  uint8_t len[2];
  uint8_t proto, ttl;
  uip_ip6addr_t srcipaddr, destipaddr;
#else /* UIP_CONF_IPV6 */
  /* IPv4 header. */
  uint8_t vhl,
    tos,
    len[2],
    ipid[2],
    ipoffset[2],
    ttl,
    proto;
  uint16_t ipchksum;
  uint16_t srcipaddr[2],
    destipaddr[2];
#endif /* UIP_CONF_IPV6 */
  uint16_t srcport, destport;
  uint8_t seqno[4], ackno[4], tcpoffset, flags, wnd[2];
  uint16_t tcpchksum;
  uint8_t urgp[2];
  uint8_t optdata[4];
};

/* The ICMP and IP headers. */
struct uip_icmpip_hdr {
#if UIP_CONF_IPV6
  /* IPv6 header. */
  uint8_t vtc, tcf;
  uint16_t flow;
  uint8_t len[2];
  uint8_t proto, ttl;
  uip_ip6addr_t srcipaddr, destipaddr;
#else /* UIP_CONF_IPV6 */
  /* IPv4 header. */
  uint8_t vhl,
    tos,
    len[2],
    ipid[2],
    ipoffset[2],
    ttl,
    proto;
  uint16_t ipchksum;
  uint16_t srcipaddr[2],
    destipaddr[2];
#endif /* UIP_CONF_IPV6 */
  
  /* ICMP (echo) header. */
  uint8_t type, icode;
  uint16_t icmpchksum;
#if !UIP_CONF_IPV6
  uint16_t id, seqno;
#else /* !UIP_CONF_IPV6 */
  uint8_t flags, reserved1, reserved2, reserved3;
  uint8_t icmp6data[16];
  uint8_t options[1];
#endif /* !UIP_CONF_IPV6 */
};

struct uip_udpip_hdr
{
    uint8_t vhl, tos, len[2], ipid[2], ipoffset[2], ttl, proto;
    uint16_t ipchksum;
    uint16_t srcipaddr[2], destipaddr[2];
    uint16_t srcport, destport, udplen, udpchksum;
};


#define UIP_APPDATA_SIZE (UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN)


#define UIP_PROTO_ICMP  1
#define UIP_PROTO_TCP   6
#define UIP_PROTO_UDP   17
#define UIP_PROTO_ICMP6 58
#define UIP_IPH_LEN    20    /* Size of IP header */
#define UIP_UDPH_LEN    8    /* Size of UDP header */
#define UIP_TCPH_LEN   20    /* Size of TCP header */
#define UIP_IPUDPH_LEN (UIP_UDPH_LEN + UIP_IPH_LEN)
#define UIP_IPTCPH_LEN (UIP_TCPH_LEN + UIP_IPH_LEN)
#define UIP_TCPIP_HLEN UIP_IPTCPH_LEN


#if UIP_FIXEDADDR
extern const uip_ipaddr_t uip_hostaddr, uip_netmask, uip_draddr;
#else /* UIP_FIXEDADDR */
extern uip_ipaddr_t uip_hostaddr, uip_netmask, uip_draddr;
#endif /* UIP_FIXEDADDR */

struct uip_eth_addr {
    uint8_t addr[6];
};

uint16_t uip_chksum(uint16_t *buf, uint16_t len);
uint16_t uip_ipchksum(void);
uint16_t uip_tcpchksum(void);
uint16_t uip_udpchksum(void);


#endif


