#include "uip_timer.h"

clock_time_t clock_time(void)
{
    return (clock_time_t)millis();
}

void uip_timer_set(struct uip_timer *t, clock_time_t interval)
{
    t->interval = interval;
    t->start = clock_time();
}

void uip_timer_reset(struct uip_timer *t)
{
    t->start += t->interval;
}

void uip_timer_restart(struct uip_timer *t)
{
    t->start = clock_time();
}

int uip_timer_expired(struct uip_timer *t)
{
    return (clock_time_t)(clock_time() - t->start) >= (clock_time_t)t->interval;
}


