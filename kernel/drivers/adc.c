#include "../kernel.h"



static volatile uint32_t *remapped_adc = (volatile uint32_t*) (0xe9400000);
/*
	remapped_adc[1] = interrupt status/acknowledge
	
	remapped_adc+0x100+0x20*channel_number = command register
	remapped_adc+0x100+0x20*channel_number+0x10 = read register
	
*/


double adc_read_channel(uint32_t channel)
{
	if (channel > 5)
	{
		return 0;
	}
	
	bool powered = power_device_powered(4);
	
	if (! powered)
	{
		power_enable_device(4);
	}
	
	volatile uint32_t *channel_command = (volatile uint32_t*) (((void*) remapped_adc)+0x100+0x20*channel);
	volatile uint32_t *channel_read = (volatile uint32_t*) (((void*) remapped_adc)+0x100+0x20*channel+0x10);
	remapped_adc[1] = 0xffffffff;
	bool irq = isIRQ();
	if (irq)
	{
		disableIRQ();
	}
	
	
	
	*channel_command = 0b1;
	msleep(500); // I don't know which are the right interrupt status bits, so do a sleep. 0.5 seconds should be enough, and this shouldn't be done often
	
	remapped_adc[1] = 0xffffffff;
	uint32_t read = *channel_read;
	DEBUGPRINTLN_1("adc channel read: %d   %d units  commannd: 0x%x   read: 0x%x powered: %d irq: %d",channel,read,channel_command,channel_read,(uint32_t) powered,(uint32_t) irq)
	
	
	if (irq)
	{
		enableIRQ();
	}
	if (! powered)
	{
		power_disable_device(4);
	}
	if (channel == 1 || channel == 2)
	{
		return (double) (read) / (double) 155;
	}
	else
	{
		return (double) (read) / (double) 310;
	}
}











































