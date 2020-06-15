#include "../kernel.h"


// TODO use remapped registers when available


volatile void** LCD_UPBASE = (volatile void**) 0xC0000010;

// gets locked if the kernel transfers control over the lcd framebuffer to itself
static uint32_t *kernel_lcd_mutex = NULL;
static volatile void *framebuffer1 = NULL;
static volatile void *framebuffer2 = NULL;

static void *old_framebuffer = NULL;




uint16_t rgbto565(uint32_t r,uint32_t g,uint32_t b);


// false if lcd framebuffer is pointing to the ti framebuffer
static bool own_framebuffer = false;

void initLCDDriver()
{
	kernel_lcd_mutex = request4Bytes();
	unlockMutex(kernel_lcd_mutex);
	
	framebuffer1 = requestLCDFramebuffer();
	framebuffer2 = requestLCDFramebuffer();
	
	
}
void remappLCD(void* address)
{
	LCD_UPBASE = (volatile void**) ((uint32_t) address+0x10);
	
	
}

void* get_old_framebuffer_address()
{
	return (void*) old_framebuffer;
}

void* get_back_framebuffer_address()
{
	return (void*) framebuffer2;
}

void* get_front_framebuffer_address()
{
	return (void*) framebuffer1;
}

void claimLCD()
{
	if (own_framebuffer)
	{
		return;
	}
	own_framebuffer = true;
	old_framebuffer = (void*) *LCD_UPBASE;
	k_memset(framebuffer1,0,320*240*2);
	k_memset(framebuffer2,0,320*240*2);
	
	
	//framebuffer_fillrect(framebuffer1,0,0,320,240,0,0,255);
	
	*LCD_UPBASE = (volatile void*) getKernelPhysicalAddress((void*) framebuffer1);
	
	
	
	clear_caches();
}


void freeLCD()
{
	if (! own_framebuffer)
	{
		return;
	}
	*LCD_UPBASE = old_framebuffer;
	
	
	own_framebuffer = false;
}

uint16_t rgbto565(uint32_t r,uint32_t g,uint32_t b)
{
	if (! (r>255|| g>255|| b>255))
	{
		return ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
	}
	return 0;
}

void blitLCDBuffer()
{
	if (! own_framebuffer)
	{
		return;
	}
	k_memcpy(framebuffer1,framebuffer2,320*240*2);
}

void lcd_setpixel(uint32_t x,uint32_t y,uint32_t r, uint32_t g,uint32_t b)
{
	if (! own_framebuffer)
	{
		return;
	}
	uint16_t colour565 = rgbto565(r,g,b);
	// add cases for other screen versions
	if (x < 320 && y < 240)
	{
		volatile uint16_t *buff = framebuffer2;
		buff[x*240+y] = colour565;
		return;
	}
	
	
	
}
void lcd_fillrect(uint32_t xs,uint32_t ys, uint32_t w, uint32_t h,uint32_t r, uint32_t g,uint32_t b)
{
	for (uint32_t x = xs;x<xs+w;x++)
	{
		for (uint32_t y = ys;y<ys+h;y++)
		{
			lcd_setpixel(x,y,r,g,b);
		}
	}
}

void lcd_write10pchar(int xx,int yy,uint32_t r, uint32_t g,uint32_t b,int charn)
{
	for (int x = 0;x<10;x++)
	{
		for (int y = 0;y<10;y++)
		{
			if (isCharPixel(x,y,charn))
			{
				lcd_setpixel(xx+x,yy+y,r,g,b);
			}
		}
	}
}

void lcd_write10pstring(char* string,int xx,int yy,uint32_t r, uint32_t g,uint32_t b)
{
	char *cchar = string;
	while (*cchar != '\0')
	{
		bool digit = false;
		uint32_t charn = 0;
		if ((*cchar >= 65 && *cchar <= 90))
		{
			charn = *cchar - 65;
		}
		if ((*cchar >= 97 && *cchar <= 122))
		{
			charn = *cchar - 97;
		}
		if ((*cchar >= 48 && *cchar <= 57))
		{
			digit = true;
			charn = *cchar - 48;
		}
		for (int x = 0;x<10;x++)
		{
			for (int y = 0;y<10;y++)
			{
				if (digit)
				{
					if (isDigitPixel(x,y,charn))
					{
						lcd_setpixel(xx+x,yy+y,r,g,b);
					}
				}
				else
				{
					if (isCharPixel(x,y,charn))
					{
						lcd_setpixel(xx+x,yy+y,r,g,b);
					}
				}
			}
		}
		cchar++;
		xx += 10;
	}
}

void framebuffer_setpixel(void *buff,uint32_t x,uint32_t y,uint32_t r, uint32_t g,uint32_t b)
{
	uint16_t colour565 = rgbto565(r,g,b);
	// add cases for other screen versions
	if (x < 320 && y < 240)
	{
		uint16_t* buffer = buff;
		buffer[x*240+y] = colour565;
		return;
	}
}

void framebuffer_setpixel565(void *buff,uint32_t x,uint32_t y,uint16_t rgb565)
{
	// add cases for other screen versions
	if (x < 320 && y < 240)
	{
		uint16_t* buffer = buff;
		buffer[x*240+y] = rgb565;
		return;
	}
}

void framebuffer_draw_img565(void *buff,struct img565* img,uint32_t xs,uint32_t ys)
{
	for (uint32_t x = 0;x<img->width;x++)
	{
		for (uint32_t y = 0;y<img->height;y++)
		{
			framebuffer_setpixel565(buff,xs+x,ys+y,img->data[x+y*img->width]);
		}
	}
}

void framebuffer_fillrect(void *buff,uint32_t xs,uint32_t ys, uint32_t w, uint32_t h,uint32_t r, uint32_t g,uint32_t b)
{
	for (uint32_t x = xs;x<xs+w;x++)
	{
		for (uint32_t y = ys;y<ys+h;y++)
		{
			framebuffer_setpixel(buff,x,y,r,g,b);
		}
	}
}

void framebuffer_drawrect(void *buff,uint32_t xs,uint32_t ys, uint32_t w, uint32_t h,uint32_t r, uint32_t g,uint32_t b)
{
	for (uint32_t x = xs;x<xs+w;x++)
	{
		framebuffer_setpixel(buff,x,ys,r,g,b);
		framebuffer_setpixel(buff,x,ys+h-1,r,g,b);
	}
	for (uint32_t y = ys;y<ys+h;y++)
	{
		framebuffer_setpixel(buff,xs,y,r,g,b);
		framebuffer_setpixel(buff,xs+w-1,y,r,g,b);
	}
}

void framebuffer_write10pchar(void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b,int charn)
{
	for (int x = 0;x<10;x++)
	{
		for (int y = 0;y<10;y++)
		{
			if (isCharPixel(x,y,charn))
			{
				framebuffer_setpixel(buff,xx+x,yy+y,r,g,b);
			}
		}
	}
}

void framebuffer_write10pdigit(void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b,int charn)
{
	for (int x = 0;x<10;x++)
	{
		for (int y = 0;y<10;y++)
		{
			if (isDigitPixel(x,y,charn))
			{
				framebuffer_setpixel(buff,xx+x,yy+y,r,g,b);
			}
		}
	}
}

void framebuffer_write10pstring(char* string,void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b)
{
	char *cchar = string;
	while (*cchar != '\0')
	{
		bool digit = false;
		uint32_t charn = 0;
		if ((*cchar >= 65 && *cchar <= 90))
		{
			charn = *cchar - 65;
		}
		if ((*cchar >= 97 && *cchar <= 122))
		{
			charn = *cchar - 97;
		}
		if ((*cchar >= 48 && *cchar <= 57))
		{
			digit = true;
			charn = *cchar - 48;
		}
		//DEBUGPRINTLN_1("cchar: %d",(uint32_t) *cchar)
		for (int x = 0;x<10;x++)
		{
			for (int y = 0;y<10;y++)
			{
				if (digit)
				{
					if (isDigitPixel(x,y,charn))
					{
						framebuffer_setpixel(buff,xx+x,yy+y,r,g,b);
					}
				}
				else
				{
					if (isCharPixel(x,y,charn))
					{
						framebuffer_setpixel(buff,xx+x,yy+y,r,g,b);
					}
				}
			}
		}
		cchar++;
		xx += 10;
	}
}

void framebuffer_write10pstring_ascii(char* string,void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b)
{
	char *cchar = string;
	while (*cchar != '\0')
	{
		for (int x = 0;x<10;x++)
		{
			for (int y = 0;y<10;y++)
			{
				
				if (isAsciiPixel(x,y,*cchar))
				{
					framebuffer_setpixel(buff,xx+x,yy+y,r,g,b);
				}
			}
		}
		cchar++;
		xx += 10;
	}
}












