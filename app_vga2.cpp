#include "vga.h"
#include "sleepy.h"
#include "board.h"

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

extern "C" void TIMER1_OVF __attribute__ ((signal, used, externally_visible));
void TIMER1_OVF
{
    VGA::getInstance()->interrupt();
}

extern "C" void TIMER2_OVF __attribute__ ((signal, used, externally_visible));
void TIMER2_OVF
{
}




