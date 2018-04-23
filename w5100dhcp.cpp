// DHCP Library v0.3 - April 25, 2009
// Author: Jordan Terrell - blog.jordanterrell.com

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include "w5100dhcp.h"
#include "util.h"
#include "stream.h"

extern ostream *gout;

void randomSeed(uint16_t seed)
{
    if (seed != 0)
        srandom(seed);
}

long random(long howbig)
{
    return howbig == 0 ? 0 : random() % howbig;
}

long random(long howsmall, long howbig)
{
    return howsmall >= howbig ? howsmall : random(howbig - howsmall) + howsmall;
}


int DhcpClass::beginWithDHCP(uint8_t *mac, uint32_t timeout, uint32_t responseTimeout)
{
    _dhcpLeaseTime=0;
    _dhcpT1=0;
    _dhcpT2=0;
    _lastCheck=0;
    _timeout = timeout;
    _responseTimeout = responseTimeout;

    // zero out _dhcpMacAddr
    memset(_dhcpMacAddr, 0, 6); 
    reset_DHCP_lease();

    memcpy((void*)_dhcpMacAddr, (void*)mac, 6);
    _dhcp_state = STATE_DHCP_START;

    return request_DHCP_lease();
}

void DhcpClass::reset_DHCP_lease(){
    // zero out _dhcpSubnetMask, _dhcpGatewayIp, _dhcpLocalIp, _dhcpDhcpServerIp, _dhcpDnsServerIp
    memset(_dhcpLocalIp, 0, 20);
}

//return:0 on error, 1 if request is sent and response is received
int DhcpClass::request_DHCP_lease(){
    
    uint8_t messageType = 0;
  
    // Pick an initial transaction ID
    _dhcpTransactionId = random(1UL, 2000UL);
    _dhcpInitialTransactionId = _dhcpTransactionId;

    _dhcpUdpSocket.stop();
    if (_dhcpUdpSocket.begin(DHCP_CLIENT_PORT) == 0)
    {
      // Couldn't get a socket
      return 0;
    }
 
    presend_DHCP();
    
    int result = 0;
    
    //unsigned long startTime = millis();
    
    for (uint8_t tries = 0; _dhcp_state != STATE_DHCP_LEASED; tries++)
    {
        if (_dhcp_state == STATE_DHCP_START)
        {
            _dhcpTransactionId++;
            *gout << "Sending DHCP Start\r\n";
            //send_DHCP_MESSAGE(DHCP_DISCOVER, ((millis() - startTime) / 1000));
            send_DHCP_MESSAGE(DHCP_DISCOVER, 0);
            _dhcp_state = STATE_DHCP_DISCOVER;
        }
        else if(_dhcp_state == STATE_DHCP_REREQUEST){
            _dhcpTransactionId++;
            *gout << "Sending DHCP Request\r\n";
            //send_DHCP_MESSAGE(DHCP_REQUEST, ((millis() - startTime)/1000));
            send_DHCP_MESSAGE(DHCP_REQUEST, 0);
            _dhcp_state = STATE_DHCP_REQUEST;
        }
        else if(_dhcp_state == STATE_DHCP_DISCOVER)
        {
            uint32_t respId;
            messageType = parseDHCPResponse(_responseTimeout, respId);
            if(messageType == DHCP_OFFER)
            {
                // We'll use the transaction ID that the offer came with,
                // rather than the one we were up to
                _dhcpTransactionId = respId;
                *gout << "Sending DHCP Discover\r\n";
                //send_DHCP_MESSAGE(DHCP_REQUEST, ((millis() - startTime) / 1000));
                send_DHCP_MESSAGE(DHCP_REQUEST, 0);
                _dhcp_state = STATE_DHCP_REQUEST;
            }
        }
        else if(_dhcp_state == STATE_DHCP_REQUEST)
        {
            uint32_t respId;
            messageType = parseDHCPResponse(_responseTimeout, respId);
            if(messageType == DHCP_ACK)
            {
                _dhcp_state = STATE_DHCP_LEASED;
                result = 1;
                //use default lease time if we didn't get it
                if(_dhcpLeaseTime == 0){
                    _dhcpLeaseTime = DEFAULT_LEASE;
                }
                //calculate T1 & T2 if we didn't get it
                if (_dhcpT1 == 0)
                {
                    //T1 should be 50% of _dhcpLeaseTime
                    _dhcpT1 = _dhcpLeaseTime >> 1;
                }
                if (_dhcpT2 == 0)
                {
                    //T2 should be 87.5% (7/8ths) of _dhcpLeaseTime
                    _dhcpT2 = _dhcpT1 << 1;
                }
                _renewInSec = _dhcpT1;
                _rebindInSec = _dhcpT2;
            }
            else if(messageType == DHCP_NAK)
                _dhcp_state = STATE_DHCP_START;
        }
        
        if(messageType == 255)
        {
            messageType = 0;
            _dhcp_state = STATE_DHCP_START;
        }
#if 0
        if(result != 1 && ((millis() - startTime) > _timeout))
            break;
#else
        if (tries > 5)
            break;
#endif
    }
 
     *gout << "Debug bericht\r\n";  
    // We're done with the socket now
    _dhcpUdpSocket.stop();
    _dhcpTransactionId++;

    return result;
}

void DhcpClass::presend_DHCP()
{
}

void DhcpClass::send_DHCP_MESSAGE(uint8_t messageType, uint16_t secondsElapsed)
{
    *gout << "Send DHCP Message...\r\n";
    uint8_t buffer[32];
    memset(buffer, 0, 32);

    if (_dhcpUdpSocket.beginPacket(0xffffffff, DHCP_SERVER_PORT) == -1)
    {
        *gout << "DHCP UDP Socket begin packet ERROR!\r\n";
        // FIXME Need to return errors
        return;
    }

    buffer[0] = DHCP_BOOTREQUEST;   // op
    buffer[1] = DHCP_HTYPE10MB;     // htype
    buffer[2] = DHCP_HLENETHERNET;  // hlen
    buffer[3] = DHCP_HOPS;          // hops

    // xid
    unsigned long xid = htonl(_dhcpTransactionId);
    memcpy(buffer + 4, &(xid), 4);

    // 8, 9 - seconds elapsed
    buffer[8] = ((secondsElapsed & 0xff00) >> 8);
    buffer[9] = (secondsElapsed & 0x00ff);

    // flags
    unsigned short flags = htons(DHCP_FLAGSBROADCAST);
    memcpy(buffer + 10, &(flags), 2);

    // ciaddr: already zeroed
    // yiaddr: already zeroed
    // siaddr: already zeroed
    // giaddr: already zeroed

    //put data in W5100 transmit buffer
    _dhcpUdpSocket.write(buffer, 28);
    *gout << "Debug bericht #2\r\n";
    memset(buffer, 0, 32); // clear local buffer

    memcpy(buffer, _dhcpMacAddr, 6); // chaddr

    //put data in W5100 transmit buffer
    _dhcpUdpSocket.write(buffer, 16);

    memset(buffer, 0, 32); // clear local buffer

    // leave zeroed out for sname && file
    // put in W5100 transmit buffer x 6 (192 bytes)
  
    for(int i = 0; i < 6; i++) {
        _dhcpUdpSocket.write(buffer, 32);
    }

    *gout << "Debug bericht #4\r\n";

    // OPT - Magic Cookie
    buffer[0] = (uint8_t)((MAGIC_COOKIE >> 24)& 0xFF);
    buffer[1] = (uint8_t)((MAGIC_COOKIE >> 16)& 0xFF);
    buffer[2] = (uint8_t)((MAGIC_COOKIE >> 8)& 0xFF);
    buffer[3] = (uint8_t)(MAGIC_COOKIE& 0xFF);

    // OPT - message type
    buffer[4] = dhcpMessageType;
    buffer[5] = 0x01;
    buffer[6] = messageType; //DHCP_REQUEST;

    // OPT - client identifier
    buffer[7] = dhcpClientIdentifier;
    buffer[8] = 0x07;
    buffer[9] = 0x01;
    memcpy(buffer + 10, _dhcpMacAddr, 6);

    // OPT - host name
    buffer[16] = hostName;
    buffer[17] = strlen(HOST_NAME) + 6; // length of hostname + last 3 bytes of mac address
    strcpy((char*)&(buffer[18]), HOST_NAME);

    printByte((char*)&(buffer[24]), _dhcpMacAddr[3]);
    printByte((char*)&(buffer[26]), _dhcpMacAddr[4]);
    printByte((char*)&(buffer[28]), _dhcpMacAddr[5]);

    //put data in W5100 transmit buffer
    _dhcpUdpSocket.write(buffer, 30);
    *gout << "Debug bericht #5\r\n";

    if(messageType == DHCP_REQUEST)
    {
        buffer[0] = dhcpRequestedIPaddr;
        buffer[1] = 0x04;
        buffer[2] = _dhcpLocalIp[0];
        buffer[3] = _dhcpLocalIp[1];
        buffer[4] = _dhcpLocalIp[2];
        buffer[5] = _dhcpLocalIp[3];

        buffer[6] = dhcpServerIdentifier;
        buffer[7] = 0x04;
        buffer[8] = _dhcpDhcpServerIp[0];
        buffer[9] = _dhcpDhcpServerIp[1];
        buffer[10] = _dhcpDhcpServerIp[2];
        buffer[11] = _dhcpDhcpServerIp[3];

        //put data in W5100 transmit buffer
        _dhcpUdpSocket.write(buffer, 12);
    }
    
    buffer[0] = dhcpParamRequest;
    buffer[1] = 0x06;
    buffer[2] = subnetMask;
    buffer[3] = routersOnSubnet;
    buffer[4] = dns;
    buffer[5] = domainName;
    buffer[6] = dhcpT1value;
    buffer[7] = dhcpT2value;
    buffer[8] = endOption;
    
    //put data in W5100 transmit buffer
    _dhcpUdpSocket.write(buffer, 9);
    *gout << "debug bericht #3\r\n";
    _dhcpUdpSocket.endPacket();
    *gout << "end DHCP Message send\r\n";
}

uint8_t DhcpClass::parseDHCPResponse(unsigned long responseTimeout, uint32_t& transactionId)
{
    uint8_t type = 0;
    uint8_t opt_len = 0;
    //uint32_t startTime = millis();

    for (uint32_t i = 0; _dhcpUdpSocket.parsePacket() <= 0; i++)
    {
        //if ((millis() - startTime) > responseTimeout)
        if (i > 0xff)
            return 255;
        
        _delay_ms(50);
    }
    // start reading in the packet
    RIP_MSG_FIXED fixedMsg;
    _dhcpUdpSocket.read((uint8_t*)&fixedMsg, sizeof(RIP_MSG_FIXED));
  
    if(fixedMsg.op == DHCP_BOOTREPLY && _dhcpUdpSocket.remotePort() == DHCP_SERVER_PORT)
    {
        transactionId = ntohl(fixedMsg.xid);
        if(memcmp(fixedMsg.chaddr, _dhcpMacAddr, 6) != 0 || (transactionId < _dhcpInitialTransactionId) || (transactionId > _dhcpTransactionId))
        {
            // Need to read the rest of the packet here regardless
            _dhcpUdpSocket.flush();
            return 0;
        }

        memcpy(_dhcpLocalIp, fixedMsg.yiaddr, 4);

        // Skip to the option part
        // Doing this a byte at a time so we don't have to put a big buffer
        // on the stack (as we don't have lots of memory lying around)
        for (int i =0; i < (240 - (int)sizeof(RIP_MSG_FIXED)); i++)
        {
            _dhcpUdpSocket.read(); // we don't care about the returned byte
        }

        while (_dhcpUdpSocket.available() > 0) 
        {
            switch (_dhcpUdpSocket.read()) 
            {
                case endOption:
                case padOption:
                    break;
                case dhcpMessageType:
                    opt_len = _dhcpUdpSocket.read();
                    type = _dhcpUdpSocket.read();
                    break;
                case subnetMask:
                    opt_len = _dhcpUdpSocket.read();
                    _dhcpUdpSocket.read(_dhcpSubnetMask, 4);
                    break;
                case routersOnSubnet :
                    opt_len = _dhcpUdpSocket.read();
                    _dhcpUdpSocket.read(_dhcpGatewayIp, 4);

                    for (int i = 0; i < opt_len-4; i++)
                        _dhcpUdpSocket.read();

                    break;
                case dns :
                    opt_len = _dhcpUdpSocket.read();
                    _dhcpUdpSocket.read(_dhcpDnsServerIp, 4);
                    for (int i = 0; i < opt_len-4; i++)
                    {
                        _dhcpUdpSocket.read();
                    }
                    break;
                
                case dhcpServerIdentifier :
                    opt_len = _dhcpUdpSocket.read();
                    if( *((uint32_t*)_dhcpDhcpServerIp) == 0 ||
                        *((uint32_t *)_dhcpDhcpServerIp) == _dhcpUdpSocket.remoteIP())
                        //IPAddress(_dhcpDhcpServerIp) == IPAddress(_dhcpUdpSocket.remoteIP()))
                    {
                        _dhcpUdpSocket.read(_dhcpDhcpServerIp, sizeof(_dhcpDhcpServerIp));
                    }
                    else
                    {
                        // Skip over the rest of this option
                        while (opt_len--)
                            _dhcpUdpSocket.read();
                    }
                    break;

                case dhcpT1value : 
                    opt_len = _dhcpUdpSocket.read();
                    _dhcpUdpSocket.read((uint8_t*)&_dhcpT1, sizeof(_dhcpT1));
                    _dhcpT1 = ntohl(_dhcpT1);
                    break;

                case dhcpT2value : 
                    opt_len = _dhcpUdpSocket.read();
                    _dhcpUdpSocket.read((uint8_t*)&_dhcpT2, sizeof(_dhcpT2));
                    _dhcpT2 = ntohl(_dhcpT2);
                    break;

                case dhcpIPaddrLeaseTime :
                    opt_len = _dhcpUdpSocket.read();
                    _dhcpUdpSocket.read((uint8_t*)&_dhcpLeaseTime, sizeof(_dhcpLeaseTime));
                    _dhcpLeaseTime = ntohl(_dhcpLeaseTime);
                    _renewInSec = _dhcpLeaseTime;
                    break;

                default :
                    opt_len = _dhcpUdpSocket.read();
                    // Skip over the rest of this option
                    while (opt_len--)
                        _dhcpUdpSocket.read();
                    
                    break;
            }
        }
    }

    // Need to skip to end of the packet regardless here
    _dhcpUdpSocket.flush();

    return type;
}


/*
    returns:
    0/DHCP_CHECK_NONE: nothing happened
    1/DHCP_CHECK_RENEW_FAIL: renew failed
    2/DHCP_CHECK_RENEW_OK: renew success
    3/DHCP_CHECK_REBIND_FAIL: rebind fail
    4/DHCP_CHECK_REBIND_OK: rebind success
*/
#if 0
int DhcpClass::checkLease()
{
    //this uses a signed / unsigned trick to deal with millis overflow
    uint32_t now = millis();
    int32_t snow = (int32_t)now;
    int rc=DHCP_CHECK_NONE;

    if (_lastCheck != 0)
    {
        //calc how many ms past the timeout we are
        int32_t factor = snow - (int32_t)_secTimeout;
        //if on or passed the timeout, reduce the counters
        if (factor >= 0)
        {
            //next timeout should be now plus 1000 ms minus parts of second in factor
            _secTimeout = snow + 1000 - factor % 1000;
            //how many seconds late are we, minimum 1
            factor = factor / 1000 +1;
            
            //reduce the counters by that mouch
            //if we can assume that the cycle time (factor) is fairly constant
            //and if the remainder is less than cycle time * 2 
            //do it early instead of late
            if (_renewInSec < factor * 2)
                _renewInSec = 0;
            else
                _renewInSec -= factor;
            
            if (_rebindInSec < factor * 2)
                _rebindInSec = 0;
            else
                _rebindInSec -= factor;
        }

        //if we have a lease but should renew, do it
        if (_dhcp_state == STATE_DHCP_LEASED && _renewInSec <=0){
            _dhcp_state = STATE_DHCP_REREQUEST;
            rc = 1 + request_DHCP_lease();
        }

        //if we have a lease or is renewing but should bind, do it
        if ((_dhcp_state == STATE_DHCP_LEASED || _dhcp_state == STATE_DHCP_START)
            && _rebindInSec <=0)
        {
            //this should basically restart completely
            _dhcp_state = STATE_DHCP_START;
            reset_DHCP_lease();
            rc = 3 + request_DHCP_lease();
        }
    }
    else
    {
        _secTimeout = snow + 1000;
    }

    _lastCheck = now;
    return rc;
}
#endif

void DhcpClass::printByte(char * buf, uint8_t n)
{
    char *str = &buf[1];
    buf[0] = '0';

    do
    {
        uint32_t m = n;
        n /= 16;
        char c = m - 16 * n;
        *str-- = c < 10 ? c + '0' : c + 'A' - 10;
    }
    while (n);
}



