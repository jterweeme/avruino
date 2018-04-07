#include "stream.h"

#define ENABLE_XRECEIVER
#define ENABLE_XSENDER

#ifdef ENABLE_XRECEIVER
class XReceiver
{
private:
    istream * const _is;
    ostream * const _os;
    int16_t _read(uint32_t timeout);
    int _getsec();
    uint8_t _secbuf[128];
public:
    XReceiver(istream *is, ostream *os) : _is(is), _os(os) { }
    void receive(ostream &os);
};
#endif

#ifdef ENABLE_XSENDER
class XSender
{
private:
    char _txbuf[128];
    istream * const _is;
    ostream * const _os;
    int putsec(int sectnum, size_t cseclen);
    size_t _filbuf(istream &is) { is.read(_txbuf, 128); return is.gcount(); }
public:
    XSender(istream *is, ostream *os) : _is(is), _os(os) { }
    int send(istream &is);
};
#endif


