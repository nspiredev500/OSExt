#include "kernel.h"


static volatile uint32_t *value = (volatile uint32_t*) 0x90090000;
static volatile uint32_t *alarm = (volatile uint32_t*) 0x90090004;
static volatile uint32_t *set = (volatile uint32_t*) 0x90090008;
static volatile uint32_t *mask = (volatile uint32_t*) 0x9009000C;
static volatile uint32_t *maskedint = (volatile uint32_t*) 0x90090010;
static volatile uint32_t *status = (volatile uint32_t*) 0x90090014;



uint32_t getRTCValue()
{
	return *value;
}
void setRTCValue(uint32_t newval)
{
	*set = newval;
}































