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
static uint32_t function = 0;
/*
	what function the watchdog timer currently serves in the kernel
	0 = reset, kernel panic if fired
	1 = wait, fiq handler just returns if in priviledged mode, returns to svc mode if in usr mode, timer is disabled
*/


uint32_t watchdog_function()
{
	return function;
}

void watchdog_set_function(uint32_t func)
{
	function = func;
}

void watchdog_enable()
{
	vic_set_fiq(3);
	vic_enable(3);
	power_enable_device(13);
	remapped_watchdog[0x300] = 0x1ACCE551;
	remapped_watchdog[2] = (remapped_watchdog[2] & (~0b11)) | 0b1;
	if (watchdog_reset)
	{
		remapped_watchdog[2] = (remapped_watchdog[2] & (~0b11)) | 0b11;
	}
	remapped_watchdog[0x300] = 0;
}

bool watchdog_enabled()
{
	power_enable_device(13);
	if ((remapped_watchdog[2] & 0b1) == 0b1)
		return true;
	else
		return false;
}


void watchdog_disable()
{
	vic_disable(3);
	vic_set_irq(3);
	power_enable_device(13);
	remapped_watchdog[0x300] = 0x1ACCE551;
	remapped_watchdog[2] = (remapped_watchdog[2] & (~0b11));
	remapped_watchdog[0x300] = 0;
}

uint32_t watchdog_value()
{
	power_enable_device(13);
	return remapped_watchdog[1];
}

void watchdog_set_value(uint32_t value)
{
	power_enable_device(13);
	remapped_watchdog[0x300] = 0x1ACCE551;
	remapped_watchdog[0] = value;
	remapped_watchdog[0x300] = 0;
}

uint32_t watchdog_load()
{
	power_enable_device(13);
	return remapped_watchdog[0];
}

void watchdog_set_period(uint32_t milis)
{
	power_enable_device(13);
	remapped_watchdog[0x300] = 0x1ACCE551;
	remapped_watchdog[0] = 33000*milis;
	remapped_watchdog[0x300] = 0;
}

bool watchdog_irq_status()
{
	power_enable_device(13);
	if (remapped_watchdog[5] == 1)
		return true;
	else
		return false;
}
void watchdog_irq_clear()
{
	power_enable_device(13);
	remapped_watchdog[0x300] = 0x1ACCE551;
	remapped_watchdog[3] = 0xffffffff;
	remapped_watchdog[0x300] = 0;
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
	power_enable_device(13);
	if (reset)
	{
		watchdog_reset = true;
		remapped_watchdog[0x300] = 0x1ACCE551;
		remapped_watchdog[2] = (remapped_watchdog[2] & (~0b11)) | 0b10;
		remapped_watchdog[0x300] = 0;
	}
	else
	{
		watchdog_reset = false;
		remapped_watchdog[0x300] = 0x1ACCE551;
		remapped_watchdog[2] = (remapped_watchdog[2] & (~0b10));
		remapped_watchdog[0x300] = 0;
	}
}

































