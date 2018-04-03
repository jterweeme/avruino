#ifndef _MD5SUM_H_
#define _MD5SUM_H_
#include "stream.h"

class Hash
{
private:
    uint32_t _h0, _h1, _h2, _h3;
public:
    uint32_t h0() const { return _h0; }
    uint32_t h1() const { return _h1; }
    uint32_t h2() const { return _h2; }
    uint32_t h3() const { return _h3; }
    void dump(ostream &os) const;
    void add(Hash &h) { _h0 += h.h0(); _h1 += h.h1(); _h2 += h.h2(); _h3 += h.h3(); }
    Hash() { reset(); }
    Hash(uint32_t a, uint32_t b, uint32_t c, uint32_t d) : _h0(a), _h1(b), _h2(c), _h3(d) { }
    void reset() { _h0 = 0x67452301; _h1 = 0xefcdab89; _h2 = 0x98badcfe; _h3 = 0x10325476; }
};

class Chunk
{
private:
    uint32_t _w[16];
    uint32_t leftRotate(uint32_t x, uint32_t c) { return x << c | x >> (32 - c); }
    uint32_t _to_int32(const uint8_t *bytes) const;
public:
    void clear() { for (uint8_t i = 0; i < 16; i++) _w[i] = 0; }
    uint32_t w(uint32_t g) const { return _w[g]; }
    void fillTail(uint32_t size) { _w[14] = size * 8; _w[15] = size >> 29; }
    Hash calc(Hash &hash);
    void read(const uint8_t *msg);
};

class Hasher
{
private:
    Hash _hash;
public:
    void reset() { _hash.reset(); }
    void stream(istream &is, ostream &os);
    Hash hash() const { return _hash; }
};

#endif



