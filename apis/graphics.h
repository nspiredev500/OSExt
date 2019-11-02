#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "rawdraw.h"
#include "digits.h"
#include "chars.h"
//uint16_t *OSGCBUFF = (uint16_t*) 0x11d067d8; // cx noncas 4.5
uint16_t *OSGCBUFF = 0x0;
static const unsigned int OSGCBUFF_adr[] =
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x11d067d8, 0x11d6e630, // currently not working in hardware
 0x0, 0x0
};
//noncas  11712960  10a3e250      10B9 5542    12C 8588
//cas  11778c90  10aa2fe4         5F 59A0      12C B64C‬

void initOSGCBUFF()
{
	OSGCBUFF = (uint16_t*) nl_osvalue(OSGCBUFF_adr,32);
}

void* resolveCharTo10pFont(char c);
typedef struct{
	uint16_t buffer[240*320*2]; // 240*320 somehow isn't enough for the whole screen
	uint16_t color;
	scr_type_t scr;
} Graphics;
void initGraphics()
{
	lcd_init(lcd_type());
}
void exitGraphics()
{
	lcd_init(SCR_TYPE_INVALID);
}
Graphics* createGraphics()
{
	Graphics *g = calloc(sizeof(Graphics),1);
	g->scr = lcd_type();
	return g;
}
void freeGraphics(Graphics *g)
{
	free(g);
}
void setGraphicsColor(Graphics *g,int r,int gg,int b)
{
	uint16_t c = rgbto565(r,gg,b);
	g->color = c;
}
void saveOSGCToGraphics(Graphics *g)
{
	for (int x = 0;x<320;x++)
	{
		for (int y = 0;y<260;y++)
		{
			*(g->buffer+y+x*240) = *(OSGCBUFF+y*320+x);
		}
	}
}
void saveScreenToGraphics(Graphics *g)
{
	memcpy(g->buffer,REAL_SCREEN_BASE_ADDRESS,240*320*2); // 240*320 somehow isn't the whole screen
}
void blitGraphicsToScreen(Graphics *g)
{
	lcd_blit(g->buffer,g->scr);
}
void blitGraphicsToOSGC(Graphics *g)
{
	for (int x = 0;x<320;x++)
	{
		for (int y = 0;y<260;y++)
		{
			*(OSGCBUFF+y*320+x) = *(g->buffer+y+x*240);
		}
	}
}
void setPixel(Graphics *g,int x,int y)
{
	setpixeldirect(g->buffer,g->scr,x,y,g->color);
}
void write10pChar(Graphics *g,int x,int y,int charn,char c[][10][10])
{
	write10pchardirect(g->buffer,g->scr,x,y,g->color,charn,c);
}
void drawArrayImage(Graphics *g,int xx,int yy,int w,int h,uint16_t **img)
{
	for (int x = 0;x<w;x++)
	{
		for (int y = 0;y<h;y++)
		{
			setpixeldirect(g->buffer,g->scr,xx+x,yy+y,img[x][y]);
		}
	}
}
void drawRect(Graphics *g,int x,int y,int w,int h)
{
	drawrectdirect(g->buffer,g->scr,x,y,w,h,g->color);
}
void fillRect(Graphics *g,int x,int y,int w,int h)
{
	fillrectdirect(g->buffer,g->scr,x,y,w,h,g->color);
}

void drawString10p(Graphics *g,int x,int y,char* string)
{
	int len = strlen(string);
	for (int i = 0;i<len;i++)
	{
		if (i >=100)
			break;
		void* c = resolveCharTo10pFont(*(string+i));
		if (c != NULL)
			write10pChar(g,x+10*i,y,0,c);
	}
}







































void* resolveCharTo10pFont(char c)
{
	if (c == *"0")
		return (digits10p+0);
	if (c == *"1")
		return (digits10p+1);
	if (c == *"2")
		return (digits10p+2);
	if (c == *"3")
		return (digits10p+3);
	if (c == *"4")
		return (digits10p+4);
	if (c == *"5")
		return (digits10p+5);
	if (c == *"6")
		return (digits10p+6);
	if (c == *"7")
		return (digits10p+7);
	if (c == *"8")
		return (digits10p+8);
	if (c == *"9")
		return (digits10p+9);
	if (c == *"a")
		return (chars10p+0);
	if (c == *"b")
		return (chars10p+1);
	if (c == *"c")
		return (chars10p+2);
	if (c == *"d")
		return (chars10p+3);
	if (c == *"e")
		return (chars10p+4);
	if (c == *"f")
		return (chars10p+5);
	if (c == *"g")
		return (chars10p+6);
	if (c == *"h")
		return (chars10p+7);
	if (c == *"i")
		return (chars10p+8);
	if (c == *"j")
		return (chars10p+9);
	if (c == *"k")
		return (chars10p+10);
	if (c == *"l")
		return (chars10p+11);
	if (c == *"m")
		return (chars10p+12);
	if (c == *"n")
		return (chars10p+13);
	if (c == *"o")
		return (chars10p+14);
	if (c == *"p")
		return (chars10p+15);
	if (c == *"q")
		return (chars10p+16);
	if (c == *"r")
		return (chars10p+17);
	if (c == *"s")
		return (chars10p+18);
	if (c == *"t")
		return (chars10p+19);
	if (c == *"u")
		return (chars10p+20);
	if (c == *"v")
		return (chars10p+21);
	if (c == *"w")
		return (chars10p+22);
	if (c == *"x")
		return (chars10p+23);
	if (c == *"y")
		return (chars10p+24);
	if (c == *"z")
		return (chars10p+25);
	if (c == *".")
		return (chars10p+26);
	if (c == *",")
		return (chars10p+27);
	if (c == *"?")
		return (chars10p+28);
	if (c == *"!")
		return (chars10p+29);
	if (c == *" ")
		return NULL;
	return NULL;
}






#endif