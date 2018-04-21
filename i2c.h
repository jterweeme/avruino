#ifndef _I2C_H_
#define _I2C_H_
#include "stream.h"

class I2CBus
{
protected:
    volatile uint8_t * const twbr;
    volatile uint8_t * const twsr;
    volatile uint8_t * const twar;
    volatile uint8_t * const twdr;
    volatile uint8_t * const twcr;

    static constexpr uint8_t
        TW_START = 0x08,
        BTWIE = 0,
        BTWEN = 2,
        BTWSTO = 4,
        BTWSTA = 5,
        BTWEA = 6,
        BTWINT = 7,
        RTWIE = 1<<BTWIE,
        RTWEN = 1<<BTWEN,
        RTWSTO = 1<<BTWSTO,
        RTWSTA = 1<<BTWSTA,
        RTWEA = 1<<BTWEA,
        RTWINT = 1<<BTWINT;
public:
    vector<uint8_t> slaves;
    I2CBus(unsigned brr = 72);
    uint8_t start();
    uint8_t restart();
    void stop();
    uint8_t write(uint8_t data);
    uint8_t read(uint8_t ack);
    void scan();
};

struct PCF8563_Regs
{
    uint8_t control_status_1;
    uint8_t control_status_2;
    uint8_t vl_seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t days;
    uint8_t weekdays;
    uint8_t century_months;
    uint8_t years;
    uint8_t minute_alarm;
    uint8_t hour_alarm;
    uint8_t day_alarm;
    uint8_t weekday_alarm;
    uint8_t clkout_control;
    uint8_t timer_control;
    uint8_t timer;
} __attribute__ ((packed));

class PCF8563
{
    I2CBus * const bus;
    PCF8563_Regs regs;
public:
    static const uint8_t ADDR = 0x51;
    PCF8563(I2CBus * const bus) : bus(bus) { }
    void readTime();
    uint8_t sec() const { return ((regs.vl_seconds >> 4) & 0x07) * 10 + (regs.vl_seconds & 0x0f); }
    uint8_t min() const { return ((regs.minutes >> 4) & 0x07) * 10 + (regs.minutes & 0x0f); }
    uint8_t hour() const { return ((regs.hours >> 4) & 0x03) * 10 + (regs.hours & 0x0f); }
};


#endif

