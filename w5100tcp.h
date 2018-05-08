#ifndef _ETHERNETCLIENT_H_
#define _ETHERNETCLIENT_H_
#include "client.h"
#include "w5100ip.h"

class EthernetClient : public Client
{
private:
    EthernetClass * const _eth;
    uint8_t _sock;
    static uint16_t _srcport;
    uint16_t send(uint8_t socket, const uint8_t *buf, uint16_t len);
    uint8_t connect(SOCKET s, uint8_t * addr, uint16_t port);
    void disconnect(SOCKET s);
public:
    EthernetClient(EthernetClass * const eth) : _eth(eth), _sock(MAX_SOCK_NUM) { }
    EthernetClient(EthernetClass * const eth, uint8_t sock) : _eth(eth), _sock(sock) { }
    uint8_t status();
    int connect(uint32_t ip, uint16_t port);
    size_t write(const uint8_t *buf, size_t size);
    int available();
    int16_t get();
    int16_t read();
    int16_t read(uint8_t *buf, size_t size);
    int16_t peek();
    void flush();
    void stop();
    uint8_t connected();
    operator bool();
};

class EthernetServer
{
private:
    EthernetClass * const _eth;
    uint16_t _port;
    void accept();
    uint8_t listen(uint8_t socket);
public:
    EthernetServer(EthernetClass * const eth, uint16_t port);
    EthernetClient available();
    virtual void begin();
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buf, size_t size);
};
#endif


