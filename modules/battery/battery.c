#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>




#define OSEXT_VERSION 0x00000004

#include "../module.h"


void module_end();

void filefunc();
void drawfunc(void* buffer);

bool draw_battery = true;
int64_t lastchecked = 0;
double vsys = 0;

const double vsys_0 = 3.542;

const double vsys_percent = 0.00606;




double (*adc_read_channel)(uint32_t) = NULL;


int64_t (*systime_unix)() = NULL;
void (*keypad_press_release_barrier)() = NULL;


bool (*isKeyPressed)() = NULL;


void (*framebuffer_fillrect)(void*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t) = NULL;
void (*framebuffer_drawrect)(void*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t) = NULL;
void (*framebuffer_write10pdigit)(void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b,int charn) = NULL;
void (*framebuffer_setpixel)(void *buff,uint32_t x,uint32_t y,uint32_t r, uint32_t g,uint32_t b) = NULL;
void (*framebuffer_write10pstring_ascii)(char* string,void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b) = NULL;

void (*k_memset)(void *str,int c, size_t n) = NULL;
void (*sprintf_safe)(char *result,char *str,uint32_t length,...) = NULL;
uint32_t (*k_strlen)(const char *string,uint32_t max) = NULL;


bool (*register_draw_function)(void*) = NULL;
bool (*unregister_draw_function)(void*) = NULL;
bool (*register_file_function)(void*) = NULL;
bool (*unregister_file_function)(void*) = NULL;
void* (*search)(char*) = NULL;
void (*panic)(const char*) = NULL;
void (*uart_printf)(const char*,...) = NULL;
void* module_start(void* search_func, uint32_t osext_version, bool standalone)
{
	
	if (standalone)
	{
		return NULL;
	}
	search = (void* (*)(char*)) search_func;
	if (search == NULL)
	{
		return NULL;
	}
	SEARCH_AND_TEST(panic)
	SEARCH_AND_TEST(systime_unix)
	SEARCH_AND_TEST(isKeyPressed)
	SEARCH_AND_TEST(register_draw_function)
	SEARCH_AND_TEST(unregister_draw_function)
	SEARCH_AND_TEST(register_file_function)
	SEARCH_AND_TEST(unregister_file_function)
	SEARCH_AND_TEST(keypad_press_release_barrier)
	SEARCH_AND_TEST(framebuffer_fillrect)
	SEARCH_AND_TEST(framebuffer_drawrect)
	SEARCH_AND_TEST(framebuffer_write10pdigit)
	SEARCH_AND_TEST(framebuffer_setpixel)
	SEARCH_AND_TEST(k_memset)
	SEARCH_AND_TEST(sprintf_safe)
	SEARCH_AND_TEST(k_strlen)
	SEARCH_AND_TEST(uart_printf)
	SEARCH_AND_TEST(framebuffer_write10pstring_ascii)
	SEARCH_AND_TEST(adc_read_channel)
	
	register_draw_function(drawfunc);
	register_file_function(filefunc);
	
	
	return &module_end;
}

void filefunc()
{
	if (isKeyPressed(KEY_CTRL) && isKeyPressed(KEY_SPACE))
	{
		draw_battery = ! draw_battery;
	}
}

void drawfunc(void* buffer)
{
	if (draw_battery)
	{
		int64_t unix_time = systime_unix();
		if (unix_time - lastchecked >= 10 || unix_time - lastchecked <= -10)
		{
			lastchecked = unix_time;
			vsys = adc_read_channel(2);
			uart_printf("updated vsys!\n");
		}
		/*
			vsys = vsys_0 + percent * vsys_percent
			percent = (vsys - vsys_0) / vsys_percent
		*/
		double percent_double = (vsys-vsys_0) / vsys_percent;
		int32_t percent_int = (int32_t) percent_double;
		if (percent_int < 0)
		{
			percent_int = 0;
		}
		if (percent_int > 100)
		{
			percent_int = 100;
		}
		
		framebuffer_fillrect(buffer,255,1,40,10,0,0,0);
		framebuffer_drawrect(buffer,254,0,42,12,255,255,255);
		
		char stringbuffer[10];
		k_memset(stringbuffer,'\0',10);
		sprintf_safe(stringbuffer,"%d%%",8,percent_int);
		framebuffer_write10pstring_ascii(stringbuffer,buffer,255,1,255,0,0);
		
		
		
	}
}



void module_end()
{
	if (! unregister_draw_function(drawfunc))
	{
		panic("unable to unregister drawfunc of the battery module!");
	}
	if (! unregister_file_function(filefunc))
	{
		panic("unable to unregister filefunc of the battery module!");
	}
}










