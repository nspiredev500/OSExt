#include "../kernel.h"

static volatile uint32_t *remapped_power = (uint32_t*) 0xe90b0000;
/*
	remapped_power[0] = clock speed load
	remapped_power[1] = low power interrupt mask
	remapped_power[2] = low power mode interrupt and acknowledge
	remapped_power[3] = clock speed control and low power mode. write 0x3a to enter low power mode
	remapped_power[6] = bus enable register (2 bytes)
	remapped_power[6] = bus enable register (2 bytes)
	// maybe using it as a 4 byte register is bad?
	
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









































