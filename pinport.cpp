#include "pinport.h"

void Pin::direction(Direction dir)
{
    switch (dir)
    {
    case INPUT:
        *port.direction &= ~(1<<bit);
        break;
    case OUTPUT:
        *port.direction |= 1<<bit;
        break;
    }
}



