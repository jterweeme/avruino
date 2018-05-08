#ifndef _UIPCLIENT_H_
#define _UIPCLIENT_H_
#include "client.h"
#include "enc28j60ip.h"

class UIPClient : public Client
{
private:
    Enc28J60IP * const _eth;
public:
    UIPClient(Enc28J60IP * const eth);
    UIPClient(struct uip_conn *_conn);
    UIPClient(Enc28J60IP * const eth, uip_userdata_t* conn_data);
    int16_t connect(uint32_t ip, uint16_t port);
    void stop();
    uint8_t connected();
    operator bool();

    virtual bool operator==(const UIPClient& rhs)
    { return data && rhs.data && (data == rhs.data); }

    virtual bool operator!=(const UIPClient &rhs) { return !this->operator==(rhs); };
    size_t write(uint8_t c) { return _eth->_write(data, &c, 1); }
    size_t write(const uint8_t *buf, size_t size) { return _eth->_write(data, buf, size); }
    size_t write(const char *buf) { return write((const uint8_t *)buf, my_strlen(buf)); }
    int16_t available();
    int16_t get();
    int16_t read();
    int16_t read(uint8_t *buf, size_t size);
    int16_t peek();
    void flush();
private:
    uip_userdata_t *data;
    int _available(uip_userdata_t *);
};

class UIPServer
{
private:
    Enc28J60IP * const _eth;
    const uint16_t _port;
public:
    UIPServer(Enc28J60IP * const eth, uint16_t port) : _eth(eth), _port(htons(port)) { }
    UIPClient available();
    void begin();
    size_t write(uint8_t c) { return write(&c, 1); }
    size_t write(const uint8_t *buf, size_t size);
};
#endif



