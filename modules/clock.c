
#include <stdbool.h>

#include <libndls.h>

#include "../include/module.h"
#include "../apis/graphics.h"

#include "../apis/digits.h"
#include "../apis/chars.h"
#include "../apis/rawdraw.h"
#include "clock/nclockfragments.h"



void checkKeys();
void drawclock();

static int clockx = 180,clocky = 1;
static volatile unsigned long *v_RTC = (volatile unsigned long*) 0x90090000;
static bool miniclock_enabled = true;
static int year = 0,month = 0,day = 0,hour = 0,minute = 0,second = 0;

long last = 0;




static Graphics *clockg;


int main(int argc, char **argv)
{
	initModule(argv);
	clockg = createGraphics();
	
	addFunction((unsigned int) checkKeys);
	addDrawFunction((unsigned int) drawclock);
	
	
	
	
	
	
}


void checkKeys()
{
	
	
	
	
	
	
}


void drawclock()
{
	//uart_printf("clock\n");
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
	
	
	volatile int *powp = 0x900B0018;
	//uart_printf("powp: %x\n",*powp);
	
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


































