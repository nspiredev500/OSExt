#include "../kernel.h"

static volatile uint32_t *remapped_i2c = (uint32_t*) 0xe9050000;
/*
	from firebird source:
	remapped_i2c[4] = command register
					write 0x100 to go into read mode (for 1 byte)
					write after reset to set the port
					write any 8-bit sequence to write to the touchpad port
					
					read to read the current port (decrements read mode)
					
					
					
	
	
	
	
	
	remapped_i2c[14] = reset/ port set mode
	
	
	
	
	
	
	
*/




void i2c_set_port(uint32_t port)
{
	if (port > 0xff)
		return;
	remapped_i2c[14] = 0;
	remapped_i2c[4] |= port & 0xff;
}

uint8_t i2c_read()
{
	remapped_i2c[4] |= 0x100;
	return remapped_i2c[4];
}

void i2c_write(uint8_t value)
{
	remapped_i2c[4] |= ((uint32_t) value) & 0xff;
}





void i2c_read_multiple(void* buffer,uint32_t length)
{
	uint8_t *b = buffer;
	for (uint32_t i = 0;i<length;i++)
	{
		remapped_i2c[4] |= 0x100;
		b[i] = remapped_i2c[4];
	}
}
void i2c_write_multiple(void* buffer,uint32_t length)
{
	uint8_t *b = buffer;
	for (uint32_t i = 0;i<length;i++)
	{
		remapped_i2c[4] |= b[i];
	}
}





































