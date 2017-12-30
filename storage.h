#ifndef _STORAGE_H_
#define _STORAGE_H_

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

class string
{
    char *s;
    size_t _capacity;
public:
    string(const char *s);
    size_t inline capacity() { return _capacity; }
    size_t inline size() const { return Utility::strlen(s); }
    size_t inline length() { return size(); }
    const char *c_str() const { return s; }
    void reserve(size_t n) { _capacity += n; ::realloc(s, _capacity); }
    void append(const char *s);
    void append(const string &s);
};

#endif

