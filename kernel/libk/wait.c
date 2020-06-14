#include "../kernel.h"




inline void wait_for_interrupt()
{
	asm volatile("mcr p15, 0, r0, c7, c0, 4":::"r0");
}




void msleep(uint32_t milis)
{
	if (milis == 0)
		return;
	bool enabled = watchdog_enabled();
	//bool reset = watchdog_is_reset();
	watchdog_disable();
	watchdog_set_reset(false);
	uint32_t load = watchdog_load();
	uint32_t function = watchdog_function();
	
	watchdog_set_function(WATCHDOG_WAIT);
	
	uint32_t overflows = (33000*milis);
	uint32_t first = overflows % 0xffffffff;
	overflows = overflows / 0xffffffff;
	
	
	//DEBUGPRINTLN_1("msleep first: %d, overflows: %d",first,overflows)
	watchdog_set_load(first);
	watchdog_enable();
	while (watchdog_enabled())
		wait_for_interrupt();
	
	for (uint32_t i = 0;i<overflows;i++)
	{
		watchdog_set_load(0xffffffff);
		watchdog_enable();
		while (watchdog_enabled())
			wait_for_interrupt();
	}
	
	
	watchdog_set_function(function);
	watchdog_set_load(load);
	if (enabled && vic_is_fiq(3)) // so it gets restarted for the Ndless lcdcompat feature
	{
		watchdog_enable();
	}
}

void microsleep(uint32_t micro)
{
	if (micro == 0)
		return;
	bool enabled = watchdog_enabled();
	//bool reset = watchdog_is_reset();
	watchdog_disable();
	watchdog_set_reset(false);
	uint32_t load = watchdog_load();
	uint32_t function = watchdog_function();
	
	watchdog_set_function(WATCHDOG_WAIT);
	
	uint32_t overflows = (33*micro);
	uint32_t first = overflows % 0xffffffff;
	overflows = overflows / 0xffffffff;
	
	
	//DEBUGPRINTLN_1("microsleep first: %d, overflows: %d",first,overflows)
	watchdog_set_load(first);
	watchdog_enable();
	while (watchdog_enabled())
		wait_for_interrupt();
	
	for (uint32_t i = 0;i<overflows;i++)
	{
		watchdog_set_load(0xffffffff);
		watchdog_enable();
		while (watchdog_enabled())
			wait_for_interrupt();
	}
	
	
	watchdog_set_function(function);
	watchdog_set_load(load);
	if (enabled && vic_is_fiq(3)) // so it gets restarted for the Ndless lcdcompat feature
	{
		watchdog_enable();
	}
}




