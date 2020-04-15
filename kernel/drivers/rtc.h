#ifndef RTC_H
#define RTC_H



uint64_t rtc_peripheral_id();

uint32_t rtc_get_value();
void rtc_set_value(uint32_t value);

void rtc_disable_interrupt();
void rtc_enable_interrupt();

bool rtc_irq_status();
void rtc_irq_clear();



void rtc_claim();



void rtc_set_alarm(uint32_t alarm);
uint32_t rtc_get_alarm();
























#endif