#include "board.h"
#include <avr/io.h>

class Sd2Card
{
    SPIBus spi;
    Pin * const cs;
public:
    Sd2Card(Pin *cs);
    uint8_t command(uint8_t cmd, uint32_t arg);
    uint8_t waitNotBusy(uint16_t timeOutMillis);
    static uint8_t const CMD0 = 0x00;
    static uint8_t const CMD8 = 0x08;
    static uint8_t const CMD9 = 0x09;
    static uint8_t const R1_IDLE_STATE = 0x01;
};

class App : public Board
{
    DefaultUart uart;
    Sd2Card sdCard;
public:
    App();
    int run();
};

Sd2Card::Sd2Card(Pin *cs) : cs(cs)
{
    cs->direction(OUTPUT);

    for (uint8_t i = 0; i < 10; i++)
        spi.transfer(0xff);

    cs->clear();
    uint8_t status;
    while ((status = command(CMD0, 0)) != R1_IDLE_STATE) { }
}

uint8_t Sd2Card::command(uint8_t cmd, uint32_t arg)
{
    cs->clear();
    waitNotBusy(300);
    spi.transfer(cmd | 0x40);
    
    for (int8_t s = 24; s >= 0; s -= 8)
        spi.transfer(arg >> s);

    uint8_t crc = 0xff;

    if (cmd == CMD0)
        crc = 0x95;

    if (cmd == CMD8)
        crc = 0x87;

    spi.transfer(crc);
    uint8_t status;
    for (uint8_t i = 0; (status = spi.read() & 0x80) && i != 0xff; i++) { }
    return status;
}

uint8_t Sd2Card::waitNotBusy(uint16_t timeout)
{
    while (true)
        if (spi.read() == 0xff)
            return true;

    return 0;
}

App::App() : sdCard(&pin4)
{
    uart.puts("3\r\n");
    Utility::delay(0xffff);
    uart.puts("2\r\n");
    Utility::delay(0xffff);
    uart.puts("1\r\n");
    Utility::delay(0xffff);
    uart.puts("Start SD Card Reader\r\n");
}

int App::run()
{
    return 0;
}

int main()
{
    App app;
    return app.run();
}


