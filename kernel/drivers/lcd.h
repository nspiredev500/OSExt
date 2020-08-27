#ifndef LCD_H
#define LCD_H


void remappLCD(void* address);

uint16_t rgbto565(uint32_t r,uint32_t g,uint32_t b);

void* get_old_framebuffer_address();
void* get_back_framebuffer_address();
void* get_front_framebuffer_address();

void initLCDDriver();


void lcd_power_down();
void lcd_power_up();


void claimLCD();
void freeLCD();

void blitLCDBuffer();
void lcd_setpixel(uint32_t x,uint32_t y,uint32_t r, uint32_t g,uint32_t b);
void lcd_fillrect(uint32_t xs,uint32_t ys, uint32_t w, uint32_t h,uint32_t r, uint32_t g,uint32_t b);
void lcd_write10pchar(int xx,int yy,uint32_t r, uint32_t g,uint32_t b,int charn);



void framebuffer_setpixel565(void *buff,uint32_t x,uint32_t y,uint16_t rgb565);
void framebuffer_draw_img565(void *buff,struct img565* img,uint32_t x,uint32_t y);
void framebuffer_setpixel(void *buff,uint32_t x,uint32_t y,uint32_t r, uint32_t g,uint32_t b);
void framebuffer_fillrect(void *buff,uint32_t xs,uint32_t ys, uint32_t w, uint32_t h,uint32_t r, uint32_t g,uint32_t b);
void framebuffer_drawrect(void *buff,uint32_t xs,uint32_t ys, uint32_t w, uint32_t h,uint32_t r, uint32_t g,uint32_t b);
void framebuffer_write10pchar(void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b,int charn);
void framebuffer_write10pdigit(void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b,int charn);

void lcd_write10pstring(char* string,int xx,int yy,uint32_t r, uint32_t g,uint32_t b);
void framebuffer_write10pstring(char* string,void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b);
void framebuffer_write10pstring_ascii(char* string,void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b);





































#endif