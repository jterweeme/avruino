/*
adapted from SD Arduino library
credits to SparkFun Electronics
*/

#ifndef _ZD2CARD_H_
#define _ZD2CARD_H_
#include <stdint.h>
#include "board.h"
#include <avr/io.h>

static constexpr uint8_t
    R1_READY_STATE = 0x00,
    R1_IDLE_STATE = 0x01, // status for card in the idle state
    R1_ILLEGAL_COMMAND = 0x04,
    DATA_START_BLOCK = 0xFE,
    STOP_TRAN_TOKEN = 0xFD, // stop token for write multiple blocks
    WRITE_MULTIPLE_TOKEN = 0XFC, // start data token for write multiple blocks
    DATA_RES_MASK = 0X1F,
    DATA_RES_ACCEPTED = 0X05,
    SPI_FULL_SPEED = 0,
    SPI_HALF_SPEED = 1,
    SPI_QUARTER_SPEED = 2,
    SEND_CSD = 9,
    SEND_CID = 10;

typedef struct CID {
  // byte 0
  uint8_t mid;  // Manufacturer ID
  // byte 1-2
  char oid[2];  // OEM/Application ID
  // byte 3-7
  char pnm[5];  // Product name
  // byte 8
  unsigned prv_m : 4;  // Product revision n.m
  unsigned prv_n : 4;
  // byte 9-12
  uint32_t psn;  // Product serial number
  // byte 13
  unsigned mdt_year_high : 4;  // Manufacturing date
  unsigned reserved : 4;
  // byte 14
  unsigned mdt_month : 4;
  unsigned mdt_year_low :4;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
}cid_t;


typedef struct CSDV1 {
  // byte 0
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  // byte 1
  uint8_t taac;
  // byte 2
  uint8_t nsac;
  // byte 3
  uint8_t tran_speed;
  // byte 4
  uint8_t ccc_high;
  // byte 5
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  // byte 6
  unsigned c_size_high : 2;
  unsigned reserved2 : 2;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign :1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  // byte 7
  uint8_t c_size_mid;
  // byte 8
  unsigned vdd_r_curr_max : 3;
  unsigned vdd_r_curr_min : 3;
  unsigned c_size_low :2;
  // byte 9
  unsigned c_size_mult_high : 2;
  unsigned vdd_w_cur_max : 3;
  unsigned vdd_w_curr_min : 3;
  // byte 10
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned c_size_mult_low : 1;
  // byte 11
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  // byte 12
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved3 : 2;
  unsigned wp_grp_enable : 1;
  // byte 13
  unsigned reserved4 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  // byte 14
  unsigned reserved5: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
}csd1_t;

typedef struct CSDV2 {
  // byte 0
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  // byte 1
  uint8_t taac;
  // byte 2
  uint8_t nsac;
  // byte 3
  uint8_t tran_speed;
  // byte 4
  uint8_t ccc_high;
  // byte 5
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  // byte 6
  unsigned reserved2 : 4;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign :1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  // byte 7
  unsigned reserved3 : 2;
  unsigned c_size_high : 6;
  // byte 8
  uint8_t c_size_mid;
  // byte 9
  uint8_t c_size_low;
  // byte 10
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned reserved4 : 1;
  // byte 11
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  // byte 12
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved5 : 2;
  unsigned wp_grp_enable : 1;
  // byte 13
  unsigned reserved6 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  // byte 14
  unsigned reserved7: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
}csd2_t;

union csd_t
{
    csd1_t v1;
    csd2_t v2;
};

class Sd2Card
{
private:
    uint32_t _ticks = 0;
    Pin * const _cs;
    uint32_t block_;
    uint8_t errorCode_ = 0;
    uint8_t inBlock_ = 0;
    uint16_t offset_;
    uint8_t partialBlockRead_ = 0;
    uint8_t status_;
    uint8_t type_ = 0;
    uint8_t cardAcmd(uint8_t cmd, uint32_t arg);
    uint8_t cardCommand(uint8_t cmd, uint32_t arg);
    void error(uint8_t code) {errorCode_ = code;}
    uint8_t readRegister(uint8_t cmd, void* buf);
    uint8_t sendWriteCommand(uint32_t blockNumber, uint32_t eraseCount);
    void chipSelectHigh() { _cs->set(); }
    void chipSelectLow() { _cs->clear(); }
    void type(uint8_t value) {type_ = value;}
    uint8_t waitNotBusy(uint16_t timeoutMillis);
    uint8_t writeData(uint8_t token, const uint8_t* src);
    uint8_t waitStartBlock();
    void partialBlockRead(uint8_t value);
    uint8_t partialBlockRead() const {return partialBlockRead_;}
    uint8_t writeData(const uint8_t* src);
    uint8_t writeStart(uint32_t blockNumber, uint32_t eraseCount);
    uint8_t writeStop();
    uint8_t readCID(cid_t* cid) { return readRegister(SEND_CID, cid); }
    void readEnd();
    uint8_t erase(uint32_t firstBlock, uint32_t lastBlock);
    uint8_t eraseSingleBlockEnable();
    uint8_t errorCode() const {return errorCode_;}
    uint8_t errorData() const {return status_;}
public:
    Sd2Card(Pin *cs) : _cs(cs) { }
    uint32_t cardSize();
    uint8_t init(uint8_t sckRateID);
    uint8_t readData(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst);
    uint8_t readBlock(uint32_t b, uint8_t *dst) { return readData(b, 0, 512, dst); }
    uint8_t setSckRate(uint8_t sckRateID);
    uint8_t type() const {return type_;}
    uint8_t writeBlock(uint32_t blockNumber, const uint8_t* src);
    uint8_t readCSD(csd_t* csd) { return readRegister(SEND_CSD, csd); }
    void tick() { _ticks++; }
};
#endif


