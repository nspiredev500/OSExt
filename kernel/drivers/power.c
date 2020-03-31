#include "../kernel.h"

static volatile uint32_t *remapped_power = (uint32_t*) 0xe90b0000;
/*
	remapped_power[6] = bus enable register
*/


bool power_device_powered(uint32_t device)
{
	if (((remapped_power[6] >> device) & 0b1) == 0b1)
		return false;
	else
		return true;
}




void power_enable_device(uint32_t device)
{
	uint32_t power = remapped_power[6];
	power &= ~(0b1 << device);
	remapped_power[6] = power;
}
void power_disable_device(uint32_t device)
{
	uint32_t power = remapped_power[6];
	power |= (0b1 << device);
	remapped_power[6] = power;
}









































