#include "../kernel.h"

static uint16_t bswap16(uint16_t s)
{
	return ((s & 0xFF) << 8) | ((s & 0xFF00) >> 8);
}

static uint16_t maxx = 0xff, maxy = 0xff;





void touchpad_get_report(struct touchpad_report *rep)
{
	uint16_t buffreal[6];
	uint8_t *buff = (uint8_t*) buffreal; // to guarantee 2 ybte alignment
	//i2c_read_port(buff,11,0);
	i2c_set_port(0);
	i2c_read_multiple(buff,11);
	if (buff[0] == 1)
	{
		rep->contact = true;
	}
	else
	{
		rep->contact = false;
	}
	rep->proximity = buff[1];
	rep->x = bswap16(*((uint16_t*) (buff+2)));
	rep->y = bswap16(*((uint16_t*) (buff+4)));
	rep->xrel = buff[6];
	rep->yrel = buff[7];
	if (buff[10] == 1)
	{
		rep->pressed = true;
	}
	else
	{
		rep->pressed = false;
	}
}



/*
bool touchpad_contact()
{
	uint8_t cont = 0;
	i2c_read_port(&cont,1,0);
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
	uint8_t prox = 0;
	i2c_read_port(&prox,1,1);
	return prox;
}

uint16_t touchpad_x_abs()
{
	uint16_t x = 0;
	i2c_read_port((uint8_t*) (&x),2,2);
	x = bswap16(x);
	return x;
}

uint16_t touchpad_y_abs()
{
	uint16_t y = 0;
	i2c_read_port((uint8_t*) (&y),2,4);
	y = bswap16(y);
	return y;
}

uint8_t touchpad_x_rel()
{
	uint8_t x = 0;
	i2c_read_port(&x,1,6);
	return x;
	
}

uint8_t touchpad_y_rel()
{
	uint8_t y = 0;
	i2c_read_port(&y,1,7);
	return y;
}

bool touchpad_pressed()
{
	uint8_t press = 0;
	i2c_read_port(&press,1,0xa);
	if (press == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
*/

uint8_t touchpad_status()
{
	uint8_t status = 0;
	//i2c_read_port(&status,1,0xb);
	i2c_set_port(0xb);
	status = i2c_read();
	return status;
}

/*
uint32_t touchpad_firmware()
{
	uint32_t firmware = 0;
	i2c_read_port((uint8_t*) &firmware,4,0xe4);
	return firmware;
}
*/














/*
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
*/




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



bool touchpad_is_arrow(enum touchpad_arrow arrow,struct touchpad_report *rep)
{
	uint32_t fx = touchpad_max_x()/8;
	uint32_t fy = touchpad_max_y()/8;
	if (! (rep->pressed))
		return false;
	switch (arrow)
	{
	case 0: // up
		if (rep->y >= touchpad_max_y()-fy)
		{
			return true;
		}
		break;
	case 1: // right
		if (rep->x >= touchpad_max_x()-fx)
		{
			return true;
		}
		break;
	case 2: // down
		if (rep->y <= fy)
		{
			return true;
		}
		break;
	case 3: // left
		if (rep->x <= fx)
		{
			return true;
		}
		break;
	}
	return false;
}










































