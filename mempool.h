#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_
#include "uip.h"

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
    static void init();
    static memhandle allocBlock(memaddress);
    static void freeBlock(memhandle);
    static void resizeBlock(memhandle handle, memaddress position);
    static void resizeBlock(memhandle handle, memaddress position, memaddress size);
    static memaddress blockSize(memhandle);
};
#endif



