#include "zd2card.h"
#include <avr/io.h>

static uint16_t constexpr
    SD_INIT_TIMEOUT = 2000,
    SD_ERASE_TIMEOUT = 10000,
    SD_READ_TIMEOUT = 300,
    SD_WRITE_TIMEOUT = 600;

static constexpr uint8_t
    SD_CARD_ERROR_CMD0 = 0X1,
    SD_CARD_ERROR_CMD8 = 0X2,
    SD_CARD_ERROR_CMD17 = 0X3,
    SD_CARD_ERROR_CMD24 = 0X4,
    SD_CARD_ERROR_CMD25 = 0X05,
    SD_CARD_ERROR_CMD58 = 0X06,
    SD_CARD_ERROR_ACMD23 = 0X07,
    SD_CARD_ERROR_ACMD41 = 0X08,
    SD_CARD_ERROR_BAD_CSD = 0X09,
    SD_CARD_ERROR_ERASE = 0X0A,
    SD_CARD_ERROR_ERASE_SINGLE_BLOCK = 0X0B,
    SD_CARD_ERROR_ERASE_TIMEOUT = 0X0C,
    SD_CARD_ERROR_READ = 0X0D,
    SD_CARD_ERROR_READ_REG = 0X0E,
    SD_CARD_ERROR_READ_TIMEOUT = 0X0F,
    SD_CARD_ERROR_STOP_TRAN = 0X10,
    SD_CARD_ERROR_WRITE = 0X11,
    SD_CARD_ERROR_WRITE_BLOCK_ZERO = 0X12,
    SD_CARD_ERROR_WRITE_MULTIPLE = 0X13,
    SD_CARD_ERROR_WRITE_PROGRAMMING = 0X14,
    SD_CARD_ERROR_WRITE_TIMEOUT = 0X15,
    SD_CARD_ERROR_SCK_RATE = 0x16,  // incorrect rate selected
    SD_CARD_TYPE_SD1 = 1,
    SD_CARD_TYPE_SD2 = 2,
    SD_CARD_TYPE_SDHC = 3;

static inline void spiSend(uint8_t b)
{
    SPDR = b;
    while (!(SPSR & 1<<SPIF));
}

static inline uint8_t spiRec(void)
{
    spiSend(0XFF);
    return SPDR;
}

uint8_t Sd2Card::cardCommand(uint8_t cmd, uint32_t arg)
{
    readEnd();  // end read if in partialBlockRead mode
    chipSelectLow();  // select card
    waitNotBusy(300); // wait up to 300 ms if busy
    spiSend(cmd | 0x40);  // send command

    for (int8_t s = 24; s >= 0; s -= 8)
        spiSend(arg >> s);

    uint8_t crc = 0XFF;

    if (cmd == 0)
        crc = 0X95;  // correct crc for CMD0 with arg 0

    if (cmd == 8)
        crc = 0X87;  // correct crc for CMD8 with arg 0X1AA

    spiSend(crc);

    // wait for response
    for (uint8_t i = 0; ((status_ = spiRec()) & 0X80) && i != 0XFF; i++)
        ;

    return status_;
}

uint32_t Sd2Card::cardSize()
{
    csd_t csd;

    if (!readCSD(&csd))
        return 0;

    if (csd.v1.csd_ver == 0)
    {
        uint8_t read_bl_len = csd.v1.read_bl_len;

        uint16_t c_size = csd.v1.c_size_high<<10 | csd.v1.c_size_mid<<2 | csd.v1.c_size_low;

        uint8_t c_size_mult = (csd.v1.c_size_mult_high << 1) | csd.v1.c_size_mult_low;
        return (uint32_t)(c_size + 1) << (c_size_mult + read_bl_len - 7);
    }

    if (csd.v2.csd_ver == 1)
    {
        uint32_t c_size = (uint32_t)csd.v2.c_size_high<<16
            | csd.v2.c_size_mid<<8 | csd.v2.c_size_low;

        return (c_size + 1) << 10;
    }

    error(SD_CARD_ERROR_BAD_CSD);
    return 0;
}

uint8_t Sd2Card::erase(uint32_t firstBlock, uint32_t lastBlock)
{
    if (!eraseSingleBlockEnable())
    {
        error(SD_CARD_ERROR_ERASE_SINGLE_BLOCK);
        goto fail;
    }

    if (type_ != SD_CARD_TYPE_SDHC)
    {
        firstBlock <<= 9;
        lastBlock <<= 9;
    }

    if (cardCommand(32, firstBlock) || cardCommand(33, lastBlock) || cardCommand(38, 0))
    {
        error(SD_CARD_ERROR_ERASE);
        goto fail;
    }

    if (!waitNotBusy(SD_ERASE_TIMEOUT))
    {
        error(SD_CARD_ERROR_ERASE_TIMEOUT);
        goto fail;
    }

    chipSelectHigh();
    return true;
fail:
    chipSelectHigh();
    return false;
}

uint8_t Sd2Card::eraseSingleBlockEnable()
{
    csd_t csd;
    return readCSD(&csd) ? csd.v1.erase_blk_en : 0;
}

uint8_t Sd2Card::init(uint8_t sckRateID)
{
    errorCode_ = inBlock_ = partialBlockRead_ = type_ = 0;
    // 16-bit init start time allows over a minute
    uint16_t t0 = (uint16_t)_ticks;
    uint32_t arg;
    _cs->direction(OUTPUT);
    _cs->set();
    *p_ddr_miso &= ~(1<<pmiso);
    *p_ddr_mosi |= 1<<pmosi;
    *p_ddr_sck |= 1<<psck;
    *p_ddr_ss |= 1<<pss;
    *p_port_ss |= 1<<pss;
    // Enable SPI, Master, clock rate f_osc/128
    SPCR = 1<<SPE | 1<<MSTR | 1<<SPR1 | 1<<SPR0;
    // clear double speed
    SPSR &= ~(1<<SPI2X);

    // must supply min of 74 clock cycles with CS high.
    for (uint8_t i = 0; i < 10; i++)
        spiSend(0XFF);

    _cs->clear();

    // command to go idle in SPI mode
    while ((status_ = cardCommand(0, 0)) != R1_IDLE_STATE)
    {
        if (((uint16_t)_ticks - t0) > SD_INIT_TIMEOUT)
        {
            error(SD_CARD_ERROR_CMD0);
            goto fail;
        }
    }
  // check SD version
  if ((cardCommand(8, 0x1AA) & R1_ILLEGAL_COMMAND)) {
    type(SD_CARD_TYPE_SD1);
  } else {
    // only need last byte of r7 response
    for (uint8_t i = 0; i < 4; i++) status_ = spiRec();
    if (status_ != 0XAA) {
      error(SD_CARD_ERROR_CMD8);
      goto fail;
    }
    type(SD_CARD_TYPE_SD2);
    }
    // initialize card and send host supports SDHC if SD2
    arg = type() == SD_CARD_TYPE_SD2 ? 0X40000000 : 0;

    while ((status_ = cardAcmd(41, arg)) != R1_READY_STATE)
    {
        // check for timeout
        if (((uint16_t)_ticks - t0) > SD_INIT_TIMEOUT)
        {
            error(SD_CARD_ERROR_ACMD41);
            goto fail;
        }
    }

    // if SD2 read OCR register to check for SDHC card
    if (type() == SD_CARD_TYPE_SD2)
    {
        if (cardCommand(58, 0))
        {
            error(SD_CARD_ERROR_CMD58);
            goto fail;
        }

        if ((spiRec() & 0XC0) == 0XC0)
            type(SD_CARD_TYPE_SDHC);

        // discard rest of ocr - contains allowed voltage range
        for (uint8_t i = 0; i < 3; i++)
            spiRec();
    }

    chipSelectHigh();
    return setSckRate(sckRateID);
fail:
    chipSelectHigh();
    return false;
}

void Sd2Card::partialBlockRead(uint8_t value)
{
    readEnd();
    partialBlockRead_ = value;
}

uint8_t Sd2Card::readData(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst)
{
  uint16_t n;
  if (count == 0) return true;
  if ((count + offset) > 512) {
    goto fail;
  }
  if (!inBlock_ || block != block_ || offset < offset_) {
    block_ = block;
    // use address if not SDHC card
    if (type()!= SD_CARD_TYPE_SDHC) block <<= 9;
    if (cardCommand(17, block)) {
      error(SD_CARD_ERROR_CMD17);
      goto fail;
    }
    if (!waitStartBlock()) {
      goto fail;
    }
    offset_ = 0;
    inBlock_ = 1;
  }

#if 1
  // start first spi transfer
  SPDR = 0XFF;

  // skip data before offset
  for (;offset_ < offset; offset_++) {
    while (!(SPSR & (1 << SPIF)));
    SPDR = 0XFF;
  }
  // transfer data
  n = count - 1;
  for (uint16_t i = 0; i < n; i++) {
    while (!(SPSR & (1 << SPIF)));
    dst[i] = SPDR;
    SPDR = 0XFF;
  }
  // wait for last byte
  while (!(SPSR & (1 << SPIF)));
  dst[n] = SPDR;
#else
  // skip data before offset
  for (;offset_ < offset; offset_++) {
    spiRec();
  }

    // transfer data
    for (uint16_t i = 0; i < count; i++)
        dst[i] = spiRec();
#endif

    offset_ += count;

    if (!partialBlockRead_ || offset_ >= 512)
    {
        // read rest of data, checksum and set chip select high
        readEnd();
    }
    return true;

fail:
    chipSelectHigh();
    return false;
}
//------------------------------------------------------------------------------
/** Skip remaining data in a block when in partial block read mode. */
void Sd2Card::readEnd(void) {
  if (inBlock_) {
      // skip data and crc
#if 1
    // optimize skip for hardware
    SPDR = 0XFF;
    while (offset_++ < 513) {
      while (!(SPSR & (1 << SPIF)));
      SPDR = 0XFF;
    }
    // wait for last crc byte
    while (!(SPSR & (1 << SPIF)));
#else
    while (offset_++ < 514) spiRec();
#endif
    chipSelectHigh();
    inBlock_ = 0;
  }
}

/** read CID or CSR register */
uint8_t Sd2Card::readRegister(uint8_t cmd, void* buf) {
  uint8_t* dst = reinterpret_cast<uint8_t*>(buf);
  if (cardCommand(cmd, 0)) {
    error(SD_CARD_ERROR_READ_REG);
    goto fail;
  }
  if (!waitStartBlock()) goto fail;
  // transfer data
  for (uint16_t i = 0; i < 16; i++) dst[i] = spiRec();
  spiRec();  // get first crc byte
  spiRec();  // get second crc byte
  chipSelectHigh();
  return true;

 fail:
  chipSelectHigh();
  return false;
}

uint8_t Sd2Card::setSckRate(uint8_t sckRateID)
{
    if (sckRateID > 6)
    {
        error(SD_CARD_ERROR_SCK_RATE);
        return false;
    }
  
    if ((sckRateID & 1) || sckRateID == 6)
        SPSR &= ~(1<<SPI2X);
    else
        SPSR |= 1<<SPI2X;
    
    SPCR &= ~(1<<SPR1 | 1<<SPR0);
    SPCR |= (sckRateID & 4 ? (1 << SPR1) : 0) | (sckRateID & 2 ? (1 << SPR0) : 0);
    return true;
}

// wait for card to go not busy
uint8_t Sd2Card::waitNotBusy(uint16_t timeoutMillis)
{
  uint16_t t0 = _ticks;
  do {
    if (spiRec() == 0XFF) return true;
  }
  while (((uint16_t)_ticks - t0) < timeoutMillis);
  return false;
}

/** Wait for start block token */
uint8_t Sd2Card::waitStartBlock()
{
  uint16_t t0 = _ticks;
  while ((status_ = spiRec()) == 0XFF) {
    if (((uint16_t)_ticks - t0) > SD_READ_TIMEOUT) {
      error(SD_CARD_ERROR_READ_TIMEOUT);
      goto fail;
    }
  }
  if (status_ != DATA_START_BLOCK) {
    error(SD_CARD_ERROR_READ);
    goto fail;
  }
  return true;

 fail:
  chipSelectHigh();
  return false;
}
//------------------------------------------------------------------------------
/**
 * Writes a 512 byte block to an SD card.
 *
 * \param[in] blockNumber Logical block to be written.
 * \param[in] src Pointer to the location of the data to be written.
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 */
uint8_t Sd2Card::writeBlock(uint32_t blockNumber, const uint8_t* src) {
  // use address if not SDHC card
  if (type() != SD_CARD_TYPE_SDHC) blockNumber <<= 9;
  if (cardCommand(24, blockNumber)) {
    error(SD_CARD_ERROR_CMD24);
    goto fail;
  }
  if (!writeData(DATA_START_BLOCK, src)) goto fail;

  // wait for flash programming to complete
  if (!waitNotBusy(SD_WRITE_TIMEOUT)) {
    error(SD_CARD_ERROR_WRITE_TIMEOUT);
    goto fail;
  }
  // response is r2 so get and check two bytes for nonzero
    if (cardCommand(13, 0) || spiRec()) {
        error(SD_CARD_ERROR_WRITE_PROGRAMMING);
        goto fail;
    }
    chipSelectHigh();
    return true;
fail:
    chipSelectHigh();
    return false;
}
//------------------------------------------------------------------------------
/** Write one data block in a multiple block write sequence */
uint8_t Sd2Card::writeData(const uint8_t* src) {
  // wait for previous write to finish
  if (!waitNotBusy(SD_WRITE_TIMEOUT)) {
    error(SD_CARD_ERROR_WRITE_MULTIPLE);
    chipSelectHigh();
    return false;
  }
  return writeData(WRITE_MULTIPLE_TOKEN, src);
}
//------------------------------------------------------------------------------
// send one block of data for write block or write multiple blocks
uint8_t Sd2Card::writeData(uint8_t token, const uint8_t* src)
{
#if 1
    SPDR = token;

    for (uint16_t i = 0; i < 512; i += 2)
    {
        while (!(SPSR & (1 << SPIF)));
        SPDR = src[i];
        while (!(SPSR & (1 << SPIF)));
        SPDR = src[i+1];
    }

    // wait for last data byte
    while (!(SPSR & (1 << SPIF)))
        ;

#else
  spiSend(token);
  for (uint16_t i = 0; i < 512; i++) {
    spiSend(src[i]);
  }
#endif
  spiSend(0xff);  // dummy crc
  spiSend(0xff);  // dummy crc

  status_ = spiRec();
  if ((status_ & DATA_RES_MASK) != DATA_RES_ACCEPTED) {
    error(SD_CARD_ERROR_WRITE);
    chipSelectHigh();
    return false;
  }
  return true;
}

uint8_t Sd2Card::writeStart(uint32_t blockNumber, uint32_t eraseCount)
{
    if (cardAcmd(23, eraseCount))
    {
        error(SD_CARD_ERROR_ACMD23);
        goto fail;
    }

    // use address if not SDHC card
    if (type() != SD_CARD_TYPE_SDHC)
        blockNumber <<= 9;

    if (cardCommand(25, blockNumber))
    {
        error(SD_CARD_ERROR_CMD25);
        goto fail;
    }
    return true;

fail:
    chipSelectHigh();
    return false;
}

uint8_t Sd2Card::writeStop()
{
    if (!waitNotBusy(SD_WRITE_TIMEOUT))
        goto fail;

    spiSend(STOP_TRAN_TOKEN);

    if (!waitNotBusy(SD_WRITE_TIMEOUT))
        goto fail;

    chipSelectHigh();
    return true;
fail:
    error(SD_CARD_ERROR_STOP_TRAN);
    chipSelectHigh();
    return false;
}

uint8_t Sd2Card::cardAcmd(uint8_t cmd, uint32_t arg)
{
    cardCommand(55, 0);
    return cardCommand(cmd, arg);
}



