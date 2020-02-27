#include "kernel.h"


volatile void** LCD_UPBASE = (volatile void**) 0xC0000014;

// gets locked if the kernel transfers control over the lcd framebuffer to itself
static uint32_t *kernel_lcd_mutex = NULL;
static void *framebuffer1 = NULL;
static void *framebuffer2 = NULL;

static void *old_framebuffer = NULL;


// false if lcd framebuffer is pointing to the ti framebuffer
static bool own_framebuffer = false;

void initLCDDriver()
{
	kernel_lcd_mutex = request4Bytes();
	unlockMutex(kernel_lcd_mutex);
	
	
	
	
}


void claimLCD()
{
	if (own_framebuffer)
	{
		return;
	}
	own_framebuffer = true;
	old_framebuffer = (void*) *LCD_UPBASE;
	framebuffer1 = requestLCDFramebuffer();
	framebuffer2 = requestLCDFramebuffer();
	k_memset(framebuffer1,0,320*240*2);
	k_memset(framebuffer2,0,320*240*2);
	
	*LCD_UPBASE = framebuffer1;
	
	
}


void freeLCD()
{
	if (! own_framebuffer)
	{
		return;
	}
	*LCD_UPBASE = old_framebuffer;
	freeLCDFramebuffer(framebuffer1);
	freeLCDFramebuffer(framebuffer2);
	
	
	own_framebuffer = false;
}

static uint16_t rgbto565(uint32_t r,uint32_t g,uint32_t b)
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
		uint16_t *buff = framebuffer2;
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

void lcd_write10pchar(int xx,int yy,uint32_t r, uint32_t g,uint32_t b,int charn,char c[][10][10])
{
	for (int x = 0;x<10;x++)
	{
		for (int y = 0;y<10;y++)
		{
			if (c[charn][y][x] == 1)
			{
				lcd_setpixel(xx+x,yy+y,r,g,b);
			}
		}
	}
}

void lcd_write10pstring(char* string,int xx,int yy,uint32_t r, uint32_t g,uint32_t b,char c[][10][10],char d[][10][10])
{
	char *cchar = string;
	while (*cchar != '\0')
	{
		char *images = NULL;
		uint32_t charn = 0;
		if ((*cchar >= 65 && *cchar <= 90))
		{
			images = (char*) c;
			charn = *cchar - 65;
		}
		if ((*cchar >= 97 && *cchar <= 122))
		{
			images = (char*) c;
			charn = *cchar - 97;
		}
		if ((*cchar >= 48 && *cchar <= 57))
		{
			images = (char*) d;
			charn = *cchar - 48;
		}
		if (images == NULL)
		{
			cchar++;
			xx += 10;
			continue;
		}
		for (int x = 0;x<10;x++)
		{
			for (int y = 0;y<10;y++)
			{
				if (c[charn][y][x] == 1)
				{
					lcd_setpixel(xx+x,yy+y,r,g,b);
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


void framebuffer_write10pchar(void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b,int charn,char c[][10][10])
{
	for (int x = 0;x<10;x++)
	{
		for (int y = 0;y<10;y++)
		{
			if (c[charn][y][x] == 1)
			{
				framebuffer_setpixel(buff,xx+x,yy+y,r,g,b);
			}
		}
	}
}

void framebuffer_write10pstring(char* string,void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b,char c[][10][10],char d[][10][10])
{
	char *cchar = string;
	while (*cchar != '\0')
	{
		char (*images)[10][10] = NULL;
		uint32_t charn = 0;
		if ((*cchar >= 65 && *cchar <= 90))
		{
			images = c;
			charn = *cchar - 65;
		}
		if ((*cchar >= 97 && *cchar <= 122))
		{
			images = c;
			charn = *cchar - 97;
		}
		if ((*cchar >= 48 && *cchar <= 57))
		{
			images = d;
			charn = *cchar - 48;
		}
		if (images == NULL)
		{
			cchar++;
			xx += 10;
			continue;
		}
		//DEBUGPRINTLN_1("cchar: %d",(uint32_t) *cchar)
		for (int x = 0;x<10;x++)
		{
			for (int y = 0;y<10;y++)
			{
				if (images[charn][y][x] == 1)
				{
					framebuffer_setpixel(buff,xx+x,yy+y,r,g,b);
				}
			}
		}
		cchar++;
		xx += 10;
	}
}

void framebuffer_write10pstring_ascii(char* string,void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b,char c[128][10][10])
{
	char *cchar = string;
	while (*cchar != '\0')
	{
		for (int x = 0;x<10;x++)
		{
			for (int y = 0;y<10;y++)
			{
				
				if (c[(uint32_t) *cchar][y][x] == 1)
				{
					framebuffer_setpixel(buff,xx+x,yy+y,r,g,b);
				}
			}
		}
		cchar++;
		xx += 10;
	}
}












