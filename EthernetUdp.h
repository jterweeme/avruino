/*
 *  Udp.cpp: Library to send/receive UDP packets with the Arduino ethernet shield.
 *  This version only offers minimal wrapping of socket.c/socket.h
 *  Drop Udp.h/.cpp into the Ethernet library directory at hardware/libraries/Ethernet/ 
 *
 * NOTE: UDP is fast, but has some important limitations (thanks to Warren Gray for mentioning these)
 * 1) UDP does not guarantee the order in which assembled UDP packets are received. This
 * might not happen often in practice, but in larger network topologies, a UDP
 * packet can be received out of sequence. 
 * 2) UDP does not guard against lost packets - so packets *can* disappear without the sender being
 * aware of it. Again, this may not be a concern in practice on small local networks.
 * For more information, see http://www.cafeaulait.org/course/week12/35.html
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

#ifndef ethernetudp_h
#define ethernetudp_h

#include "udp.h"
#include "socket.h"

class EthernetClass;

#define UDP_TX_PACKET_MAX_SIZE 24

class EthernetUDP : public UDP
{
private:
    EthernetClass * const _eth;
    uint8_t _sock;  // socket ID for Wiz5100
    uint16_t _port; // local port to listen on
    IPAddress _remoteIP; // remote IP address for the incoming packet whilst it's being processed
    uint16_t _remotePort; // remote port for the incoming packet whilst it's being processed
    uint16_t _offset; // offset into the packet being sent
    uint16_t _remaining; // remaining bytes of incoming packet yet to be processed
public:
    EthernetUDP(EthernetClass * const eth) : _eth(eth), _sock(MAX_SOCK_NUM) { }
    virtual uint8_t begin(uint16_t);	// initializeo sockets available to use
    virtual void stop();  // Finish with the UDP socket
    virtual int beginPacket(IPAddress ip, uint16_t port);
    virtual int beginPacket(const char *host, uint16_t port);
    virtual int endPacket() { return sendUDP(_sock); }
    virtual size_t write(const uint8_t *buffer, size_t size);
    virtual size_t write(uint8_t c) { return write(&c, 1); }
    using Print::write;
    virtual int parsePacket();
    virtual int available() { return _remaining; }
    virtual int read();
    virtual int read(uint8_t *buffer, size_t len);
    virtual int read(char* buffer, size_t len) { return read((unsigned char*)buffer, len); };
    virtual int peek();
    virtual void flush() { while (_remaining) { read(); } }
    virtual IPAddress remoteIP() { return _remoteIP; };
    virtual uint16_t remotePort() { return _remotePort; };
};

#endif
