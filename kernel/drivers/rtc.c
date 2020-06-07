#include "../kernel.h"




/*
	if haskspire is right:
	remapped_rtc[0] = value
	remapped_rtc[1] = alarm
	remapped_rtc[2] = set
	remapped_rtc[3] = interrupt mask
	remapped_rtc[4] = masked interrupt status
	
	
	if arm documentation is right:
	remapped_rtc[0] = value
	remapped_rtc[1] = alarm
	remapped_rtc[2] = set
	remapped_rtc[3] = control
	remapped_rtc[4] = interrupt mask
	remapped_rtc[5] = raw interrupt status
	remapped_rtc[6] = masked interrupt status
	remapped_rtc[7] = interrupt clear
	
	remapped_rtc[1016-1023] = peripheral identification registers
	
*/


static volatile uint32_t *remapped_rtc = (volatile uint32_t*) 0xe9090000;

// id is the standard arm pl032 id
 
uint64_t rtc_peripheral_id()
{
	uint64_t id = 0;
	uint8_t *byte = (uint8_t*) &id;
	for (int i = 0;i<8;i++)
	{
		byte[i] = remapped_rtc[1016+i];
	}
	return id;
}


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
	//if ((remapped_rtc[4] & 0b1) == 0b1)
	if ((remapped_rtc[6] & 0b1) == 0b1)
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
	remapped_rtc[7] = 0b1;
	//remapped_rtc[4] = 0b1;
}

void rtc_disable_interrupt()
{
	remapped_rtc[4] = 0;
}
void rtc_enable_interrupt()
{
	remapped_rtc[4] = 1;
}




void rtc_claim()
{
	vic_enable(4);
	vic_set_fiq(4);
	
	remapped_rtc[3] = 0b1;
	remapped_rtc[4] = 0b1;
	
}

void rtc_set_alarm(uint32_t alarm)
{
	remapped_rtc[1] = alarm;
}
uint32_t rtc_get_alarm()
{
	return remapped_rtc[1];
}






















