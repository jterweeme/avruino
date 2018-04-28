#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <util/delay.h>
#include "socket.h"
#include "w5100client.h"

uint16_t EthernetClient::_srcport = 1024;

int EthernetClient::connect(uint32_t ip, uint16_t port)
{
    if (_sock != MAX_SOCK_NUM)
        return 0;

    for (int i = 0; i < MAX_SOCK_NUM; i++)
    {
        uint8_t s = _eth->nw()->readSnSR(i);
        if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT || s == SnSR::CLOSE_WAIT)
        {
            _sock = i;
            break;
        }
    }

    if (_sock == MAX_SOCK_NUM)
        return 0;

    _srcport++;

    if (_srcport == 0)
        _srcport = 1024;

    socket(_sock, SnMR::TCP, _srcport, 0);

    if (!::connect(_sock, (uint8_t *)&ip, port))
    {
        _sock = MAX_SOCK_NUM;
        return 0;
    }

    while (status() != SnSR::ESTABLISHED)
    {
        _delay_ms(1);

        if (status() == SnSR::CLOSED)
        {
            _sock = MAX_SOCK_NUM;
            return 0;
        }
    }

  return 1;
}

size_t EthernetClient::write(const uint8_t *buf, size_t size)
{
    if (_sock == MAX_SOCK_NUM)
        return 0;

    if (!send(_sock, buf, size))
        return 0;

    return size;
}

int EthernetClient::available()
{
    return _sock != MAX_SOCK_NUM ? _eth->nw()->getRXReceivedSize(_sock) : 0;
}

int EthernetClient::read() {
  uint8_t b;
  if ( recv(_sock, &b, 1) > 0 )
  {
    // recv worked
    return b;
  }
  else
  {
    // No data available
    return -1;
  }
}

int EthernetClient::read(uint8_t *buf, size_t size)
{
    return recv(_sock, buf, size);
}

int EthernetClient::peek()
{
    uint8_t b;
    // Unlike recv, peek doesn't check to see if there's any data available, so we must
    if (!available())
        return -1;

    _eth->peek(_sock, &b);
    return b;
}

void EthernetClient::flush()
{
    while (available())
        read();
}

void EthernetClient::stop()
{
    if (_sock == MAX_SOCK_NUM)
        return;

    // attempt to close the connection gracefully (send a FIN to other side)
    disconnect(_sock);
    //unsigned long start = millis();
    

    // wait a second for the connection to close
    //while (status() != SnSR::CLOSED && millis() - start < 1000)
    for (uint16_t ms = 0; status() != SnSR::CLOSED && ms < 1000; ms++)
        _delay_ms(1);

    // if it hasn't closed, close it forcefully
    if (status() != SnSR::CLOSED)
        close(_sock);

    _eth->_server_port[_sock] = 0;
    _sock = MAX_SOCK_NUM;
}

uint8_t EthernetClient::connected() {
  if (_sock == MAX_SOCK_NUM) return 0;
  
  uint8_t s = status();
  return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
    (s == SnSR::CLOSE_WAIT && !available()));
}

uint8_t EthernetClient::status() {
    if (_sock == MAX_SOCK_NUM)
        return SnSR::CLOSED;
    return _eth->nw()->readSnSR(_sock);
}

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.

EthernetClient::operator bool() {
  return _sock != MAX_SOCK_NUM;
}
