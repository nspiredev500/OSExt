#ifndef MINICLOCK_H
#define MINICLOCK_H

#include "../../apis/digits.h"
#include "../../apis/chars.h"
#include "../../apis/graphics.h"
#include "../../apis/rawdraw.h"
#include "nclockfragments.h"


#ifndef EXPERIMENTAL
static const unsigned int clock_hook_addrs1[] =
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x10023810, 0x100237d4,
 0x0, 0x0
};
#endif

#ifdef EXPERIMENTAL
static const unsigned int clock_hook_addrs1[] = //override write addresses
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x10023810, 0x10023808,
 0x0, 0x0
};
#endif

/*
const unsigned int clock_hook_addrs2[] =
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x10023840, 0x100237cc,
 0x0, 0x0
};


const unsigned int clock_hook_addrs3[] =
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x10023808,//0x10023790
 0x0, 0x0
};
*/






static int clockx = 180,clocky = 1;
static volatile unsigned long *v_RTC = (volatile unsigned long*) 0x90090000;
bool miniclock_enabled = true;
static int year = 0,month = 0,day = 0,hour = 0,minute = 0,second = 0;
//static uint16_t clockbuffer[240*320*2];





static Graphics *clockg;
#ifdef EXPERIMENTAL
	//uint16_t *os_scrbuf = (uint16_t*) 0x11349890;
#endif

HOOK_DEFINE(clockhook1) // TODO hook is only for HW W+, becaus it's apparently in the 240*320 compatibility code
{
	
	drawclock();
	
	
	
	
	
	#ifndef EXPERIMENTAL
		HOOK_RESTORE_RETURN(clockhook1);
	#else
		
		//saveOSGCToGraphics(clockg);
		
		//memcpy(REAL_SCREEN_BASE_ADDRESS,clockg->buffer,240*320*2);
		
		
		
		
		HOOK_RESTORE_RETURN(clockhook1);
	#endif
	
	
	
	
	
	
};


HOOK_DEFINE(clockhook2)
{
	drawclock();
	HOOK_RESTORE_RETURN(clockhook2);
};
HOOK_DEFINE(clockhook3)
{
	drawclock();
	HOOK_RESTORE_RETURN(clockhook2);
};

long last = 0;
void drawclock()
{
	//bkpt();
	if (! miniclock_enabled)
		return;
	long val = *v_RTC;
	
	
	initGraphics();
	saveScreenToGraphics(clockg);
	setGraphicsColor(clockg,0,0,0);
	fillRect(clockg,clockx,clocky,70,10);
	
	
	//fillRect(clockg,clockx-20,clocky,10,10);
	
	setGraphicsColor(clockg,255,255,255);
	drawRect(clockg,clockx-1,clocky-1,72,12);
	
	
	timestamp2date(val,&year,&month,&day,&hour,&minute,&second);
	
	setGraphicsColor(clockg,255,0,0);
	write10pChar(clockg,clockx+60,clocky,second%10,digits10p);
	write10pChar(clockg,clockx+50,clocky,(second/10)%10,digits10p);
	
	setPixel(clockg,clockx+47,clocky+3);
	setPixel(clockg,clockx+47,clocky+7);
	
	
	write10pChar(clockg,clockx+35,clocky,minute%10,digits10p);
	write10pChar(clockg,clockx+25,clocky,(minute/10)%10,digits10p);
	
	
	setPixel(clockg,clockx+23,clocky+3);
	setPixel(clockg,clockx+23,clocky+7);
	
	
	write10pChar(clockg,clockx+10,clocky,hour%10,digits10p);
	write10pChar(clockg,clockx,clocky,(hour/10)%10,digits10p);
	
	
	
	//write10pChar(clockg,clockx-20,clocky,(*timer)%10,digits10p);
	
	#ifdef USBTEST_H
		drawString10p(clockg,50,60,usbstring);
	#endif
	blitGraphicsToScreen(clockg);
	//bkpt();
	//blitGraphicsToOSGC(clockg);
	//bkpt();
	exitGraphics();
	
	
}



void settime()
{
	int y = 2019,m = 0,d = 0,h = 0,min = 0,s = 0;
	int clockselect = 0;
	// SDL is somehow not working in hardware
	/*
	SDL_Surface *screen;
	nSDL_Font *font;
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(320, 240, has_colors ? 16 : 8, SDL_SWSURFACE);
	font = nSDL_LoadFont(NSDL_FONT_TINYTYPE, 0, 0, 0);
	*/
	initGraphics();
	while (true)
	{
		if (isKeyPressed(KEY_NSPIRE_ESC))
		{
			break;
		}
		if (isKeyPressed(KEY_NSPIRE_LEFT))
		{
			if (clockselect > 0)
				clockselect--;
		}
		if (isKeyPressed(KEY_NSPIRE_RIGHT))
		{
			if (clockselect < 5)
				clockselect++;
		}
		if (isKeyPressed(KEY_NSPIRE_UP))
		{
			if (clockselect == 0 && y<2100)
				y++;
			if (clockselect == 1 && m<12)
				m++;
			if (clockselect == 2 && d<maxDayInMonth(m,isLeapYear(y)))
				d++;
			if (clockselect == 3 && h<24)
				h++;
			if (clockselect == 4 && min<60)
				min++;
			if (clockselect == 5 && s<60)
				s++;
		}
		if (isKeyPressed(KEY_NSPIRE_DOWN))
		{
			if (clockselect == 0 && y>1972)
				y--;
			if (clockselect == 1 && m>0)
				m--;
			if (clockselect == 2 && d>0)
				d--;
			if (clockselect == 3 && h>0)
				h--;
			if (clockselect == 4 && min>0)
				min--;
			if (clockselect == 5 && s>0)
				s--;
		}
		if (isKeyPressed(KEY_NSPIRE_ENTER))
		{
			//actually works on real hardware
			*(v_RTC+2) = date2timestamp(y,m,d,h,min,s);
			break;
		}
		// SDL is somehow not working in hardware
		//SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 255));
		setGraphicsColor(clockg,255,255,255);
		fillRect(clockg,0,0,320,240);
		setGraphicsColor(clockg,0,0,0);
		char fstring[80];
		sprintf(fstring,"%d:%d:%d:%d:%d:%d",y,m,d,h,min,s);
		switch (clockselect)
		{
			case 0:
			//nSDL_DrawString(screen, font,10,30,"y");
			drawString10p(clockg,10,30,"y");
			break;
			case 1:
			//nSDL_DrawString(screen, font,10,30,"m");
			drawString10p(clockg,10,30,"m");
			break;
			case 2:
			//nSDL_DrawString(screen, font,10,30,"d");
			drawString10p(clockg,10,30,"d");
			break;
			case 3:
			//nSDL_DrawString(screen, font,10,30,"h");
			drawString10p(clockg,10,30,"h");
			break;
			case 4:
			//nSDL_DrawString(screen, font,10,30,"min");
			drawString10p(clockg,10,30,"min");
			break;
			case 5:
			//nSDL_DrawString(screen, font,10,30,"s");
			drawString10p(clockg,10,30,"s");
			break;
		}
		// SDL is somehow not working in hardware
		//nSDL_DrawString(screen, font,10,10,fstring);
		//SDL_Flip(screen);
		drawString10p(clockg,10,10,fstring);
		blitGraphicsToScreen(clockg);
		msleep(100);
	}
	exitGraphics();
	//SDL_Quit();
}



int sledlength = 16;
uint32_t * sled = (uint32_t*) 0x10023808;
void hook_minicklock()
{
	clockg = createGraphics();
	#ifdef EXPERIMENTAL
		for (int i = 0;i<sledlength;i++)
		{
			*(sled+i) = 0xe2800000;
		}
		//*(sled+sledlength) = 0xe12fff1e;
		
		
		
		
		clear_cache();
		
	#endif
	
	
	HOOK_INSTALL(nl_osvalue(clock_hook_addrs1,32),clockhook1);
	#ifdef EXPERIMENTAL
		/*
		uint32_t * adr = (nl_osvalue(clock_hook_addrs1,32));
		*(adr+3) = 0xebfffffe; // as("bl 0x1007f5c0")
		*/
	#endif
	clear_cache();
	
	
	
}











#endif