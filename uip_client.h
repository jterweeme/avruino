#ifndef _UIPCLIENT_H_
#define _UIPCLIENT_H_
#include "uip.h"

class UIPClient
{
private:
    UIPEthernetClass * const _eth;
public:
    UIPClient(UIPEthernetClass * const eth);
    int connect(IPAddrezz ip, uint16_t port);
    int connect(const char *host, uint16_t port);
    int read(uint8_t *buf, size_t size);
    void stop();
    uint8_t connected();
    operator bool();

    virtual bool operator==(const UIPClient& rhs)
    { return data && rhs.data && (data == rhs.data); }

    virtual bool operator!=(const UIPClient& rhs) { return !this->operator==(rhs); };
    size_t write(uint8_t c) { return _write(data, &c, 1); }
    size_t write(const uint8_t *buf, size_t size) { return _write(data, buf, size); }
    size_t write(const char *buf) { return write((const uint8_t *)buf, strlen(buf)); }
    int available();
    int read();
    int peek();
    void flush();
    static uip_userdata_t all_data[UIP_CONNS];
    UIPClient(struct uip_conn *_conn);
    UIPClient(UIPEthernetClass * const eth, uip_userdata_t* conn_data);
    static size_t _write(uip_userdata_t *, const uint8_t *buf, size_t size);
    static uip_userdata_t* _allocateData();
    static void _eatBlock(memhandle* blocks);
    static void _flushBlocks(memhandle* blocks);
private:
    uip_userdata_t *data;
    int _available(uip_userdata_t *);
    static uint8_t _currentBlock(memhandle* blocks);
};
#endif



