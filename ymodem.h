#include "stream.h"
#include "fatty.h"

#define ENABLE_YRECEIVER
#define ENABLE_YSENDER

#ifdef ENABLE_YRECEIVER
class YReceiver
{
private:
    istream * const _is;
    ostream * const _os;
    uint8_t _secbuf[128];
    int _getsec();
    uint8_t _firstsec = 1;
    char _fn[50];
    long _modtime;
    uint16_t _mode;
    long _filesize;
    int _wcrxpn();
    int wcrx(ostream &os);
    void _procheader();
    int16_t _read(uint32_t timeout);    // non blocking read
public:
    YReceiver(istream *is, ostream *os) : _is(is), _os(os) { }
    void receive(Fatty &zd);
};
#endif

#ifdef ENABLE_YSENDER
class YSender
{
private:
    istream * const _is;
    ostream * const _os;
    uint8_t firstsec, _crcflag = 1;
    char _txbuf[128];
    void _sync();
    int wctx(istream &is);
    int putsec(uint8_t sectnum, size_t cseclen);
    int wctxpn(const char *fn, uint32_t filesize, uint16_t time, uint16_t mode);
    size_t filbuf(istream &is);
public:
    YSender(istream *is, ostream *os) : _is(is), _os(os) { }
    int send(istream &is, const char *fn, uint32_t filesize, uint16_t time, uint16_t mode);
};
#endif



