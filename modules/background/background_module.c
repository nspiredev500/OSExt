#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>


#define OSEXT_VERSION 0x00000004


#include "../module.h"


#include "bmp.h"
#include "img565.h"
#include "nspire_menus.h"
#include "background.h"

void filefunc();
void drawfunc(void* buffer);



void (*msleep)(uint32_t) = NULL;


bool (*isKeyPressed)() = NULL;
void* (*get_back_framebuffer_address)() = NULL;
void (*blitLCDBuffer)() = NULL;

void* (*useConsecutivePages)(uint32_t,uint32_t) = NULL;
void (*freeConsecutivePages)(void*,uint32_t) = NULL;

uint32_t (*rtc_get_value)() = NULL;


NUC_FILE* (*nuc_fopen)(const char *path,char *mode) = NULL;
int32_t (*nuc_fclose)(NUC_FILE* f) = NULL;
uint32_t (*nuc_fread)(void* buffer,uint32_t elements,uint32_t size,NUC_FILE* f) = NULL;


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
void (*panic)(const char*) = NULL;
void (*uart_printf)(const char*,...) = NULL;

void* (*kmalloc)(uint32_t) = NULL;
void (*kfree)(void*) = NULL;

void (*framebuffer_setpixel565)(void *buff,uint32_t x,uint32_t y,uint16_t rgb565) = NULL;
uint16_t (*rgbto565)(uint32_t,uint32_t,uint32_t) = NULL;

void (*k_memcpy)(void*,void*,size_t) = NULL;
uint32_t (*signedtounsigned32)(int32_t i) = NULL;


void* (*search)(char*) = NULL;
void* module_start(void* search_func, uint32_t osext_version, bool standalone)
{
	search = (void* (*)(char*)) search_func;
	if (search == NULL)
	{
		return NULL;
	}
	SEARCH_AND_TEST(msleep)
	SEARCH_AND_TEST(isKeyPressed)
	SEARCH_AND_TEST(get_back_framebuffer_address)
	SEARCH_AND_TEST(blitLCDBuffer)
	SEARCH_AND_TEST(useConsecutivePages)
	SEARCH_AND_TEST(freeConsecutivePages)
	SEARCH_AND_TEST(rtc_get_value)
	SEARCH_AND_TEST(nuc_fopen)
	SEARCH_AND_TEST(nuc_fclose)
	SEARCH_AND_TEST(nuc_fread)
	SEARCH_AND_TEST(framebuffer_fillrect)
	SEARCH_AND_TEST(framebuffer_drawrect)
	SEARCH_AND_TEST(framebuffer_write10pdigit)
	SEARCH_AND_TEST(framebuffer_setpixel)
	SEARCH_AND_TEST(framebuffer_write10pstring_ascii)
	SEARCH_AND_TEST(k_memset)
	SEARCH_AND_TEST(sprintf_safe)
	SEARCH_AND_TEST(k_strlen)
	SEARCH_AND_TEST(register_draw_function)
	SEARCH_AND_TEST(unregister_draw_function)
	SEARCH_AND_TEST(register_file_function)
	SEARCH_AND_TEST(unregister_file_function)
	SEARCH_AND_TEST(panic)
	SEARCH_AND_TEST(uart_printf)
	SEARCH_AND_TEST(kmalloc)
	SEARCH_AND_TEST(kfree)
	SEARCH_AND_TEST(framebuffer_setpixel565)
	SEARCH_AND_TEST(rgbto565)
	SEARCH_AND_TEST(k_memcpy)
	SEARCH_AND_TEST(signedtounsigned32)
	
	register_draw_function(drawfunc);
	register_file_function(filefunc);
	
	background_update();
	return &module_end;
}


void filefunc()
{
	if (isKeyPressed(KEY_CTRL) && isKeyPressed(KEY_PI))
	{
		background_update();
	}
}

void drawfunc(void* buffer)
{
	background_draw_image(buffer,buffer);
}



void module_end()
{
	if (! unregister_draw_function(drawfunc))
	{
		panic("unable to unregister drawfunc of the background module!");
	}
	if (! unregister_file_function(filefunc))
	{
		panic("unable to unregister filefunc of the background module!");
	}
	if (background_is_slideshow())
	{
		struct img565* show = background_get_slideshow();
		for (uint32_t i = 0;i<background_slideshow_length();i++)
		{
			destroy_img565(&show[i]);
		}
		background_unset_slideshow();
		kfree(show);
	}
	else
	{
		struct img565* img = background_get_image();
		background_unset_image();
		destroy_img565(img);
	}
}









