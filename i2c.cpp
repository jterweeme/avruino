#include "i2c.h"
#include <stdio.h>

I2CBus::I2CBus(unsigned brr)
  :
    twbr((uint8_t *)0xb8),
    twsr((uint8_t *)0xb9),
    twar((uint8_t *)0xba),
    twdr((uint8_t *)0xbb),
    twcr((uint8_t *)0xbc),
    slaves(128)
{
    *twcr = 0;
    *twar = 0;
    *twsr = 0;
    *twbr = brr;
}

uint8_t I2CBus::start()
{
    *twcr = RTWINT | RTWEN | RTWSTA;
    while (!(*twcr & RTWINT)) { }
    return *twsr;
}

uint8_t I2CBus::write(uint8_t data)
{
    *twdr = data;
    *twcr = RTWINT | RTWEN;
    while (!(*twcr & RTWINT)) { }
    return *twsr;
}

uint8_t I2CBus::read(uint8_t ack)
{
    *twcr = RTWINT | RTWEN | ((ack == 0) ? RTWEA : 0);
    while (!(*twcr & RTWINT)) { }
    return *twdr;
}

void I2CBus::scan()
{
    for (int i = 0; i < 128; i++)
    {
        start();
        uint8_t status = write(i << 1);
        stop();

        if (status == 0x18)
            slaves.push_back(i);
    }
}

void I2CBus::stop()
{
    *twcr = RTWINT | RTWEN | RTWSTO;
    while (!(*twcr & RTWSTO)) { }
}

void PCF8563::readTime()
{
    bus->start();
    bus->write(ADDR << 1);
    bus->write(0x0);
    bus->start();
    bus->write((ADDR << 1) | 1);
    regs.control_status_1 = bus->read(0);
    regs.control_status_2 = bus->read(0);
    regs.vl_seconds = bus->read(0);
    regs.minutes = bus->read(0);
    regs.hours = bus->read(0);
    regs.days = bus->read(0);
    regs.weekdays = bus->read(0);
    regs.century_months = bus->read(0);
    regs.years = bus->read(0);
    regs.minute_alarm = bus->read(0);
    bus->stop();
}


