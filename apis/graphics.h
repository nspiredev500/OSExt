#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "rawdraw.h"
#include "digits.h"
#include "chars.h"
#include "utils.h"
#include "bmp.h"

/*
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
 0x11d067d8, 0x11d6e630, // currently not working in hardware, only for read I think or when gc drawing initialized
 0x0, 0x0
};*/
//noncas  11712960  10a3e250      10B9 5542    12C 8588
//cas  11778c90  10aa2fe4         5F 59A0      12C B64C‬


/*
void initOSGCBUFF()
{
	OSGCBUFF = (uint16_t*) nl_osvalue(OSGCBUFF_adr,32);
}
*/
Gc os_gc = NULL;

void* resolveCharTo10pFont(char c);
typedef struct{
	uint16_t buffer[240*320*2]; // 240*320 somehow isn't enough for the whole screen
	uint16_t color;
	scr_type_t scr;
} Graphics;
void initGraphics()
{
	if (os_gc == NULL)
	{
		os_gc = gui_gc_global_GC();
	}
	lcd_init(lcd_type());
}
void exitGraphics()
{
	lcd_init(SCR_TYPE_INVALID);
}
Graphics* createGraphics()
{
	if (os_gc == NULL)
	{
		os_gc = gui_gc_global_GC();
	}
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
	uint16_t *osbuff = (((((char *****)os_gc)[9])[0])[0x8])[0]; // from ngc.c
	for (int x = 0;x<320;x++)
	{
		for (int y = 0;y<240;y++)
		{
			*(g->buffer+y+x*240) = *(osbuff+x+y*320);
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
void blitRegionToScreen(Graphics *g,int xs,int ys,int w,int h)
{
	for (int x = xs;x<w;x++)
	{
		for (int y = ys;y<h;y++)
		{
			setpixeldirect(REAL_SCREEN_BASE_ADDRESS,g->scr,x,y,(g->buffer+240*x+y));
		}
	}
}
void blitGraphicsToBuffer(Graphics *g,void *bufferp)
{
	uint16_t *buffer = bufferp;
	for (int x = 0;x<320;x++)
	{
		for (int y = 0;y<240;y++)
		{
			*(buffer+240*x+y) = *(g->buffer+240*x+y);
		}
	}
}
void blitGraphicsRegionToBuffer(Graphics *g,void *bufferp,int xs,int ys,int w,int h)
{
	uint16_t *buffer = bufferp;
	for (int x = xs;x<xs+w;x++)
	{
		for (int y = ys;y<ys+h;y++)
		{
			if (x >= 0 && x < 320 && y >= 0 && y < 240)
				*(buffer+240*x+y) = *(g->buffer+240*x+y);
		}
	}
}
void blitGraphicsToOSGC(Graphics *g)
{
	uint16_t *osbuff = (((((char *****)os_gc)[9])[0])[0x8])[0]; // from ngc.c
	for (int x = 0;x<320;x++)
	{
		for (int y = 0;y<240;y++)
		{
			*(osbuff+x+y*320) = *(g->buffer+y+x*240);
		}
	}
}
void setPixel(Graphics *g,int x,int y)
{
	setpixeldirect(g->buffer,g->scr,x,y,g->color);
}
void drawArrayImage(Graphics *g,ArrayImage *img,int xs,int ys) // currently no resizing
{
	if (g->scr == SCR_240x320_565 || g->scr == SCR_320x240_565)
	{
		uint16_t *buff = (uint16_t*) img->data;
		for (int x = 0;x<img->width;x++)
		{
			for (int y = 0;y<img->height;y++)
			{
				setpixeldirect(g->buffer,g->scr,xs+x,ys+y,buff[240*x+y]);
			}
		}
	}
}
void drawLine(Graphics *g,int xs,int ys,int xe,int ye)
{
	int xdiff = xe-xs;
	int ydiff = ye-ys;
	if (xdiff == 0 && ydiff == 0)
		return;
	if (xdiff == 0)
	{
		int *ysp = (ydiff<0) ? &ye: &ys;
		int *yep = (ydiff>0) ? &ye: &ys;
		for (int y = *ysp;y<*yep;y++)
		{
			setPixel(g,xs,y);
		}
		return;
	}
	if (ydiff == 0)
	{
		int *xsp = (xdiff<0) ? &xe: &xs;
		int *xep = (xdiff>0) ? &xe: &xs;
		for (int x = *xsp;x<*xep;x++)
		{
			setPixel(g,x,ys);
		}
		return;
	}
	if (xe < xs)
	{
		intswap(&xs,&xe);
		intswap(&ys,&ye);
	}
	// y = m*x+b    b = y-m*x
	double m = ((double)(ye-ys))/((double)(xe-xs));
	double b = ((double)ys)-m*((double) xs);
	for (int x = xs;x<xe;x++)
	{
		setPixel(g,x,roundtoint(x*m+b));
	}
}
void write10pChar(Graphics *g,int x,int y,int charn,char c[][10][10])
{
	write10pchardirect(g->buffer,g->scr,x,y,g->color,charn,c);
}
/*
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
*/
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
