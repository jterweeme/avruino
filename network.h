#ifndef Enc28J60Network_H_
#define Enc28J60Network_H_

#include "util.h"
#include "uip.h"

// ENC28J60 Control Registers
// Control register definitions are a combination of address,
// bank number, and Ethernet/MAC/PHY indicator bits.
// - Register address        (bits 0-4)
// - Bank number        (bits 5-6)
// - MAC/PHY indicator        (bit 7)
#define ADDR_MASK        0x1F
#define BANK_MASK        0x60
#define SPRD_MASK        0x80
// All-bank registers
static const uint8_t EIE   = 0x1B;
static const uint8_t EIR   = 0x1C;
static const uint8_t ESTAT = 0x1D;
static const uint8_t ECON2 = 0x1E;
static const uint8_t ECON1 = 0x1F;
// Bank 0 registers
static const uint8_t ERDPTL   = (0x00|0x00);
static const uint8_t ERDPTH   = (0x01|0x00);
static const uint8_t EWRPTL   = (0x02|0x00);
static const uint8_t EWRPTH   = (0x03|0x00);
static const uint8_t ETXSTL   = (0x04|0x00);
static const uint8_t ETXSTH   = (0x05|0x00);
static const uint8_t ETXNDL   = (0x06|0x00);
static const uint8_t ETXNDH   = (0x07|0x00);
static const uint8_t ERXSTL   = (0x08|0x00);
static const uint8_t ERXSTH   = (0x09|0x00);
static const uint8_t ERXNDL   = (0x0A|0x00);
static const uint8_t ERXNDH   = (0x0B|0x00);
static const uint8_t ERXRDPTL = (0x0C|0x00);
static const uint8_t ERXRDPTH = (0x0D|0x00);
static const uint8_t ERXWRPTL = (0x0E|0x00);
static const uint8_t ERXWRPTH = (0x0F|0x00);
static const uint8_t EDMASTL  = (0x10|0x00);
static const uint8_t EDMASTH  = (0x11|0x00);
static const uint8_t EDMANDL  = (0x12|0x00);
static const uint8_t EDMANDH  = (0x13|0x00);
static const uint8_t EDMADSTL = (0x14|0x00);
static const uint8_t EDMADSTH = (0x15|0x00);
static const uint8_t EDMACSL  = (0x16|0x00);
static const uint8_t EDMACSH  = (0x17|0x00);

static const uint8_t EHT0    = (0x00|0x20);
static const uint8_t EHT1    = (0x01|0x20);
static const uint8_t EHT2    = (0x02|0x20);
static const uint8_t EHT3    = (0x03|0x20);
static const uint8_t EHT4    = (0x04|0x20);
static const uint8_t EHT5    = (0x05|0x20);
static const uint8_t EHT6    = (0x06|0x20);
static const uint8_t EHT7    = (0x07|0x20);
static const uint8_t EPMM0   = (0x08|0x20);
static const uint8_t EPMM1   = (0x09|0x20);
static const uint8_t EPMM2   = (0x0A|0x20);
static const uint8_t EPMM3   = (0x0B|0x20);
static const uint8_t EPMM4   = (0x0C|0x20);
static const uint8_t EPMM5   = (0x0D|0x20);
static const uint8_t EPMM6   = (0x0E|0x20);
static const uint8_t EPMM7   = (0x0F|0x20);
static const uint8_t EPMCSL  = (0x10|0x20);
static const uint8_t EPMCSH  = (0x11|0x20);
static const uint8_t EPMOL   = (0x14|0x20);
static const uint8_t EPMOH   = (0x15|0x20);
static const uint8_t EWOLIE  = (0x16|0x20);
static const uint8_t EWOLIR  = (0x17|0x20);
static const uint8_t ERXFCON = (0x18|0x20);
static const uint8_t EPKTCNT = (0x19|0x20);

#define MACON1           (0x00|0x40|0x80)
#define MACON2           (0x01|0x40|0x80)
#define MACON3           (0x02|0x40|0x80)
#define MACON4           (0x03|0x40|0x80)
#define MABBIPG          (0x04|0x40|0x80)
#define MAIPGL           (0x06|0x40|0x80)
#define MAIPGH           (0x07|0x40|0x80)
#define MACLCON1         (0x08|0x40|0x80)
#define MACLCON2         (0x09|0x40|0x80)
#define MAMXFLL          (0x0A|0x40|0x80)
#define MAMXFLH          (0x0B|0x40|0x80)
#define MAPHSUP          (0x0D|0x40|0x80)
#define MICON            (0x11|0x40|0x80)
#define MICMD            (0x12|0x40|0x80)
#define MIREGADR         (0x14|0x40|0x80)
#define MIWRL            (0x16|0x40|0x80)
#define MIWRH            (0x17|0x40|0x80)
#define MIRDL            (0x18|0x40|0x80)
#define MIRDH            (0x19|0x40|0x80)

#define MAADR1           (0x00|0x60|0x80)
#define MAADR0           (0x01|0x60|0x80)
#define MAADR3           (0x02|0x60|0x80)
#define MAADR2           (0x03|0x60|0x80)
#define MAADR5           (0x04|0x60|0x80)
#define MAADR4           (0x05|0x60|0x80)
#define EBSTSD           (0x06|0x60)
#define EBSTCON          (0x07|0x60)
#define EBSTCSL          (0x08|0x60)
#define EBSTCSH          (0x09|0x60)
#define MISTAT           (0x0A|0x60|0x80)
#define EREVID           (0x12|0x60)
#define ECOCON           (0x15|0x60)
#define EFLOCON          (0x17|0x60)
#define EPAUSL           (0x18|0x60)
#define EPAUSH           (0x19|0x60)
// PHY registers
#define PHCON1           0x00
#define PHSTAT1          0x01
#define PHHID1           0x02
#define PHHID2           0x03
#define PHCON2           0x10
#define PHSTAT2          0x11
#define PHIE             0x12
#define PHIR             0x13
#define PHLCON           0x14

#define ERXFCON_UCEN     0x80
#define ERXFCON_ANDOR    0x40
#define ERXFCON_CRCEN    0x20
#define ERXFCON_PMEN     0x10
#define ERXFCON_MPEN     0x08
#define ERXFCON_HTEN     0x04
#define ERXFCON_MCEN     0x02
#define ERXFCON_BCEN     0x01
// ENC28J60 EIE Register Bit Definitions
#define EIE_INTIE        0x80
#define EIE_PKTIE        0x40
#define EIE_DMAIE        0x20
#define EIE_LINKIE       0x10
#define EIE_TXIE         0x08
#define EIE_WOLIE        0x04
#define EIE_TXERIE       0x02
#define EIE_RXERIE       0x01
// ENC28J60 EIR Register Bit Definitions
#define EIR_PKTIF        0x40
#define EIR_DMAIF        0x20
#define EIR_LINKIF       0x10
#define EIR_TXIF         0x08
#define EIR_WOLIF        0x04
#define EIR_TXERIF       0x02
#define EIR_RXERIF       0x01
// ENC28J60 ESTAT Register Bit Definitions
#define ESTAT_INT        0x80
#define ESTAT_LATECOL    0x10
#define ESTAT_RXBUSY     0x04
#define ESTAT_TXABRT     0x02
#define ESTAT_CLKRDY     0x01
// ENC28J60 ECON2 Register Bit Definitions
#define ECON2_AUTOINC    0x80
#define ECON2_PKTDEC     0x40
#define ECON2_PWRSV      0x20
#define ECON2_VRPS       0x08

#define ECON1_TXRST      0x80
#define ECON1_RXRST      0x40
#define ECON1_DMAST      0x20
#define ECON1_CSUMEN     0x10
#define ECON1_TXRTS      0x08
#define ECON1_RXEN       0x04
#define ECON1_BSEL1      0x02
#define ECON1_BSEL0      0x01
// ENC28J60 MACON1 Register Bit Definitions
#define MACON1_LOOPBK    0x10
#define MACON1_TXPAUS    0x08
#define MACON1_RXPAUS    0x04
#define MACON1_PASSALL   0x02
#define MACON1_MARXEN    0x01
// ENC28J60 MACON2 Register Bit Definitions
#define MACON2_MARST     0x80
#define MACON2_RNDRST    0x40
#define MACON2_MARXRST   0x08
#define MACON2_RFUNRST   0x04
#define MACON2_MATXRST   0x02
#define MACON2_TFUNRST   0x01
// ENC28J60 MACON3 Register Bit Definitions
static const uint8_t MACON3_PADCFG2 =  0x80;
static const uint8_t MACON3_PADCFG1 =  0x40;
static const uint8_t MACON3_PADCFG0 =  0x20;
static const uint8_t MACON3_TXCRCEN =  0x10;
static const uint8_t MACON3_PHDRLEN =  0x08;
static const uint8_t MACON3_HFRMLEN =  0x04;
static const uint8_t MACON3_FRMLNEN =  0x02;
static const uint8_t MACON3_FULDPX  =  0x01;
// ENC28J60 MICMD Register Bit Definitions
#define MICMD_MIISCAN    0x02
#define MICMD_MIIRD      0x01
// ENC28J60 MISTAT Register Bit Definitions
#define MISTAT_NVALID    0x04
#define MISTAT_SCAN      0x02
#define MISTAT_BUSY      0x01

#define PHCON1_PRST      0x8000
#define PHCON1_PLOOPBK   0x4000
#define PHCON1_PPWRSV    0x0800
#define PHCON1_PDPXMD    0x0100
// ENC28J60 PHY PHSTAT1 Register Bit Definitions
#define PHSTAT1_PFDPX    0x1000
#define PHSTAT1_PHDPX    0x0800
#define PHSTAT1_LLSTAT   0x0004
#define PHSTAT1_JBSTAT   0x0002
// ENC28J60 PHY PHCON2 Register Bit Definitions
#define PHCON2_FRCLINK   0x4000
#define PHCON2_TXDIS     0x2000
#define PHCON2_JABBER    0x0400
#define PHCON2_HDLDIS    0x0100

// ENC28J60 Packet Control Byte Bit Definitions
static const uint8_t PKTCTRL_PHUGEEN = 0x08,
PKTCTRL_PPADEN = 0x04,
PKTCTRL_PCRCEN = 0x02,
PKTCTRL_POVERRIDE = 0x01;

// SPI operation codes
static const uint8_t ENC28J60_READ_CTRL_REG = 0x00,
    ENC28J60_READ_BUF_MEM = 0x3A,
    ENC28J60_WRITE_CTRL_REG = 0x40,
    ENC28J60_WRITE_BUF_MEM = 0x7A,
    ENC28J60_BIT_FIELD_SET = 0x80,
    ENC28J60_BIT_FIELD_CLR = 0xA0,
    ENC28J60_SOFT_RESET = 0xFF;

#define RXSTART_INIT     0x0
#define RXSTOP_INIT      (0x1FFF-0x1800)
#define TXSTART_INIT     (RXSTOP_INIT+1)
#define TXSTOP_INIT      0x1FFF
#define        MAX_FRAMELEN        1500   // (note: maximum ethernet frame length would be 1518

#define ENC28J60_CONTROL_CS     SS
#define SPI_MOSI        MOSI
#define SPI_MISO        MISO
#define SPI_SCK         SCK
#define SPI_SS          SS

#define UIP_RECEIVEBUFFERHANDLE 0xff

static const uint8_t POOLSTART = 0, NOBLOCK = 0;





typedef uint16_t memaddress;
typedef uint8_t memhandle;

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

void enc28J60_mempool_block_move_callback(memaddress,memaddress,memaddress);

#define MEMPOOL_MEMBLOCK_MV(dest,src,size) enc28J60_mempool_block_move_callback(dest,src,size)


struct memblock
{
  memaddress begin;
  memaddress size;
  memhandle nextblock;
};

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


class Enc28J60Network : public MemoryPool
{
private:
  static uint16_t nextPacketPtr;
  static uint8_t bank;
  static struct memblock receivePkt;
  static uint8_t readOp(uint8_t op, uint8_t address);
  static void writeOp(uint8_t op, uint8_t address, uint8_t data);
  static uint16_t setReadPtr(memhandle handle, memaddress position, uint16_t len);
  static void setERXRDPT();
  static void readBuffer(uint16_t len, uint8_t* data);
  static void writeBuffer(uint16_t len, uint8_t* data);
  static uint8_t readByte(uint16_t addr);
  static void writeByte(uint16_t addr, uint8_t data);
  static void setBank(uint8_t address);
  static uint8_t readReg(uint8_t address);
  static void writeReg(uint8_t address, uint8_t data);
  static void writeRegPair(uint8_t address, uint16_t data);
  static void phyWrite(uint8_t address, uint16_t data);
  static uint16_t phyRead(uint8_t address);
  static void clkout(uint8_t clk);
  friend void enc28J60_mempool_block_move_callback(memaddress,memaddress,memaddress);
public:
  uint8_t getrev(void);
  void powerOn();
  void powerOff();
  bool linkStatus();
  static void init(uint8_t* macaddr);
  static memhandle receivePacket();
  static void freePacket();
  static memaddress blockSize(memhandle handle);
  static void sendPacket(memhandle handle);
  static uint16_t readPacket(memhandle handle, memaddress position, uint8_t* buffer, uint16_t len);
  static uint16_t writePacket(memhandle handle, memaddress position, uint8_t* buffer, uint16_t len);
  static void copyPacket(memhandle dest, memaddress dest_pos, memhandle src, memaddress src_pos, uint16_t len);
  static uint16_t chksum(uint16_t sum, memhandle handle, memaddress pos, uint16_t len);
};

extern Enc28J60Network Enc28J60;
#endif /* Enc28J60NetworkClass_H_ */



