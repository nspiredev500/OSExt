#ifndef WATCHDOG_H
#define WATCHDOG_H


#define WATCHDOG_INVALID 0
#define WATCHDOG_WAIT 1

uint32_t watchdog_function();
void watchdog_set_function(uint32_t func);

bool watchdog_enabled();
void watchdog_enable();
void watchdog_disable();

uint32_t watchdog_value();
void watchdog_set_value(uint32_t value);
uint32_t watchdog_load();

void watchdog_set_period(uint32_t milis);

bool watchdog_is_oneshot();
void watchdog_set_oneshot(bool oneshot);

bool watchdog_is_reset();
void watchdog_set_reset(bool reset);




bool watchdog_irq_status();
void watchdog_irq_clear();























#endif