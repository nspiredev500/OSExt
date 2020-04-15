#include "../kernel.h"


/*
static volatile uint32_t *value = (volatile uint32_t*) 0x90090000;
static volatile uint32_t *alarm = (volatile uint32_t*) 0x90090004;
static volatile uint32_t *set = (volatile uint32_t*) 0x90090008;
static volatile uint32_t *mask = (volatile uint32_t*) 0x9009000C;
static volatile uint32_t *maskedint = (volatile uint32_t*) 0x90090010;
static volatile uint32_t *status = (volatile uint32_t*) 0x90090014;
*/


/*
	remapped_rtc[0] = value
	remapped_rtc[1] = alarm
	remapped_rtc[2] = set
	remapped_rtc[3] = interrupt mask
	remapped_rtc[4] = masked interrupt status
	
	
*/


static volatile uint32_t *remapped_rtc = (volatile uint32_t*) 0xe9090000;



uint32_t rtc_get_value()
{
	return remapped_rtc[0];
}
void rtc_set_value(uint32_t newval)
{
	remapped_rtc[2] = newval;
}






bool rtc_irq_status()
{
	if ((remapped_rtc[4] & 0b1) == 0b1)
	{
		return true;
	}
	else
	{
		return false;
	}
}



void rtc_irq_clear()
{
	remapped_rtc[4] = 0xffffffff;
}



void rtc_claim()
{
	vic_enable(4);
	vic_set_fiq(4);
	
	remapped_rtc[3] |= 1;
}

void rtc_set_alarm(uint32_t alarm)
{
	remapped_rtc[1] = alarm;
}
uint32_t rtc_get_alarm()
{
	return remapped_rtc[1];
}






















