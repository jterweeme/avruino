#ifndef _UIPCLIENT_H_
#define _UIPCLIENT_H_
#include "uip.h"
#include "enc28j60.h"

#define UIP_SOCKET_DATALEN UIP_TCP_MSS
#ifndef UIP_SOCKET_NUMPACKETS
#define UIP_SOCKET_NUMPACKETS 5
#endif

static constexpr uint8_t
    UIP_CLIENT_CONNECTED = 0x10,
    UIP_CLIENT_CLOSE = 0x20,
    UIP_CLIENT_REMOTECLOSED = 0x40,
    UIP_CLIENT_RESTART = 0x80;

static constexpr uint8_t
    UIP_CLIENT_STATEFLAGS = UIP_CLIENT_CONNECTED | UIP_CLIENT_CLOSE |
        UIP_CLIENT_REMOTECLOSED | UIP_CLIENT_RESTART;

static const uint8_t UIP_CLIENT_SOCKETS = (uint8_t)(~UIP_CLIENT_STATEFLAGS);

typedef uint8_t uip_socket_ptr;

struct uip_userdata_closed_t
{
    uint8_t state;
    memhandle packets_in[UIP_SOCKET_NUMPACKETS];
    uint16_t lport;        /**< The local TCP port, in network byte order. */
};

typedef struct
{
    uint8_t state;
    memhandle packets_in[UIP_SOCKET_NUMPACKETS];
    memhandle packets_out[UIP_SOCKET_NUMPACKETS];
    memaddress out_pos;
    unsigned long timer;
} uip_userdata_t;

class UIPClient
{
private:
    UIPEthernetClass * const _eth;
public:
    UIPClient(UIPEthernetClass * const eth);
    int connect(IPAddrezz ip, uint16_t port);
    int connect(const char *host, uint16_t port);
    int read(uint8_t *buf, size_t size);
    void stop();
    uint8_t connected();
    operator bool();

    virtual bool operator==(const UIPClient& rhs)
    { return data && rhs.data && (data == rhs.data); }

    virtual bool operator!=(const UIPClient& rhs) { return !this->operator==(rhs); };
    size_t write(uint8_t c) { return _write(data, &c, 1); }
    size_t write(const uint8_t *buf, size_t size) { return _write(data, buf, size); }
    size_t write(const char *buf) { return write((const uint8_t *)buf, strlen(buf)); }
    int available();
    int read();
    int peek();
    void flush();
    static uip_userdata_t all_data[UIP_CONNS];
    UIPClient(struct uip_conn *_conn);
    UIPClient(UIPEthernetClass * const eth, uip_userdata_t* conn_data);
    static size_t _write(uip_userdata_t *, const uint8_t *buf, size_t size);
    static uip_userdata_t* _allocateData();
    static void _eatBlock(memhandle* blocks);
    static void _flushBlocks(memhandle* blocks);
private:
    uip_userdata_t *data;
    int _available(uip_userdata_t *);
    static uint8_t _currentBlock(memhandle* blocks);
};
#endif



