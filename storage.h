#ifndef _STORAGE_H_
#define _STORAGE_H_
#include "types.h"
#include <stdlib.h>

static inline size_t my_strlen(const char *s)
{
    const char *t;
    for (t = s; *t; ++t);
    return t - s;
}

static inline void my_strcpy(char *d, const char *s)
{
    while ((*d++ = *s++));
}

static inline int my_strcmp(const char *a, const char *b)
{
    for (; *a == *b; a++, b++)
        if (*a == '\0')
            return 0;

    return *a - *b;
}

static inline int my_strncmp(const char *a, const char *b, size_t n)
{
    while (n--)
        if (*a++ != *b++)
            return *(uint8_t *)(a - 1) - *(uint8_t *)(b - 1);

    return 0;
}

template <size_t N> class bitset
{
public:
    uint8_t data;
    bitset() : data(0) { }
    bitset(uint8_t data) : data(data) { }
    bool test(size_t pos) const { return data & 1<<pos; }
    void set() { data = 0xff; }
    void set(size_t pos, bool val = true) { if (val) data |= 1<<pos; else data &= ~(1<<pos); }
    unsigned long to_ulong() const { return data; }
};

inline void * operator new (size_t size) { return malloc(size); }
inline void * operator new[] (size_t size) { return malloc(size); }

template <class T> class vector
{
protected:
    size_t capacity;
    size_t size;
    T *buffer;
public:
    typedef T *iterator;
    size_t getSize() const { return size; }
    vector(size_t capacity) : capacity(capacity), size(0) { buffer = new T[capacity]; }
    void push_back(const T &value) { buffer[size++] = value; }
    iterator begin() { return buffer; }
    iterator end() { return buffer + getSize(); }
    T & operator [] (unsigned index) { return buffer[index]; }
};

template <typename T, size_t N> struct MyArray
{
    T elems[N];
    constexpr size_t size() const { return N; }
    T &at(size_t n) const { return const_cast<T&>(elems[n]); }
};

template <uint8_t N> class RingBuf
{
private:
    volatile uint8_t _buf[N];
public:
    volatile uint8_t head = 0, tail = 0;
    void push(uint8_t v) { uint8_t i = (head + 1) % N; if (i != tail) { _buf[i] = v; head = i; } }
    bool empty() const { return head == tail; }
    inline uint8_t get(uint8_t pos) { if (empty()) return 0; tail = pos; return _buf[pos]; }
    inline uint8_t get() { uint8_t i = tail; if (++i >= N) i = 0; return get(i); }
};

class string
{
    char *s;
    size_t _capacity;
public:
    string(const char *s);
    size_t inline capacity() { return _capacity; }
    size_t inline size() const { return my_strlen(s); }
    size_t inline length() { return size(); }
    const char *c_str() const { return s; }
    void reserve(size_t n) { _capacity += n; ::realloc(s, _capacity); }
    void append(const string &s);
    void append(const char *s) { append(string(s)); }
};

#endif

