#include "enc28j60.h"
#include "board.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>

Enc28J60Network *Enc28J60Network::instance;

Enc28J60Network::Enc28J60Network()
{
    instance = this;
}

void Enc28J60Network::init(uint8_t* macaddr)
{
    MemoryPool::init();
    *p_cs_ddr |= 1<<cs_pin;
    *p_cs_port |= 1<<cs_pin;
    *p_mosi_ddr |= 1<<mosi_bit;
    *p_sck_ddr |= 1<<sck_bit;
    *p_miso_ddr &= ~(1<<miso_bit);
    *p_mosi_port &= ~(1<<mosi_bit);
    *p_sck_port &= ~(1<<sck_bit);
    *p_spcr = 1<<spe | 1<<mstr;
    *p_spsr |= 1<<spi2x;
    writeOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
    _delay_ms(50);
    nextPacketPtr = RXSTART_INIT;
    writeRegPair(ERXSTL, RXSTART_INIT);
    writeRegPair(ERXRDPTL, RXSTART_INIT);
    writeRegPair(ERXNDL, RXSTOP_INIT);
    writeReg(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN|ERXFCON_BCEN);
    writeRegPair(EPMM0, 0x303f);
    writeRegPair(EPMCSL, 0xf7f9);
    writeRegPair(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
    writeOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
    writeRegPair(MAIPGL, 0x0C12);
    writeReg(MABBIPG, 0x12);
    writeRegPair(MAMXFLL, MAX_FRAMELEN);
    writeReg(MAADR5, macaddr[0]);
    writeReg(MAADR4, macaddr[1]);
    writeReg(MAADR3, macaddr[2]);
    writeReg(MAADR2, macaddr[3]);
    writeReg(MAADR1, macaddr[4]);
    writeReg(MAADR0, macaddr[5]);
    phyWrite(PHCON2, PHCON2_HDLDIS);
    setBank(ECON1);
    writeOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
    phyWrite(PHLCON,0x476);
}

memhandle Enc28J60Network::receivePacket()
{
    uint8_t rxstat;
    uint16_t len;

    if (readReg(EPKTCNT) != 0)
    {
        uint16_t readPtr = nextPacketPtr+6 > RXSTOP_INIT ?
            nextPacketPtr+6-RXSTOP_INIT+RXSTART_INIT : nextPacketPtr+6;

        writeRegPair(ERDPTL, nextPacketPtr);
        nextPacketPtr = readOp(ENC28J60_READ_BUF_MEM, 0);
        nextPacketPtr |= readOp(ENC28J60_READ_BUF_MEM, 0) << 8;
        len = readOp(ENC28J60_READ_BUF_MEM, 0);
        len |= readOp(ENC28J60_READ_BUF_MEM, 0) << 8;
      len -= 4; //remove the CRC count
      // read the receive status (see datasheet page 43)
      rxstat = readOp(ENC28J60_READ_BUF_MEM, 0);
      //rxstat |= readOp(ENC28J60_READ_BUF_MEM, 0) << 8;
      writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
      // check CRC and symbol errors (see datasheet page 44, table 7-3):
      // The ERXFCON.CRCEN is set by default. Normally we should not
      // need to check this.
      if ((rxstat & 0x80) != 0)
        {
          receivePkt.begin = readPtr;
          receivePkt.size = len;
          return UIP_RECEIVEBUFFERHANDLE;
        }
      // Move the RX read pointer to the start of the next received packet
      // This frees the memory we just read out
      setERXRDPT();
    }
  return (NOBLOCK);
}

void Enc28J60Network::setERXRDPT()
{
    writeRegPair(ERXRDPTL, nextPacketPtr == RXSTART_INIT ? RXSTOP_INIT : nextPacketPtr-1);
}

memaddress Enc28J60Network::blockSize(memhandle handle)
{
    return handle == NOBLOCK ? 0 : handle == UIP_RECEIVEBUFFERHANDLE ?
            receivePkt.size : blocks[handle].size;
}

void Enc28J60Network::sendPacket(memhandle handle)
{
    memblock *packet = &blocks[handle];
    uint16_t start = packet->begin-1;
    uint16_t end = start + packet->size;

    // backup data at control-byte position
    uint8_t data = readByte(start);

    // write control-byte (if not 0 anyway)
    if (data)
        writeByte(start, 0);

    // TX start
    writeRegPair(ETXSTL, start);
    // Set the TXND pointer to correspond to the packet size given
    writeRegPair(ETXNDL, end);
    // send the contents of the transmit buffer onto the network
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);

    // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
    if( (readReg(EIR) & EIR_TXERIF) )
    {
        writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
    }

    //restore data on control-byte position
    if (data)
        writeByte(start, data);
}

uint16_t Enc28J60Network::setReadPtr(memhandle handle, memaddress position, uint16_t len)
{
    memblock *packet = handle == UIP_RECEIVEBUFFERHANDLE ? &receivePkt : &blocks[handle];

    memaddress start = handle == UIP_RECEIVEBUFFERHANDLE &&
        packet->begin + position > RXSTOP_INIT ?
        packet->begin + position-RXSTOP_INIT+RXSTART_INIT : packet->begin + position;

    writeRegPair(ERDPTL, start);
  
    if (len > packet->size - position)
        len = packet->size - position;

    return len;
}

uint16_t
Enc28J60Network::readPacket(memhandle handle, memaddress position, uint8_t* buffer, uint16_t len)
{
    len = setReadPtr(handle, position, len);
    readBuffer(len, buffer);
    return len;
}

uint16_t
Enc28J60Network::writePacket(memhandle handle, memaddress position, uint8_t* buffer, uint16_t len)
{
    memblock *packet = &blocks[handle];
    uint16_t start = packet->begin + position;
    writeRegPair(EWRPTL, start);

    if (len > packet->size - position)
        len = packet->size - position;

    writeBuffer(len, buffer);
    return len;
}

uint8_t Enc28J60Network::readByte(uint16_t addr)
{
    writeRegPair(ERDPTL, addr);
    *p_cs_port &= ~(1<<cs_pin);
    *p_spdr = ENC28J60_READ_BUF_MEM;

    while ((*p_spsr & 1<<spif) == 0)
        ;

    *p_spdr = 0x00;

    while ((*p_spsr & 1<<spif) == 0)
        ;
    
    *p_cs_port |= 1<<cs_pin;
    return *p_spdr;
}

void Enc28J60Network::writeByte(uint16_t addr, uint8_t data)
{
    writeRegPair(EWRPTL, addr);
    *p_cs_port &= ~(1<<cs_pin);
    *p_spdr = ENC28J60_WRITE_BUF_MEM;
    
    while ((*p_spsr & 1<<spif) == 0)
        ;

    *p_spdr = data;

    while ((*p_spsr & 1<<spif) == 0)
        ;

    *p_cs_port |= 1<<cs_pin;
}

void
Enc28J60Network::copyPacket(memhandle dest_pkt, memaddress dest_pos,
    memhandle src_pkt, memaddress src_pos, uint16_t len)
{
  memblock *dest = &blocks[dest_pkt];
  memblock *src = src_pkt == UIP_RECEIVEBUFFERHANDLE ? &receivePkt : &blocks[src_pkt];
  memaddress start = src_pkt == UIP_RECEIVEBUFFERHANDLE && src->begin + src_pos > RXSTOP_INIT ? src->begin + src_pos-RXSTOP_INIT+RXSTART_INIT : src->begin + src_pos;

  //enc28J60_mempool_block_move_callback(dest->begin+dest_pos,start,len);
    move_cb(dest->begin + dest_pos, start, len);
  // Move the RX read pointer to the start of the next received packet
  // This frees the memory we just read out
  setERXRDPT();
}

#if 0
void enc28J60_mempool_block_move_callback(memaddress dest, memaddress src, memaddress len)
{
    Enc28J60Network::instance->move_cb(dest, src, len);
}
#endif

void Enc28J60Network::move_cb(memaddress dest, memaddress src, memaddress len)
{
    //as ENC28J60 DMA is unable to copy single bytes:
    if (len == 1)
    {
        writeByte(dest, readByte(src));
    }
    else
    {
        // calculate address of last byte
        len += src - 1;

        /*  1. Appropriately program the EDMAST, EDMAND
        and EDMADST register pairs. The EDMAST
        registers should point to the first byte to copy
        from, the EDMAND registers should point to the
        last byte to copy and the EDMADST registers
        should point to the first byte in the destination
        range. The destination range will always be
        linear, never wrapping at any values except from
        8191 to 0 (the 8-Kbyte memory boundary).
        Extreme care should be taken when
        programming the start and end pointers to
        prevent a never ending DMA operation which
        would overwrite the entire 8-Kbyte buffer.
        */
        writeRegPair(EDMASTL, src);
        writeRegPair(EDMADSTL, dest);

        if ((src <= RXSTOP_INIT)&& (len > RXSTOP_INIT))
            len -= (RXSTOP_INIT-RXSTART_INIT);

        writeRegPair(EDMANDL, len);

        /*
        2. If an interrupt at the end of the copy process is
        desired, set EIE.DMAIE and EIE.INTIE and
        clear EIR.DMAIF.

        3. Verify that ECON1.CSUMEN is clear. */
        writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_CSUMEN);

        /* 4. Start the DMA copy by setting ECON1.DMAST. */
        writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_DMAST);

        // wait until runnig DMA is completed
        while (readOp(ENC28J60_READ_CTRL_REG, ECON1) & ECON1_DMAST);
    }
}

void Enc28J60Network::freePacket()
{
    setERXRDPT();
}

uint8_t Enc28J60Network::readOp(uint8_t op, uint8_t address)
{
    *p_cs_port &= ~(1<<cs_pin);
    *p_spdr = op | (address & ADDR_MASK);

    while((*p_spsr & 1<<spif) == 0)
        ;

    *p_spdr = 0x00;

    while ((*p_spsr & 1<<spif) == 0)
        ;

    if (address & 0x80)
    {
        *p_spdr = 0x00;

        while ((*p_spsr & 1<<spif) == 0)
            ;
    }

    *p_cs_port |= 1<<cs_pin;
    return *p_spdr;
}

void Enc28J60Network::writeOp(uint8_t op, uint8_t address, uint8_t data)
{
    *p_cs_port &= ~(1<<cs_pin);
    *p_spdr = op | (address & ADDR_MASK);

    while ((*p_spsr & 1<<spif) == 0)
        ;

    *p_spdr = data;

    while ((*p_spsr & 1<<spif) == 0)
        ;

    *p_cs_port |= 1<<cs_pin;
}

void Enc28J60Network::readBuffer(uint16_t len, uint8_t* data)
{
    *p_cs_port &= ~(1<<cs_pin);
    *p_spdr = ENC28J60_READ_BUF_MEM;

    while ((*p_spsr & 1<<spif) == 0)
        ;

    while(len)
    {
        len--;
        *p_spdr = 0x00;

        while ((*p_spsr & 1<<spif) == 0)
            ;

        *data = *p_spdr;
        data++;
    }

    *p_cs_port |= 1<<cs_pin;
}

void Enc28J60Network::writeBuffer(uint16_t len, uint8_t* data)
{
    *p_cs_port &= ~(1<<cs_pin);
    *p_spdr = ENC28J60_WRITE_BUF_MEM;

    while ((*p_spsr & 1<<spif) == 0)
        ;

    while (len)
    {
        len--;
        *p_spdr = *data;
        data++;

        while ((*p_spsr & 1<<spif) == 0)
            ;

    }

    *p_cs_port |= 1<<cs_pin;
}

void Enc28J60Network::setBank(uint8_t address)
{
  // set the bank (if needed)
  if((address & BANK_MASK) != bank)
  {
    // set the bank
    writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
    bank = (address & BANK_MASK);
  }
}

uint8_t Enc28J60Network::readReg(uint8_t address)
{
    setBank(address);
    return readOp(ENC28J60_READ_CTRL_REG, address);
}

void Enc28J60Network::writeReg(uint8_t address, uint8_t data)
{
    setBank(address);
    writeOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

void Enc28J60Network::writeRegPair(uint8_t address, uint16_t data)
{
    setBank(address);
    writeOp(ENC28J60_WRITE_CTRL_REG, address, (data&0xFF));
    writeOp(ENC28J60_WRITE_CTRL_REG, address+1, (data) >> 8);
}

void Enc28J60Network::phyWrite(uint8_t address, uint16_t data)
{
    // set the PHY register address
    writeReg(MIREGADR, address);
    // write the PHY data
    writeRegPair(MIWRL, data);
    // wait until the PHY write completes
    while(readReg(MISTAT) & MISTAT_BUSY)
    {
        _delay_us(15);
    }
}

uint16_t Enc28J60Network::phyRead(uint8_t address)
{
  writeReg(MIREGADR,address);
  writeReg(MICMD, MICMD_MIIRD);
  // wait until the PHY read completes
  while(readReg(MISTAT) & MISTAT_BUSY){
    _delay_us(15);
  }  //and MIRDH
  writeReg(MICMD, 0);
  return (readReg(MIRDL) | readReg(MIRDH) << 8);
}

uint16_t Enc28J60Network::chksum(uint16_t sum, memhandle handle, memaddress pos, uint16_t len)
{
    uint16_t t;
    len = setReadPtr(handle, pos, len)-1;
    *p_cs_port &= ~(1<<cs_pin);
    *p_spdr = ENC28J60_READ_BUF_MEM;

    while ((*p_spsr & 1<<spif) == 0)
        ;

    uint16_t i;

    for (i = 0; i < len; i+=2)
    {
        *p_spdr = 0x00;

        while ((*p_spsr & 1<<spif) == 0)
            ;

        t = *p_spdr << 8;
        *p_spdr = 0x00;

        while ((*p_spsr & 1<<spif) == 0)
            ;

        t += *p_spdr;
        sum += t;

        if (sum < t)
            sum++;
    }

    if (i == len)
    {
        *p_spdr = 0x00;

        while ((*p_spsr & 1<<spif) == 0)
            ;

        t = (*p_spdr << 8) + 0;
        sum += t;

        if (sum < t)
            sum++;
    }
    *p_cs_port |= 1<<cs_pin;
    return sum;
}

void Enc28J60Network::powerOff()
{
    writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_RXEN);
    _delay_ms(50);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_VRPS);
    _delay_ms(50);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PWRSV);
}

void Enc28J60Network::powerOn()
{
    writeOp(ENC28J60_BIT_FIELD_CLR, ECON2, ECON2_PWRSV);
    _delay_ms(50);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
    _delay_ms(50);
}





