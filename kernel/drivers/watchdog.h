#ifndef WATCHDOG_H
#define WATCHDOG_H








void watchdog_enable();
void watchdog_disable();

void watchdog_set_period(uint32_t milis);

bool watchdog_is_oneshot();
void watchdog_set_oneshot(bool oneshot);

bool watchdog_is_reset();
void watchdog_set_reset(bool oneshot);




bool watchdog_irq_status();
void watchdog_irq_clear();























#endif