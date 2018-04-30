/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _W5100_H_
#define _W5100_H_
#include "types.h"

static constexpr uint8_t MAX_SOCK_NUM = 4;

typedef uint8_t SOCKET;

class SnMR {
public:
  static const uint8_t CLOSE  = 0x00;
  static const uint8_t TCP    = 0x01;
  static const uint8_t UDP    = 0x02;
  static const uint8_t IPRAW  = 0x03;
  static const uint8_t MACRAW = 0x04;
  static const uint8_t PPPOE  = 0x05;
  static const uint8_t ND     = 0x20;
  static const uint8_t MULTI  = 0x80;
};

enum SockCMD {
  Sock_OPEN      = 0x01,
  Sock_LISTEN    = 0x02,
  Sock_CONNECT   = 0x04,
  Sock_DISCON    = 0x08,
  Sock_CLOSE     = 0x10,
  Sock_SEND      = 0x20,
  Sock_SEND_MAC  = 0x21,
  Sock_SEND_KEEP = 0x22,
  Sock_RECV      = 0x40
};

class SnIR {
public:
  static const uint8_t SEND_OK = 0x10;
  static const uint8_t TIMEOUT = 0x08;
  static const uint8_t RECV    = 0x04;
  static const uint8_t DISCON  = 0x02;
  static const uint8_t CON     = 0x01;
};

class SnSR
{
public:
    static const uint8_t CLOSED      = 0x00;
    static const uint8_t INIT        = 0x13;
    static const uint8_t LISTEN      = 0x14;
    static const uint8_t SYNSENT     = 0x15;
    static const uint8_t SYNRECV     = 0x16;
    static const uint8_t ESTABLISHED = 0x17;
    static const uint8_t FIN_WAIT    = 0x18;
    static const uint8_t CLOSING     = 0x1A;
    static const uint8_t TIME_WAIT   = 0x1B;
    static const uint8_t CLOSE_WAIT  = 0x1C;
    static const uint8_t LAST_ACK    = 0x1D;
    static const uint8_t UDP         = 0x22;
    static const uint8_t IPRAW       = 0x32;
    static const uint8_t MACRAW      = 0x42;
    static const uint8_t PPPOE       = 0x5F;
};

class W5100Class
{
private:
    uint8_t write(uint16_t _addr, uint8_t _data);
    uint16_t write(uint16_t addr, const uint8_t *buf, uint16_t len);
    uint8_t read(uint16_t addr);
    uint16_t read(uint16_t addr, uint8_t *buf, uint16_t len);
    static constexpr uint8_t RST = 7; // Reset BIT
    static constexpr int SOCKETS = 4;
    static constexpr uint16_t SMASK = 0x07FF; // Tx buffer MASK
    static constexpr uint16_t RMASK = 0x07FF; // Rx buffer MASK
    static const uint16_t RSIZE = 2048; // Max Rx buffer size
    uint16_t SBASE[SOCKETS]; // Tx buffer base address
    uint16_t RBASE[SOCKETS]; // Rx buffer base address
    inline uint8_t readSn(SOCKET s, uint16_t addr);
    inline uint8_t writeSn(SOCKET s, uint16_t addr, uint8_t data);

    inline void writeSn16(SOCKET s, uint16_t addr, uint16_t data)
    {
        writeSn(s, addr, data >> 8);
        writeSn(s, addr + 1, data & 0xff);
    }

    inline uint16_t readSn16(SOCKET s, uint16_t addr)
    {
        uint16_t res1 = readSn(s, addr);
        uint16_t res2 = readSn(s, addr + 1);
        return (res1 << 8) | (res2 & 0xff);
    }

    inline uint16_t readSn(SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len);
    inline uint16_t writeSn(SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len);
    static const uint16_t CH_BASE = 0x0400;
    static const uint16_t CH_SIZE = 0x0100;
public:
    void init();
    void read_data(SOCKET s, volatile uint8_t * src, volatile uint8_t * dst, uint16_t len);
    void send_data_processing(SOCKET s, const uint8_t *data, uint16_t len);
    void send_data_processing_offset(SOCKET s, uint16_t offs, const uint8_t *data, uint16_t len);
    void recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek = 0);
    inline void setGatewayIp(uint8_t *addr) { writeGAR(addr); }
    inline void setGatewayIp(uint32_t addr) { writeGAR((uint8_t *)&addr); }
    inline void getGatewayIp(uint8_t *addr) { readGAR(addr); }
    inline void setSubnetMask(uint8_t *addr) { writeSUBR(addr); }
    inline void setSubnetMask(uint32_t addr) { writeSUBR((uint8_t *)&addr); }
    inline void getSubnetMask(uint8_t *addr) { readSUBR(addr); }
    inline void setMACAddress(uint8_t *addr) { writeSHAR(addr); }
    inline void getMACAddress(uint8_t *addr) { readSHAR(addr); }
    inline void setIPAddress(uint32_t addr) { writeSIPR((uint8_t *)&addr); }
    inline void getIPAddress(uint8_t *addr) { readSIPR(addr); }
    void execCmdSn(SOCKET s, SockCMD _cmd);
    uint16_t getTXFreeSize(SOCKET s);
    uint16_t getRXReceivedSize(SOCKET s);
    inline void writeMR(uint8_t data) { write(0x0000, data); }
    inline uint8_t readMR() { return read(0x0000); }
    inline uint16_t writeGAR(uint8_t *buf) { return write(0x0001, buf, 4); }
    inline uint16_t readGAR(uint8_t *buf) { return read(0x0001, buf, 4); }
    inline uint16_t writeSUBR(uint8_t *buf) { return write(0x0005, buf, 4); }
    inline uint16_t readSUBR(uint8_t *buf) { return read(0x0005, buf, 4); }
    inline uint16_t writeSHAR(uint8_t *buf) { return write(0x0009, buf, 6); }
    inline uint16_t readSHAR(uint8_t *buf) { return read(0x0009, buf, 6); }
    inline uint16_t writeSIPR(uint8_t *buf) { return write(0x000f, buf, 4); }
    inline uint16_t readSIPR(uint8_t *buf) { return read(0x000f, buf, 4); }
    uint16_t writeUIPR(uint8_t *buf) { return write(0x002a, buf, 4); }
    uint16_t readUIPR(uint8_t *buf) { return read(0x002a, buf, 4); }
    inline void writeIR(uint8_t data) { write(0x0015, data); }
    inline uint8_t readIR() { return read(0x0015); }
    inline void writeIMR(uint8_t data) { write(0x0016, data); }
    inline uint8_t readIMR() { return read(0x0016); }
    inline void writeRMSR(uint8_t data) { write(0x001a, data); }
    inline void writeTMSR(uint8_t data) { write(0x001b, data); }
    inline void writeSnMR(SOCKET s, uint8_t data) { writeSn(s, 0x0000, data); }
    inline uint8_t readSnMR(SOCKET s) { return readSn(s, 0x0000); }
    inline void writeSnCR(SOCKET s, uint8_t data) { writeSn(s, 0x0001, data); }
    inline uint8_t readSnCR(SOCKET s) { return readSn(s, 0x0001); }
    inline void writeSnIR(SOCKET s, uint8_t data) { writeSn(s, 0x0002, data); }
    inline uint8_t readSnIR(SOCKET s) { return readSn(s, 0x0002); }
    inline void writeSnSR(SOCKET s, uint8_t data) { writeSn(s, 0x0003, data); }
    inline uint8_t readSnSR(SOCKET s) { return readSn(s, 0x0003); }
    void writeSnPORT(SOCKET s, uint16_t data) { writeSn16(s, 0x0004, data); }
    uint16_t readSnPORT(SOCKET _s) { return readSn16(_s, 0x0004); }
    uint16_t writeSnDHAR(SOCKET s, uint8_t *buf) { return writeSn(s, 0x0006, buf, 6); }
    uint16_t writeSnDIPR(SOCKET s, uint8_t *buf) { return writeSn(s, 0x000c, buf, 4); }
    void writeSnDPORT(SOCKET s, uint16_t data) { writeSn16(s, 0x0010, data); }
    uint16_t readSnDPORT(SOCKET s) { return readSn16(s, 0x0010); }
    void writeSnTX_FSR(SOCKET s, uint16_t data) { writeSn16(s, 0x0020, data); }
    uint16_t readSnTX_FSR(SOCKET s) { return readSn16(s, 0x0020); }
    void writeSnTX_RD(SOCKET s, uint16_t data) { writeSn16(s, 0x0022, data); }
    uint16_t readSnTX_RD(SOCKET s) { return readSn16(s, 0x0022); }
    void writeSnTX_WR(SOCKET s, uint16_t data) { writeSn16(s, 0x0024, data); }
    uint16_t readSnTX_WR(SOCKET s) { return readSn16(s, 0x0024); }
    void writeSnRX_RSR(SOCKET s, uint16_t data) { writeSn16(s, 0x0026, data); }
    uint16_t readSnRX_RSR(SOCKET s) { return readSn16(s, 0x0026); }
    void writeSnRX_RD(SOCKET s, uint16_t data) { writeSn16(s, 0x0028, data); }
    uint16_t readSnRX_RD(SOCKET s) { return readSn16(s, 0x0028); }
    void writeSnRX_WR(SOCKET s, uint16_t data) { writeSn16(s, 0x002a, data); }
    uint16_t readSnRX_WR(SOCKET s) { return readSn16(s, 0x002a); }
    static const uint16_t SSIZE = 2048; // Max Tx buffer size
};

uint8_t W5100Class::readSn(SOCKET _s, uint16_t _addr) {
    return read(CH_BASE + _s * CH_SIZE + _addr);
}

uint8_t W5100Class::writeSn(SOCKET _s, uint16_t _addr, uint8_t _data) {
  return write(CH_BASE + _s * CH_SIZE + _addr, _data);
}

uint16_t W5100Class::readSn(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
  return read(CH_BASE + _s * CH_SIZE + _addr, _buf, _len);
}

uint16_t W5100Class::writeSn(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
  return write(CH_BASE + _s * CH_SIZE + _addr, _buf, _len);
}
#endif



