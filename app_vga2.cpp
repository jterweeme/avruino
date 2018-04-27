#include "vga.h"
#include "sleepy.h"

int main()
{
    VGA vga;

    while (true)
    {
        sleep_mode();
        vga.scanLine();
    }

    return 0;
}


