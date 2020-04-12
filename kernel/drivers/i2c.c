#include "../kernel.h"

static volatile uint32_t *remapped_i2c = (uint32_t*) 0xe9050000;
/*
	from firebird source:
	remapped_i2c[4] = command register
					write 0x100 to go into read mode (for 1 byte)
					write after reset to set the port
					write any 8-bit sequence to write to the touchpad port
					
					read to read the current port (decrements read mode)
					
					
					
	
	
	0x10 = [4]
	0x38 = [14]
	0x3c = [15]
	0x70 = [28]
	
	remapped_i2c[14] = reset/ port set mode
	
	
	
	
	
	
	
*/






/*
void i2c_read_port(uint8_t *buffer, uint32_t length, uint32_t port)
{
	//i2c_set_port(port);
	//asm(".long 0xE1212374"); // bkpt
	remapped_i2c[14] = 0xb0b0b0b;
	remapped_i2c[15] = 0xc0c0c0c;
	remapped_i2c[4] = ((port & 0xff) | ((port & 0xff) << 16));
	const uint32_t mask = 0x1000100;
	for (uint32_t i = 0;i<length;i++)
	{
		remapped_i2c[4] = mask | ((port & 0xff) + i) | (((port & 0xff) + i) << 16);
	}
	for (uint32_t i = 0;i<length;i++)
	{
		buffer[i] = remapped_i2c[4];
	}
}


void i2c_write_port(uint8_t *buffer, uint32_t length, uint32_t port)
{
	//i2c_set_port(port);
	remapped_i2c[14] = 0x1010101;
	remapped_i2c[15] = 0x2020202;
	remapped_i2c[4] = (port & 0xff) | ((port & 0xff) << 16);
	for (uint32_t i = 0;i<length;i++)
	{
		remapped_i2c[4] = buffer[i] | (buffer[i] << 16);
	}
}
*/





void i2c_set_port(uint32_t port)
{
	if (port > 0xff)
		return;
	remapped_i2c[14] = 0x0;
	remapped_i2c[4] = (port & 0xff);
}


uint8_t i2c_read()
{
	remapped_i2c[4] = 0x100;
	return remapped_i2c[4];
}

void i2c_write(uint8_t value)
{
	remapped_i2c[4] = ((uint32_t) value) & 0xff;
}





void i2c_read_multiple(void* buffer,uint32_t length)
{
	uint8_t *b = buffer;
	for (uint32_t i = 0;i<length;i++)
	{
		remapped_i2c[4] = 0x100;
		b[i] = remapped_i2c[4];
	}
}
void i2c_write_multiple(void* buffer,uint32_t length)
{
	uint8_t *b = buffer;
	for (uint32_t i = 0;i<length;i++)
	{
		remapped_i2c[4] = b[i];
	}
}

























