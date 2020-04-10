#include "../kernel.h"



// TODO use remapped registers

/*
static volatile uint32_t *value = (volatile uint32_t*) 0x90090000;
static volatile uint32_t *alarm = (volatile uint32_t*) 0x90090004;
static volatile uint32_t *set = (volatile uint32_t*) 0x90090008;
static volatile uint32_t *mask = (volatile uint32_t*) 0x9009000C;
static volatile uint32_t *maskedint = (volatile uint32_t*) 0x90090010;
static volatile uint32_t *status = (volatile uint32_t*) 0x90090014;
*/


static volatile uint32_t *remapped_rtc = (volatile uint32_t*) 0xe9090000;



uint32_t getRTCValue()
{
	return remapped_rtc[0];
}
void setRTCValue(uint32_t newval)
{
	remapped_rtc[2] = newval;
}































