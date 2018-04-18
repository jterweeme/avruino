#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_
#include "types.h"

#define RXSTART_INIT     0x0
#define RXSTOP_INIT      (0x1FFF-0x1800)
#define TXSTART_INIT     (RXSTOP_INIT+1)
#define TXSTOP_INIT      0x1FFF


static const uint8_t POOLSTART = 0, NOBLOCK = 0;

typedef uint16_t memaddress;
typedef uint8_t memhandle;

struct memblock
{
    memaddress begin;
    memaddress size;
    memhandle nextblock;
};

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


#if UIP_SOCKET_NUMPACKETS and UIP_CONNS
#define NUM_TCP_MEMBLOCKS (UIP_SOCKET_NUMPACKETS*2)*UIP_CONNS
#else
#define NUM_TCP_MEMBLOCKS 0
#endif

#if UIP_UDP and UIP_UDP_CONNS
#define NUM_UDP_MEMBLOCKS 3*UIP_UDP_CONNS
#else
#define NUM_UDP_MEMBLOCKS 0
#endif

#define MEMPOOL_NUM_MEMBLOCKS (NUM_TCP_MEMBLOCKS+NUM_UDP_MEMBLOCKS)

#define MEMPOOL_STARTADDRESS TXSTART_INIT+1
#define MEMPOOL_SIZE TXSTOP_INIT-TXSTART_INIT

class MemoryPool
{
protected:
    static struct memblock blocks[MEMPOOL_NUM_MEMBLOCKS+1];
public:
    void init();
    memhandle allocBlock(memaddress);
    void freeBlock(memhandle);
    void resizeBlock(memhandle handle, memaddress position);
    void resizeBlock(memhandle handle, memaddress position, memaddress size);
    memaddress blockSize(memhandle);
};
#endif



