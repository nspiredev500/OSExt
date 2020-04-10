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
	bool reset = watchdog_is_reset();
	watchdog_disable();
	watchdog_set_reset(false);
	uint32_t value = watchdog_value();
	uint32_t function = watchdog_function();
	
	watchdog_set_function(WATCHDOG_WAIT);
	
	
	watchdog_set_period(milis);
	watchdog_enable();
	while (watchdog_enabled())
		wait_for_interrupt();
	watchdog_disable();
	
	
	
	
	watchdog_set_function(function);
	watchdog_set_value(value);
	if (reset)
	{
		watchdog_set_reset(true);
	}
	if (enabled)
	{
		watchdog_enable();
	}
	
	
}






