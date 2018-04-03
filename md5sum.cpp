#include "md5sum.h"

static constexpr uint32_t k[64] =
{
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static constexpr uint32_t r[] =
{
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

static inline void dump32(uint32_t x, ostream &os)
{
    os.put(nibble(x >> 4 & 0xf));
    os.put(nibble(x & 0xf));
    os.put(nibble(x >> 12 & 0xf));
    os.put(nibble(x >> 8 & 0xf));
    os.put(nibble(x >> 20 & 0xf));
    os.put(nibble(x >> 16 & 0xf));
    os.put(nibble(x >> 28 & 0xf));
    os.put(nibble(x >> 24 & 0xf));
}

void Hash::dump(ostream &os) const
{
    dump32(_h0, os);
    dump32(_h1, os);
    dump32(_h2, os);
    dump32(_h3, os);
}

uint32_t Chunk::_to_int32(const uint8_t *bytes) const
{
    return (uint32_t)bytes[0]
        | ((uint32_t)bytes[1] << 8)
        | ((uint32_t)bytes[2] << 16)
        | ((uint32_t)bytes[3] << 24);
}

void Chunk::read(const uint8_t *msg)
{
    for (uint8_t i = 0; i < 16; i++)
        _w[i] = _to_int32(msg + i * 4);
}

Hash Chunk::calc(Hash &hash)
{
    uint32_t a, b, c, d, f, g, temp;
    a = hash.h0();
    b = hash.h1();
    c = hash.h2();
    d = hash.h3();

    for (uint8_t i = 0; i < 64; i++)
    {
        if (i < 16)
        {
            f = (b & c) | ((~b) & d);
            g = i;
        }
        else if (i < 32)
        {
            f = (d & b) | ((~d) & c);
            g = (5 * i + 1) % 16;
        }
        else if (i < 48)
        {
            f = b ^ c ^ d;
            g = (3 * i + 5) % 16;
        }
        else
        {
            f = c ^ (b | (~d));
            g = (7 * i) % 16;
        }

        temp = d;
        d = c;
        c = b;
        b = b + leftRotate((a + f + k[i] + w(g)), r[i]);
        a = temp;
    }

    Hash foo(a, b, c, d);
    return foo;
}

void Hasher::stream(istream &is, ostream &os)
{
    _hash.reset();
    bool kontinue = true;

    for (uint32_t i = 0; kontinue; i++)
    {
        uint8_t data[64] = {0};
        is.read((char *)data, 64);
        Chunk chunk;
        uint8_t cnt = is.gcount();

        if (cnt < 56)
        {
            data[cnt] = 0x80;
            chunk.read(data);
            chunk.fillTail(i * 64 + cnt);
            kontinue = false;
        }
        else if (cnt < 64)
        {
            data[cnt] = 0x80;
            chunk.read(data);
            Hash bar = chunk.calc(_hash);
            _hash.add(bar);
            chunk.clear();
            chunk.fillTail(i * 64 + cnt);
            kontinue = false;
        }
        else
        {
            chunk.read(data);
        }

        Hash bar = chunk.calc(_hash);
        _hash.add(bar);
    }
}



