#include "w5100server.h"
#include "w5100client.h"

extern W5100Class *g_w5100;

static uint8_t listen(SOCKET s)
{
    if (g_w5100->readSnSR(s) != SnSR::INIT)
        return 0;

    g_w5100->execCmdSn(s, Sock_LISTEN);
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

#if 0
Client EthernetServer::available2()
{
    accept();

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
    {
        EthernetClient client(_eth, sock);

        if (EthernetClass::_server_port[sock] == _port &&
            (client.status() == SnSR::ESTABLISHED ||
            client.status() == SnSR::CLOSE_WAIT))
        {
            if (client.available()) {
                // XXX: don't always pick the lowest numbered socket.
                return client;
            }
        }
    }

    return EthernetClient(_eth, MAX_SOCK_NUM);
}
#endif

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



