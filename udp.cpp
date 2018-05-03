#include "udp.h"
#include "misc.h"

size_t UDP::write(const char *str)
{
    if (str == 0)
        return 0;
    return write((const uint8_t *)str, my_strlen(str));
}

size_t UDP::write(const uint8_t *buffer, size_t size)
{
    size_t n = 0;

    while (size--)
        n += write(*buffer++);

    return n;
}


