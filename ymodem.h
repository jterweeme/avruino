#include "stream.h"

#define ENABLE_YSENDER

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
    int send(char *fn);
};
#endif



