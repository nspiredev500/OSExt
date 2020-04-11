#include "../kernel.h"

static uint16_t bswap16(uint16_t s)
{
	return ((s & 0xFF) << 8) | ((s & 0xFF00) >> 8);
}

static uint16_t maxx = 0xff, maxy = 0xff;


bool touchpad_contact()
{
	i2c_set_port(0x0);
	uint8_t cont = i2c_read();
	if (cont == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}



uint8_t touchpad_proximity()
{
	i2c_set_port(0x1);
	return i2c_read();
}


uint16_t touchpad_x_abs()
{
	i2c_set_port(0x2);
	uint16_t x = 0;
	i2c_read_multiple(&x,2);
	x = bswap16(x);
	return x;
}


uint16_t touchpad_y_abs()
{
	i2c_set_port(0x4);
	uint16_t y = 0;
	i2c_read_multiple(&y,2);
	y = bswap16(y);
	return y;
}


uint8_t touchpad_x_rel()
{
	i2c_set_port(0x6);
	return i2c_read();
}


uint8_t touchpad_y_rel()
{
	i2c_set_port(0x7);
	return i2c_read();
}


bool touchpad_pressed()
{
	i2c_set_port(0xa);
	uint8_t press = i2c_read();
	if (press == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}


uint8_t touchpad_status()
{
	i2c_set_port(0xb);
	return i2c_read();
}


uint32_t touchpad_firmware()
{
	uint32_t firmware = 0;
	i2c_set_port(0xe4);
	i2c_read_multiple(&firmware,4);
	return firmware;
}





uint16_t touchpad_max_x()
{
	return 0x0918;
	//doesn't work on hardware, lets the os halt
	/*
	if (maxx != 0xff)
	{
		return maxx;
	}
	bool irq = false;
	if (isIRQ())
	{
		irq = true;
		disableIRQ();
	}
	i2c_set_port(0xff);
	i2c_write(0x10);
	
	
	
	i2c_set_port(0x4);
	uint16_t xmax = 0;
	i2c_read_multiple(&xmax,2);
	xmax = bswap16(xmax);
	
	
	i2c_set_port(0xff);
	i2c_write(0x4);
	if (irq)
	{
		enableIRQ();
	}
	maxx = xmax;
	return xmax;
	*/
}


uint16_t touchpad_max_y()
{
	return 0x069B;
	//doesn't work on hardware, lets the os halt
	/*
	if (maxy != 0xff)
	{
		return maxy;
	}
	bool irq = false;
	if (isIRQ())
	{
		irq = true;
		disableIRQ();
	}
	i2c_set_port(0xff);
	i2c_write(0x10);
	
	i2c_set_port(0x6);
	uint16_t ymax = 0;
	i2c_read_multiple(&ymax,2);
	ymax = bswap16(ymax);
	
	i2c_set_port(0xff);
	i2c_write(0x4);
	if (irq)
	{
		enableIRQ();
	}
	maxy = ymax;
	return ymax;
	*/
}



bool touchpad_is_arrow(enum toucpad_arrow arrow)
{
	uint32_t fx = touchpad_max_x()/5;
	uint32_t fy = touchpad_max_y()/5;
	if (! touchpad_pressed())
		return false;
	switch (arrow)
	{
	case 0: // up
		if (touchpad_y_abs() >= touchpad_max_y()-fy)
		{
			return true;
		}
		break;
	case 1: // right
		if (touchpad_x_abs() >= touchpad_max_x()-fx)
		{
			return true;
		}
		break;
	case 2: // down
		if (touchpad_y_abs() <= fy)
		{
			return true;
		}
		break;
	case 3: // left
		if (touchpad_x_abs() <= fx)
		{
			return true;
		}
		break;
	}
	return false;
}











































