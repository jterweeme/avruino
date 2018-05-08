#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <util/delay.h>
#include "w5100tcp.h"

uint16_t EthernetClient::_srcport = 1024;

uint8_t EthernetClient::connect(SOCKET s, uint8_t * addr, uint16_t port)
{
    if (((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
        ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
        (port == 0x00))
    {
        return 0;
    }

    _eth->nw()->writeSnDIPR(s, addr);  // set destination IP
    _eth->nw()->writeSnDPORT(s, port);
    _eth->nw()->execCmdSn(s, Sock_CONNECT);
    return 1;
}

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

    _eth->socket(_sock, SnMR::TCP, _srcport, 0);

    if (!connect(_sock, (uint8_t *)&ip, port))
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

uint16_t EthernetClient::send(SOCKET s, const uint8_t *buf, uint16_t len)
{
    uint8_t status=0;
    uint16_t ret=0;
    uint16_t freesize=0;

    ret = len > _eth->nw()->SSIZE ? _eth->nw()->SSIZE : len;

    do
    {
        freesize = _eth->nw()->getTXFreeSize(s);
        status = _eth->nw()->readSnSR(s);

        if ((status != SnSR::ESTABLISHED) && (status != SnSR::CLOSE_WAIT))
        {
            ret = 0;
            break;
        }
    }
    while (freesize < ret);

    // copy data
    _eth->nw()->send_data_processing(s, (uint8_t *)buf, ret);
    _eth->nw()->execCmdSn(s, Sock_SEND);

    /* +2008.01 bj */
    while ((_eth->nw()->readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK )
    {
        /* m2008.01 [bj] : reduce code */
        if (_eth->nw()->readSnSR(s) == SnSR::CLOSED )
        {
            _eth->close(s);
            return 0;
        }
    }

    _eth->nw()->writeSnIR(s, SnIR::SEND_OK);
    return ret;
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

int16_t EthernetClient::get()
{
    uint8_t b;

    if (_eth->recv(_sock, &b, 1) > 0)
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

int EthernetClient::read()
{
    uint8_t b;

    if (_eth->recv(_sock, &b, 1) > 0)
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
    return _eth->recv(_sock, buf, size);
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

void EthernetClient::disconnect(SOCKET s)
{
    _eth->nw()->execCmdSn(s, Sock_DISCON);
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
        _eth->close(_sock);

    _eth->_server_port[_sock] = 0;
    _sock = MAX_SOCK_NUM;
}

uint8_t EthernetClient::connected()
{
    if (_sock == MAX_SOCK_NUM)
        return 0;
  
    uint8_t s = status();

    return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
        (s == SnSR::CLOSE_WAIT && !available()));
}

uint8_t EthernetClient::status()
{
    if (_sock == MAX_SOCK_NUM)
        return SnSR::CLOSED;

    return _eth->nw()->readSnSR(_sock);
}

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.

EthernetClient::operator bool()
{
    return _sock != MAX_SOCK_NUM;
}

uint8_t EthernetServer::listen(SOCKET s)
{
    if (_eth->nw()->readSnSR(s) != SnSR::INIT)
        return 0;

    _eth->nw()->execCmdSn(s, Sock_LISTEN);
    return 1;
}

void EthernetServer::begin()
{
    for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
    {
        EthernetClient client(_eth, sock);

        if (client.status() == SnSR::CLOSED)
        {
            _eth->socket(sock, SnMR::TCP, _port, 0);
            listen(sock);
            _eth->_server_port[sock] = _port;
            break;
        }
    }
}

EthernetServer::EthernetServer(EthernetClass * const eth, uint16_t port) : _eth(eth), _port(port)
{
}

void EthernetServer::accept()
{
    int listening = 0;

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
    {
        EthernetClient client(_eth, sock);

        if (_eth->_server_port[sock] == _port)
        {
            if (client.status() == SnSR::LISTEN)
                listening = 1;
            else if (client.status() == SnSR::CLOSE_WAIT && !client.available())
                client.stop();
        }
    }

    if (!listening)
        begin();
}

EthernetClient EthernetServer::available()
{
    accept();

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
    {
        EthernetClient client(_eth, sock);

        if (_eth->_server_port[sock] == _port && (client.status() == SnSR::ESTABLISHED ||
            client.status() == SnSR::CLOSE_WAIT))
        {
            if (client.available())
                return client;  // don't always pick the lowest numbered socket.
        }
    }

    return EthernetClient(_eth, MAX_SOCK_NUM);
}

size_t EthernetServer::write(uint8_t b)
{
    return write(&b, 1);
}

size_t EthernetServer::write(const uint8_t *buffer, size_t size)
{
    size_t n = 0;
    accept();

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
    {
        EthernetClient client(_eth, sock);

        if (_eth->_server_port[sock] == _port && client.status() == SnSR::ESTABLISHED)
            n += client.write(buffer, size);
    }

    return n;
}


