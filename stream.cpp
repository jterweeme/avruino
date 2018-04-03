#include "stream.h"

void ostream::write(const char *s, size_t n)
{
    for (size_t i = 0; i < n; i++)
        put(s[i]);
}

void istream::read(char *s, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        int c = get();

        if (c == -1)
        {
            _lastread = i;
            return;
        }

        s[i] = c;
    }

    _lastread = n;
}

