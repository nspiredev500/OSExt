#include "../kernel.h"




static volatile uint32_t *remapped_watchdog = (volatile uint32_t*) 0xe9060000;
// runs at 33 MHz
/*
	remapped_watchdog[0] = load register
	remapped_watchdog[1] = value register
	remapped_watchdog[2] = control regiser
	remapped_watchdog[3] = interrupt clear register
	remapped_watchdog[4] = raw interrupt status register
	remapped_watchdog[5] = masked interrupt status register
	remapped_watchdog[300] = lock register
*/


static bool watchdog_oneshot = false;
static bool watchdog_reset = false;


void watchdog_enable()
{
	vic_enable(3);
	remapped_watchdog[300] = 0x1ACCE551;
	remapped_watchdog[2] = (remapped_watchdog[2] & (~0b11)) | 0b1;
	if (watchdog_reset)
	{
		remapped_watchdog[2] = (remapped_watchdog[2] & (~0b11)) | 0b11;
	}
	remapped_watchdog[300] = 0;
}


void watchdog_disable()
{
	vic_disable(3);
	remapped_watchdog[300] = 0x1ACCE551;
	remapped_watchdog[2] = (remapped_watchdog[2] & (~0b11));
	remapped_watchdog[300] = 0;
}



void watchdog_set_period(uint32_t milis)
{
	remapped_watchdog[300] = 0x1ACCE551;
	remapped_watchdog[0] = 33000*milis;
	remapped_watchdog[300] = 0;
}

bool watchdog_irq_status()
{
	if (remapped_watchdog[5] == 1)
		return true;
	else
		return false;
}
void watchdog_irq_clear()
{
	remapped_watchdog[300] = 0x1ACCE551;
	remapped_watchdog[3] = 0xffffffff;
	remapped_watchdog[300] = 0;
}


bool watchdog_is_oneshot()
{
	return watchdog_oneshot;
}


void watchdog_set_oneshot(bool oneshot)
{
	watchdog_oneshot = oneshot;
}


bool watchdog_is_reset()
{
	return watchdog_reset;
}


void watchdog_set_reset(bool reset)
{
	if (reset)
	{
		watchdog_reset = true;
		remapped_watchdog[300] = 0x1ACCE551;
		remapped_watchdog[2] = (remapped_watchdog[2] & (~0b11)) | 0b10;
		remapped_watchdog[300] = 0;
	}
	else
	{
		watchdog_reset = false;
		remapped_watchdog[300] = 0x1ACCE551;
		remapped_watchdog[2] = (remapped_watchdog[2] & (~0b10));
		remapped_watchdog[300] = 0;
	}
}

































