#ifndef RAWDRAW_H
#define RAWDRAW_H
void setpixeldirect(void* buffer,scr_type_t scr,int x,int y,uint16_t color);
void write10pchardirect(void* buffer,scr_type_t scr,int xx,int yy,uint16_t color,int charn,char c[][10][10]);
void drawrectdirect(void* buffer,scr_type_t scr,int xx,int yy,int w,int h,uint16_t color);
void fillrectdirect(void* buffer,scr_type_t scr,int xx,int yy,int w,int h,uint16_t color);
uint16_t rgbto565(int r,int g, int b);




void setpixeldirect(void *buffer,scr_type_t scr,int x,int y,uint16_t color)
{
	if (scr == SCR_240x320_565)
	{
		if (x<0 || y<0 || x>320 || y >240)
			return;
		uint16_t *wbuff = (uint16_t*) buffer;
		*(wbuff+y+x*240) = color;
	}
}
void write10pchardirect(void* buffer,scr_type_t scr,int xx,int yy,uint16_t color,int charn,char c[][10][10])
{
	for (int x = 0;x<10;x++)
	{
		for (int y = 0;y<10;y++)
		{
			if (c[charn][y][x] == 1)
			{
				setpixeldirect(buffer,scr,xx+x,yy+y,color);
			}
		}
	}
}
void fillrectdirect(void* buffer,scr_type_t scr,int xx,int yy,int w,int h,uint16_t color)
{
	for (int x = xx;x<xx+w;x++)
	{
		for (int y = yy;y<yy+h;y++)
		{
			setpixeldirect(buffer,scr,x,y,color);
		}
	}
}
void drawrectdirect(void* buffer,scr_type_t scr,int xx,int yy,int w,int h,uint16_t color)
{
	for (int x = xx;x<xx+w;x++)
	{
		setpixeldirect(buffer,scr,x,yy,color);
		setpixeldirect(buffer,scr,x,yy+h-1,color);
	}
	for (int y = yy;y<yy+h;y++)
	{
		setpixeldirect(buffer,scr,xx,y,color);
		setpixeldirect(buffer,scr,xx+w-1,y,color);
	}
}
uint16_t rgbto565(int r,int g, int b)
{
	if (! (r<0 || r>255 || g<0 || g>255 || b<0 || b>255))
	{
		return ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
	}
	return 0;
}





#endif