#ifndef _ENC28J60NETWORK_H_
#define _ENC28J60NETWORK_H_
#include "types.h"

typedef uint16_t memaddress;
typedef uint8_t memhandle;

struct memblock
{
    memaddress begin;
    memaddress size;
    memhandle nextblock;
};

#define RXSTART_INIT     0x0
#define RXSTOP_INIT      (0x1FFF-0x1800)
#define TXSTART_INIT     (RXSTOP_INIT+1)
#define TXSTOP_INIT      0x1FFF
#define UIP_SOCKET_NUMPACKETS    5
#define UIP_CONF_MAX_CONNECTIONS 4
#define UIP_CONF_UDP             1
#define UIP_CONF_BROADCAST       1
#define UIP_CONF_UDP_CONNS       4
#define UIP_ATTEMPTS_ON_WRITE    -1
#define UIP_CONNECT_TIMEOUT      -1
#define UIP_PERIODIC_TIMER       250
#define UIP_CLIENT_TIMER         10
#define UIP_CONF_MAX_LISTENPORTS 4
#define UIP_CONF_TCP_MSS 512
#define UIP_CONF_RECEIVE_WINDOW 512
#define UIP_CONF_LOGGING         0
#define UIP_CONF_UDP_CHECKSUMS 1
#define UIP_ARCH_CHKSUM 1
#define UIP_PINGADDRCONF 0
#define UIP_UDP UIP_CONF_UDP
#define UIP_UDP_CHECKSUMS UIP_CONF_UDP_CHECKSUMS
#define UIP_UDP_CONNS UIP_CONF_UDP_CONNS
#define UIP_ACTIVE_OPEN 1
#define UIP_CONNS UIP_CONF_MAX_CONNECTIONS
#define NUM_TCP_MEMBLOCKS (UIP_SOCKET_NUMPACKETS*2)*UIP_CONNS
#define NUM_UDP_MEMBLOCKS 3*UIP_UDP_CONNS
#define MEMPOOL_NUM_MEMBLOCKS (NUM_TCP_MEMBLOCKS+NUM_UDP_MEMBLOCKS)
#define MEMPOOL_STARTADDRESS TXSTART_INIT+1
#define MEMPOOL_SIZE TXSTOP_INIT-TXSTART_INIT

// ENC28J60 Control Registers
// Control register definitions are a combination of address,
// bank number, and Ethernet/MAC/PHY indicator bits.
// - Register address        (bits 0-4)
// - Bank number        (bits 5-6)
// - MAC/PHY indicator        (bit 7)
static constexpr uint8_t
    ADDR_MASK = 0x1F,
    BANK_MASK = 0x60,
    SPRD_MASK = 0x80,
    EIE   = 0x1B,
    EIR   = 0x1C,
    ESTAT = 0x1D,
    ECON2 = 0x1E,
    ECON1 = 0x1F,
    ERDPTL   = 0x00|0x00,
    ERDPTH   = 0x01|0x00,
    EWRPTL   = 0x02|0x00,
    EWRPTH   = 0x03|0x00,
    ETXSTL   = 0x04|0x00,
    ETXSTH   = 0x05|0x00,
    ETXNDL   = 0x06|0x00,
    ETXNDH   = 0x07|0x00,
    ERXSTL   = 0x08|0x00,
    ERXSTH   = 0x09|0x00,
    ERXNDL   = 0x0A|0x00,
    ERXNDH   = 0x0B|0x00,
    ERXRDPTL = 0x0C|0x00,
    ERXRDPTH = 0x0D|0x00,
    ERXWRPTL = 0x0E|0x00,
    ERXWRPTH = 0x0F|0x00,
    EDMASTL  = 0x10|0x00,
    EDMASTH  = 0x11|0x00,
    EDMANDL  = 0x12|0x00,
    EDMANDH  = 0x13|0x00,
    EDMADSTL = 0x14|0x00,
    EDMADSTH = 0x15|0x00,
    EDMACSL  = 0x16|0x00,
    EDMACSH  = 0x17|0x00,
    EHT0    = 0x00|0x20,
    EHT1    = 0x01|0x20,
    EHT2    = 0x02|0x20,
    EHT3    = 0x03|0x20,
    EHT4    = 0x04|0x20,
    EHT5    = 0x05|0x20,
    EHT6    = 0x06|0x20,
    EHT7    = 0x07|0x20,
    EPMM0   = 0x08|0x20,
    EPMM1   = 0x09|0x20,
    EPMM2   = 0x0A|0x20,
    EPMM3   = 0x0B|0x20,
    EPMM4   = 0x0C|0x20,
    EPMM5   = 0x0D|0x20,
    EPMM6   = 0x0E|0x20,
    EPMM7   = 0x0F|0x20,
    EPMCSL  = 0x10|0x20,
    EPMCSH  = 0x11|0x20,
    EPMOL   = 0x14|0x20,
    EPMOH   = 0x15|0x20,
    EWOLIE  = 0x16|0x20,
    EWOLIR  = 0x17|0x20,
    ERXFCON = 0x18|0x20,
    EPKTCNT = 0x19|0x20,
    MACON1   = 0x00|0x40|0x80,
    MACON2   = 0x01|0x40|0x80,
    MACON3   = 0x02|0x40|0x80,
    MACON4   = 0x03|0x40|0x80,
    MABBIPG  = 0x04|0x40|0x80,
    MAIPGL   = 0x06|0x40|0x80,
    MAIPGH   = 0x07|0x40|0x80,
    MACLCON1 = 0x08|0x40|0x80,
    MACLCON2 = 0x09|0x40|0x80,
    MAMXFLL  = 0x0A|0x40|0x80,
    MAMXFLH  = 0x0B|0x40|0x80,
    MAPHSUP  = 0x0D|0x40|0x80,
    MICON    = 0x11|0x40|0x80,
    MICMD    = 0x12|0x40|0x80,
    MIREGADR = 0x14|0x40|0x80,
    MIWRL    = 0x16|0x40|0x80,
    MIWRH    = 0x17|0x40|0x80,
    MIRDL    = 0x18|0x40|0x80,
    MIRDH    = 0x19|0x40|0x80,
    MAADR1  = 0x00|0x60|0x80,
    MAADR0  = 0x01|0x60|0x80,
    MAADR3  = 0x02|0x60|0x80,
    MAADR2  = 0x03|0x60|0x80,
    MAADR5  = 0x04|0x60|0x80,
    MAADR4  = 0x05|0x60|0x80,
    EBSTSD  = 0x06|0x60,
    EBSTCON = 0x07|0x60,
    EBSTCSL = 0x08|0x60,
    EBSTCSH = 0x09|0x60,
    MISTAT  = 0x0A|0x60|0x80,
    EREVID  = 0x12|0x60,
    ECOCON  = 0x15|0x60,
    EFLOCON = 0x17|0x60,
    EPAUSL  = 0x18|0x60,
    EPAUSH  = 0x19|0x60,
    PHCON1  = 0x00,
    PHSTAT1 = 0x01,
    PHHID1  = 0x02,
    PHHID2  = 0x03,
    PHCON2  = 0x10,
    PHSTAT2 = 0x11,
    PHIE    = 0x12,
    PHIR    = 0x13,
    PHLCON  = 0x14,
    ERXFCON_UCEN  = 0x80,
    ERXFCON_ANDOR = 0x40,
    ERXFCON_CRCEN = 0x20,
    ERXFCON_PMEN  = 0x10,
    ERXFCON_MPEN  = 0x08,
    ERXFCON_HTEN  = 0x04,
    ERXFCON_MCEN  = 0x02,
    ERXFCON_BCEN  = 0x01,
    EIE_INTIE  = 0x80,
    EIE_PKTIE  = 0x40,
    EIE_DMAIE  = 0x20,
    EIE_LINKIE = 0x10,
    EIE_TXIE   = 0x08,
    EIE_WOLIE  = 0x04,
    EIE_TXERIE = 0x02,
    EIE_RXERIE = 0x01,
    EIR_PKTIF  = 0x40,
    EIR_DMAIF  = 0x20,
    EIR_LINKIF = 0x10,
    EIR_TXIF   = 0x08,
    EIR_WOLIF  = 0x04,
    EIR_TXERIF = 0x02,
    EIR_RXERIF = 0x01,
    ESTAT_INT       = 0x80,
    ESTAT_LATECOL   = 0x10,
    ESTAT_RXBUSY    = 0x04,
    ESTAT_TXABRT    = 0x02,
    ESTAT_CLKRDY    = 0x01,
    ECON2_AUTOINC  = 0x80,
    ECON2_PKTDEC   = 0x40,
    ECON2_PWRSV    = 0x20,
    ECON2_VRPS     = 0x08,
    ECON1_TXRST    = 0x80,
    ECON1_RXRST    = 0x40,
    ECON1_DMAST    = 0x20,
    ECON1_CSUMEN   = 0x10,
    ECON1_TXRTS    = 0x08,
    ECON1_RXEN     = 0x04,
    ECON1_BSEL1    = 0x02,
    ECON1_BSEL0    = 0x01,
    MACON1_LOOPBK  = 0x10,
    MACON1_TXPAUS  = 0x08,
    MACON1_RXPAUS  = 0x04,
    MACON1_PASSALL = 0x02,
    MACON1_MARXEN  = 0x01,
    MACON2_MARST   = 0x80,
    MACON2_RNDRST  = 0x40,
    MACON2_MARXRST = 0x08,
    MACON2_RFUNRST = 0x04,
    MACON2_MATXRST = 0x02,
    MACON2_TFUNRST = 0x01,
    MACON3_PADCFG2 = 0x80,
    MACON3_PADCFG1 = 0x40,
    MACON3_PADCFG0 = 0x20,
    MACON3_TXCRCEN = 0x10,
    MACON3_PHDRLEN = 0x08,
    MACON3_HFRMLEN = 0x04,
    MACON3_FRMLNEN = 0x02,
    MACON3_FULDPX  = 0x01,
    MICMD_MIISCAN  = 0x02,
    MICMD_MIIRD    = 0x01,
    MISTAT_NVALID  = 0x04,
    MISTAT_SCAN    = 0x02,
    MISTAT_BUSY    = 0x01,
    PKTCTRL_PHUGEEN = 0x08,
    PKTCTRL_PPADEN = 0x04,
    PKTCTRL_PCRCEN = 0x02,
    PKTCTRL_POVERRIDE = 0x01,
    ENC28J60_READ_CTRL_REG = 0x00,
    ENC28J60_READ_BUF_MEM = 0x3A,
    ENC28J60_WRITE_CTRL_REG = 0x40,
    ENC28J60_WRITE_BUF_MEM = 0x7A,
    ENC28J60_BIT_FIELD_SET = 0x80,
    ENC28J60_BIT_FIELD_CLR = 0xA0,
    ENC28J60_SOFT_RESET = 0xFF;

static constexpr uint16_t
    PHCON1_PRST     = 0x8000,
    PHCON1_PLOOPBK  = 0x4000,
    PHCON1_PPWRSV   = 0x0800,
    PHCON1_PDPXMD   = 0x0100,
    PHSTAT1_PFDPX   = 0x1000,
    PHSTAT1_PHDPX   = 0x0800,
    PHSTAT1_LLSTAT  = 0x0004,
    PHSTAT1_JBSTAT  = 0x0002,
    PHCON2_FRCLINK  = 0x4000,
    PHCON2_TXDIS    = 0x2000,
    PHCON2_JABBER   = 0x0400,
    PHCON2_HDLDIS   = 0x0100;

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

class Enc28J60Network : public MemoryPool
{
public:
    uint16_t nextPacketPtr;
    uint8_t bank = 0xff;
    struct memblock receivePkt;
    uint8_t readOp(uint8_t op, uint8_t address);
    void writeOp(uint8_t op, uint8_t address, uint8_t data);
    uint16_t setReadPtr(memhandle handle, memaddress position, uint16_t len);
    void setERXRDPT();
    void readBuffer(uint16_t len, uint8_t* data);
    void writeBuffer(uint16_t len, uint8_t* data);
    uint8_t readByte(uint16_t addr);
    void writeByte(uint16_t addr, uint8_t data);
    void setBank(uint8_t address);
    uint8_t readReg(uint8_t address);
    void writeReg(uint8_t address, uint8_t data);
    void writeRegPair(uint8_t address, uint16_t data);
    void phyWrite(uint8_t address, uint16_t data);
    uint16_t phyRead(uint8_t address);
    void clkout(uint8_t clk) { writeReg(ECOCON, clk & 0x7); }
public:
    void move_cb(memaddress dest, memaddress src, memaddress len);
    static Enc28J60Network *instance;
    Enc28J60Network();
    uint8_t getrev() { return(readReg(EREVID)); }
    void powerOn();
    void powerOff();
    bool linkStatus() { return (phyRead(PHSTAT2) & 0x0400) > 0; }
    void init(uint8_t* macaddr);
    memhandle receivePacket();
    void freePacket();
    memaddress blockSize(memhandle handle);
    void sendPacket(memhandle handle);
    uint16_t readPacket(memhandle handle, memaddress position, uint8_t* buffer, uint16_t len);
    uint16_t writePacket(memhandle handle, memaddress position, uint8_t* buffer, uint16_t len);

    void copyPacket(memhandle dest, memaddress dest_pos, memhandle src,
        memaddress src_pos, uint16_t len);

    uint16_t chksum(uint16_t sum, memhandle handle, memaddress pos, uint16_t len);
};

static const uint8_t POOLSTART = 0, NOBLOCK = 0;
#endif



