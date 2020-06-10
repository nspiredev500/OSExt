#include "../kernel.h"

const uint32_t CHANNEL_VBATT = 1;
const uint32_t CHANNEL_VSYS = 2;
const uint32_t CHANNEL_B12 = 4;
const uint32_t CHANNEL_KEYPAD = 3;

static volatile uint32_t *remapped_adc = (volatile uint32_t*) (0xe9400000);
/*
	remapped_adc[0] = masked interrupt status
	remapped_adc[1] = interrupt status/acknowledge
	remapped_adc[2] = interrupt enable register
	
	
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
	//TODO: in firebird the adc seems to be device 4 of power2
	
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
	
	uint32_t prev_cmd = *channel_command;
	
	*channel_command = 0b1;
	msleep(50); // I don't know which are the right interrupt status bits, so do a sleep. 50 milliseconds should be enough, and this shouldn't be done often
	
	// the interrupt bits don't seem to work on hardware
	/*
	while (((remapped_adc[1] >> (4*channel)) & 0b11) != 0b11)
	{
		msleep(1);
	}
	*/
	//debug_shell_println("ADC interrupt bits: 0x%x",remapped_adc[1]);
	//debug_shell_println("ADC masked interrupt bits: 0x%x",remapped_adc[0]);
	uint32_t read = *channel_read;
	//DEBUGPRINTLN_1("adc channel read: %d   %d units  commannd: 0x%x   read: 0x%x powered: %d irq: %d",channel,read,channel_command,channel_read,(uint32_t) powered,(uint32_t) irq)
	
	
	*channel_command = prev_cmd;
	msleep(50);
	remapped_adc[1] = 0xffffffff;
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











































