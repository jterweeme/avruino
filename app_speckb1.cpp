// ZX Spectrum keyboard input, serial output

#include "board.h"
#include "stream.h"

int main()
{
    DefaultUart uart;
    UartStream cout(&uart);
    *p_pinA0_ddr |= 1<<pinA0_bit;
    *p_pinA1_ddr |= 1<<pinA1_bit;
    *p_pinA2_ddr |= 1<<pinA2_bit;
    *p_pinA3_ddr |= 1<<pinA3_bit;
    *p_pinA4_ddr |= 1<<pinA4_bit;
    *p_pin6_ddr &= ~(1<<pin6_bit);
    *p_pin7_ddr &= ~(1<<pin7_bit);
    *p_pin8_ddr &= ~(1<<pin8_bit);
    *p_pin9_ddr &= ~(1<<pin9_bit);
    *p_pin10_ddr &= ~(1<<pin10_bit);
    *p_pin11_ddr &= ~(1<<pin11_bit);
    *p_pin12_ddr &= ~(1<<pin12_bit);
    *p_pin13_ddr &= ~(1<<pin13_bit);
    *p_pin6_port &= ~(1<<pin6_bit);

    while (true)
    {
        *p_pinA0_port |= 1<<pinA0_bit;
        *p_pin6_port &= ~(1<<pin6_bit);
        
        if (*p_pin6_in & 1<<pin6_bit)
        {
            cout << "Space\r\n";

            for (volatile uint32_t i = 0; i < 0x1ffff; i++) // delay
                ;
        }
            
    }
}



