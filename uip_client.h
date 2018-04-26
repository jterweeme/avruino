#ifndef _UIPCLIENT_H_
#define _UIPCLIENT_H_
#include "client.h"
#include "uip.h"

class UIPClient : public Client
{
private:
    UIPEthernetClass * const _eth;
public:
    UIPClient(UIPEthernetClass * const eth);
    UIPClient(struct uip_conn *_conn);
    UIPClient(UIPEthernetClass * const eth, uip_userdata_t* conn_data);
    int connect(uint32_t ip, uint16_t port);
    //int connect(const char *host, uint16_t port);
    int read(uint8_t *buf, size_t size);
    void stop();
    uint8_t connected();
    operator bool();

    virtual bool operator==(const UIPClient& rhs)
    { return data && rhs.data && (data == rhs.data); }

    virtual bool operator!=(const UIPClient &rhs) { return !this->operator==(rhs); };
    size_t write(uint8_t c) { return _eth->_write(data, &c, 1); }
    size_t write(const uint8_t *buf, size_t size) { return _eth->_write(data, buf, size); }
    size_t write(const char *buf) { return write((const uint8_t *)buf, my_strlen(buf)); }
    int available();
    int read();
    int peek();
    void flush();
private:
    uip_userdata_t *data;
    int _available(uip_userdata_t *);
};
#endif



