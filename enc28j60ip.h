#ifndef _UIP_H_
#define _UIP_H_
#include "enc28j60hw.h"
#include "eth.h"

typedef void * uip_udp_appstate_t;

struct uip_udp_conn
{
    uint16_t ripaddr[2];   /**< The IP address of the remote peer. */
    uint16_t lport;        /**< The local port number in network byte order. */
    uint16_t rport;        /**< The remote port number in network byte order. */
    uint8_t  ttl;          /**< Default time-to-live. */
    uip_udp_appstate_t appstate;
};

typedef void * uip_tcp_appstate_t;
static constexpr uint8_t UIP_ARPHDRSIZE = 42;
#define UIP_BUFSIZE 98
typedef uint16_t uip_ipaddr_t[2];
void uip_setipid(uint16_t id);
extern uint8_t uip_buf[UIP_BUFSIZE+2];
void uip_listen(uint16_t port);
struct uip_conn *uip_connect(uip_ipaddr_t *ripaddr, uint16_t port);
void uip_send(const void *data, int len);
struct uip_udp_conn *uip_udp_new(uip_ipaddr_t *ripaddr, uint16_t rport);
uint16_t htons(uint16_t val);
extern void *uip_appdata;
extern uint16_t uip_len;

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
extern struct uip_udp_conn *uip_udp_conn;
extern struct uip_udp_conn uip_udp_conns[UIP_UDP_CONNS];
extern uint8_t uip_flags;

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
    UIP_TIMEDOUT = 128,
    UIPETHERNET_FREEPACKET = 1,
    UIPETHERNET_SENDPACKET = 2,
    UIPETHERNET_BUFFERREAD = 4,
    UIP_CLIENT_CONNECTED = 0x10,
    UIP_CLIENT_CLOSE = 0x20,
    UIP_CLIENT_REMOTECLOSED = 0x40,
    UIP_CLIENT_RESTART = 0x80;

struct uip_tcpip_hdr
{
    uint8_t vhl, tos, len[2], ipid[2], ipoffset[2], ttl, proto;
    uint16_t ipchksum;
    uint16_t srcipaddr[2], destipaddr[2];
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

extern uip_ipaddr_t uip_hostaddr, uip_netmask, uip_draddr;
struct uip_eth_addr { uint8_t addr[6]; };

typedef struct
{
    memaddress out_pos;
    memhandle packet_next;
    memhandle packet_in;
    memhandle packet_out;
    bool send;
}
uip_udp_userdata_t;

struct uip_userdata_t
{
    uint8_t state;
    memhandle packets_in[UIP_SOCKET_NUMPACKETS];
    memhandle packets_out[UIP_SOCKET_NUMPACKETS];
    memaddress out_pos;
    uint32_t timer;
};

class Enc28J60IP : public Ethernet
{
private:
    void _uip_init();
    Enc28J60Network _nw;
    uint32_t periodic_timer;
    uint16_t chksum(uint16_t sum, const uint8_t* data, uint16_t len);
    void uipclient_appcall();
    void uipudp_appcall();
public:
    static uip_userdata_t all_data[UIP_CONNS];
    uip_userdata_t *_allocateData();
    size_t _write(uip_userdata_t *u, const uint8_t *buf, size_t size);
    void _send(uip_udp_userdata_t *data);
    Enc28J60Network *nw() { return &_nw; }
    void uip_process(uint8_t flag);
    void process(uint8_t flags) { uip_process(flags); }
    void init(const uint8_t* mac);
    void configure(uint32_t ip, uint32_t dns, uint32_t gw, uint32_t subnet);
    uint16_t ipchksum();
    uint16_t upper_layer_chksum(uint8_t proto);
    memhandle in_packet;
    uint8_t uip_hdrlen;
    bool network_send();
    memhandle uip_packet;
    uint8_t packetstate;
    void _flushBlocks(memhandle *blocks);
    uint32_t _dnsServerAddress;
    void tick();
    static Enc28J60IP *instance;
    Enc28J60IP() { instance = this; }
    uint32_t localIP() { return (uint32_t)uip_hostaddr[0] | (uint32_t)uip_hostaddr[1] << 16; }
    uint32_t subnetMask() { return (uint32_t)uip_netmask[0] | (uint32_t)uip_netmask[1] << 16; }
    uint32_t gatewayIP() { return (uint32_t)uip_draddr[0] | (uint32_t)uip_draddr[1] << 16; }
    uint32_t dnsServerIP() { return _dnsServerAddress; }
    void tick2();
    uint8_t _currentBlock(memhandle *block);
    void _eatBlock(memhandle *blocks);
};

static constexpr uint8_t
    UIP_CLIENT_STATEFLAGS = UIP_CLIENT_CONNECTED | UIP_CLIENT_CLOSE |
        UIP_CLIENT_REMOTECLOSED | UIP_CLIENT_RESTART;

static const uint8_t UIP_CLIENT_SOCKETS = (uint8_t)(~UIP_CLIENT_STATEFLAGS);

struct uip_userdata_closed_t
{
    uint8_t state;
    memhandle packets_in[UIP_SOCKET_NUMPACKETS];
    uint16_t lport;        /**< The local TCP port, in network byte order. */
};

#define uip_ipaddr_cmp(addr1, addr2) (((uint16_t *)addr1)[0] == ((uint16_t *)addr2)[0] && \
				      ((uint16_t *)addr1)[1] == ((uint16_t *)addr2)[1])

typedef uint8_t uip_socket_ptr;
#define UIP_IPH_LEN    20
#define UIP_UDPH_LEN    8
#define UIP_TCPH_LEN   20
#define UIP_LLH_LEN     14
#define UIP_TIME_WAIT_TIMEOUT 120
#define HTONS(n) (uint16_t)((((uint16_t) (n)) << 8) | (((uint16_t) (n)) >> 8))
#define ntohs htons
#define UIP_TCP_MSS UIP_CONF_TCP_MSS
#define UIP_IPTCPH_LEN (UIP_TCPH_LEN + UIP_IPH_LEN)
#define UIP_TCPIP_HLEN UIP_IPTCPH_LEN
#define UIP_IPUDPH_LEN (UIP_UDPH_LEN + UIP_IPH_LEN)
#define uip_udp_remove(conn) (conn)->lport = 0
#define uip_udp_bind(conn, port) (conn)->lport = port
#define UIP_BROADCAST UIP_CONF_BROADCAST
#define UIP_RECEIVE_WINDOW UIP_CONF_RECEIVE_WINDOW
static constexpr uint8_t UIP_LISTENPORTS = 4;
#define UIP_LITTLE_ENDIAN  3412
#define UIP_BIG_ENDIAN     1234
#endif




