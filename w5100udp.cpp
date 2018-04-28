/*
 *  Udp.cpp: Library to send/receive UDP packets with the Arduino ethernet shield.
 *  This version only offers minimal wrapping of socket.c/socket.h
 *  Drop Udp.h/.cpp into the Ethernet library directory at hardware/libraries/Ethernet/ 
 *
 * MIT License:
 * Copyright (c) 2008 Bjoern Hartmann
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * bjoern@cs.stanford.edu 12/30/2008
 */

#include "w5100eth.h"
#include "w5100udp.h"
#include "socket.h"

uint8_t EthernetUDP::begin(uint16_t port)
{
    if (_sock != MAX_SOCK_NUM)
        return 0;

    for (int i = 0; i < MAX_SOCK_NUM; i++)
    {
        uint8_t s = _eth->nw()->readSnSR(i);

        if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT)
        {
            _sock = i;
            break;
        }
    }

    if (_sock == MAX_SOCK_NUM)
        return 0;

    _port = port;
    _remaining = 0;
    socket(_sock, SnMR::UDP, _port, 0);
    return 1;
}

void EthernetUDP::stop()
{
    if (_sock == MAX_SOCK_NUM)
        return;

    close(_sock);
    EthernetClass::_server_port[_sock] = 0;
    _sock = MAX_SOCK_NUM;
}

extern W5100Class *g_w5100;

static int startUDP(SOCKET s, uint8_t* addr, uint16_t port)
{
    if (((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
        ((port == 0x00)))
    {
        return 0;
    }

    g_w5100->writeSnDIPR(s, addr);
    g_w5100->writeSnDPORT(s, port);
    return 1;
}

static int sendUDP(SOCKET s)
{
    g_w5100->execCmdSn(s, Sock_SEND);

    while ((g_w5100->readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK )
    {
        if (g_w5100->readSnIR(s) & SnIR::TIMEOUT)
        {
            g_w5100->writeSnIR(s, (SnIR::SEND_OK|SnIR::TIMEOUT));
            return 0;
        }
    }

    g_w5100->writeSnIR(s, SnIR::SEND_OK);
    return 1; // sent ok
}

static uint16_t bufferData(SOCKET s, uint16_t offset, const uint8_t* buf, uint16_t len)
{
    uint16_t ret = len > g_w5100->getTXFreeSize(s) ? g_w5100->getTXFreeSize(s) : len;
    g_w5100->send_data_processing_offset(s, offset, buf, ret);
    return ret;
}

int EthernetUDP::beginPacket(uint32_t ip, uint16_t port)
{
    _offset = 0;
    return startUDP(_sock, (uint8_t *)&ip, port);
}

size_t EthernetUDP::write(const uint8_t *buffer, size_t size)
{
    uint16_t bytes_written = bufferData(_sock, _offset, buffer, size);
    _offset += bytes_written;
    return bytes_written;
}

int EthernetUDP::parsePacket()
{
    // discard any remaining bytes in the last packet
    flush();

    if (_eth->nw()->getRXReceivedSize(_sock) > 0)
    {
        //HACK - hand-parse the UDP packet using TCP recv method
        uint8_t tmpBuf[8];
        int ret =0; 
        //read 8 header bytes and get IP and port from it
        ret = recv(_sock,tmpBuf,8);

        if (ret > 0)
        {
            //_remoteIP = tmpBuf;
            my_memcpy(&_remoteIP, tmpBuf, 4);
            _remotePort = tmpBuf[4];
            _remotePort = (_remotePort << 8) + tmpBuf[5];
            _remaining = tmpBuf[6];
            _remaining = (_remaining << 8) + tmpBuf[7];
            ret = _remaining;  // When we get here, any remaining bytes are the data
        }
        return ret;
    }
    // There aren't any packets available
    return 0;
}

int EthernetUDP::read()
{
  uint8_t byte;

  if ((_remaining > 0) && (recv(_sock, &byte, 1) > 0))
  {
    // We read things without any problems
    _remaining--;
    return byte;
  }

  // If we get here, there's no data available
  return -1;
}

int EthernetUDP::read(unsigned char* buffer, size_t len)
{
    if (_remaining > 0)
    {
        int got;

        if (_remaining <= len)
        {
            // data should fit in the buffer
            got = recv(_sock, buffer, _remaining);
        }
        else
        {
            // too much data for the buffer, 
            // grab as much as will fit
            got = recv(_sock, buffer, len);
        }

        if (got > 0)
        {
            _remaining -= got;
            return got;
        }
    }

    // If we get here, there's no data available or recv failed
    return -1;
}

int EthernetUDP::endPacket()
{
    return sendUDP(_sock);
}

EthernetUDP::EthernetUDP(EthernetClass * const eth) : _eth(eth), _sock(MAX_SOCK_NUM)
{
}

int EthernetUDP::peek()
{
    uint8_t b;
    // Unlike recv, peek doesn't check to see if there's any data available, so we must.
    // If the user hasn't called parsePacket yet then return nothing otherwise they
    // may get the UDP header
    if (!_remaining)
        return -1;

    _eth->peek(_sock, &b);
    return b;
}


