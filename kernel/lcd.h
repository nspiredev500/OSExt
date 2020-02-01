#ifndef LCD_H
#define LCD_H



void initLCDDriver();

void claimLCD();
void freeLCD();

void blitLCDBuffer();
void lcd_setpixel(uint32_t x,uint32_t y,uint32_t r, uint32_t g,uint32_t b);
void lcd_fillrect(uint32_t xs,uint32_t ys, uint32_t w, uint32_t h,uint32_t r, uint32_t g,uint32_t b);
void lcd_write10pchar(int xx,int yy,uint32_t r, uint32_t g,uint32_t b,int charn,char c[][10][10]);

void framebuffer_setpixel(void *buff,uint32_t x,uint32_t y,uint32_t r, uint32_t g,uint32_t b);
void framebuffer_fillrect(void *buff,uint32_t xs,uint32_t ys, uint32_t w, uint32_t h,uint32_t r, uint32_t g,uint32_t b);
void framebuffer_write10pchar(void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b,int charn,char c[][10][10]);

void lcd_write10pstring(char* string,int xx,int yy,uint32_t r, uint32_t g,uint32_t b,char c[][10][10],char d[][10][10]);
void framebuffer_write10pstring(char* string,void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b,char c[][10][10],char d[][10][10]);






































#endif