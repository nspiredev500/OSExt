#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>

#include "time-dialog.h"



#define SEARCH_AND_TEST(function) \
									function = search(#function); \
									if (function == NULL) return NULL;


#define OSEXT_VERSION 0x00000004

void module_end();

void filefunc();
void drawfunc(void* buffer);

bool draw_clock = true;




// define the keycodes
enum keycode {KEY_RET = 0, KEY_ENTER, KEY_MATH_MINUS, KEY_SPACE, KEY_Z, KEY_Y, KEY_ZERO, KEY_QUESTION, KEY_ON,
			  KEY_X, KEY_W, KEY_V, KEY_THREE, KEY_U, KEY_T, KEY_S, KEY_ONE, KEY_PI, KEY_TRIG, KEY_TEN_POW,
			  KEY_R, KEY_Q, KEY_P, KEY_SIX, KEY_O, KEY_N, KEY_M, KEY_4, KEY_EE, KEY_SQUARED,
			  KEY_L, KEY_K, KEY_J, KEY_NINE, KEY_I, KEY_H, KEY_G, KEY_SEVEN, KEY_SLASH, KEY_E_POW,
			  KEY_F, KEY_E, KEY_D, KEY_C, KEY_B, KEY_A, KEY_EQUALS, KEY_STAR, KEY_CIRCUMFLEX,
			  KEY_VAR, KEY_MINUS, KEY_CLOSING_BRACKET, KEY_DOT, KEY_OPENING_BRACKET, KEY_FIVE, KEY_CAT, KEY_FRAC, KEY_DEL, KEY_SCRATCH,
			  KEY_FLAG, KEY_PLUS, KEY_DOC, KEY_TWO, KEY_MENU, KEY_EIGHT, KEY_ESCAPE, KEY_TAB,
			  KEY_SHIFT, KEY_CTRL, KEY_COMMA};
//
enum touchpad_arrow {ARROW_UP = 0, ARROW_RIGHT, ARROW_DOWN, ARROW_LEFT};
struct touchpad_report {
	bool contact;
	uint8_t proximity;
	uint16_t x;
	uint16_t y;
	uint8_t xrel;
	uint8_t yrel;
	bool pressed;
	//uint8_t status;
};






int64_t (*systime_unix)() = NULL;
void (*systime_unix_to_timestamp)(uint64_t,uint32_t*,uint32_t*,uint32_t*,uint32_t*,uint32_t*,uint32_t*) = NULL;
void (*keypad_press_release_barrier)() = NULL;
void (*systime_set_unix)(int64_t) = NULL;
int64_t (*systime_timestamp_to_unix)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t) = NULL;
void (*msleep)(uint32_t) = NULL;


bool (*isKeyPressed)() = NULL;
void* (*get_back_framebuffer_address)() = NULL;
void (*blitLCDBuffer)() = NULL;
void (*touchpad_get_report)(struct touchpad_report*) = NULL;
bool (*touchpad_is_arrow)(enum touchpad_arrow,struct touchpad_report*) = NULL;


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
void (*debug_shell_println_rgb)(const char*,uint32_t,uint32_t,uint32_t,...) = NULL;
void* (*search)(char*) = NULL;
void (*panic)(const char*) = NULL;
void (*uart_printf)(const char*,...) = NULL;
void* module_start(void* search_func, uint32_t osext_version, bool standalone)
{
	search = (void* (*)(char*)) search_func;
	if (search == NULL)
	{
		return NULL;
	}
	SEARCH_AND_TEST(panic)
	SEARCH_AND_TEST(systime_unix)
	SEARCH_AND_TEST(systime_unix_to_timestamp)
	SEARCH_AND_TEST(isKeyPressed)
	SEARCH_AND_TEST(register_draw_function)
	SEARCH_AND_TEST(unregister_draw_function)
	SEARCH_AND_TEST(register_file_function)
	SEARCH_AND_TEST(unregister_file_function)
	SEARCH_AND_TEST(keypad_press_release_barrier)
	SEARCH_AND_TEST(systime_set_unix)
	SEARCH_AND_TEST(systime_timestamp_to_unix)
	SEARCH_AND_TEST(msleep)
	SEARCH_AND_TEST(get_back_framebuffer_address)
	SEARCH_AND_TEST(blitLCDBuffer)
	SEARCH_AND_TEST(touchpad_get_report)
	SEARCH_AND_TEST(touchpad_is_arrow)
	SEARCH_AND_TEST(framebuffer_fillrect)
	SEARCH_AND_TEST(framebuffer_drawrect)
	SEARCH_AND_TEST(framebuffer_write10pdigit)
	SEARCH_AND_TEST(framebuffer_setpixel)
	SEARCH_AND_TEST(k_memset)
	SEARCH_AND_TEST(sprintf_safe)
	SEARCH_AND_TEST(k_strlen)
	SEARCH_AND_TEST(uart_printf)
	SEARCH_AND_TEST(framebuffer_write10pstring_ascii)
	
	register_draw_function(drawfunc);
	register_file_function(filefunc);
	
	
	
	debug_shell_println_rgb = search("debug_shell_println_rgb");
	if (debug_shell_println_rgb != NULL)
	{
		debug_shell_println_rgb("clock module loaded!",0,255,0);
	}
	return &module_end;
}

void filefunc()
{
	if (isKeyPressed(KEY_CTRL) && isKeyPressed(KEY_EE) && isKeyPressed(KEY_G))
	{
		set_time_dialog();
	}
	if (isKeyPressed(KEY_CTRL) && isKeyPressed(KEY_EE))
	{
		if (draw_clock)
		{
			draw_clock = false;
		}
		else
		{
			draw_clock = true;
		}
	}
}

void drawfunc(void* buffer)
{
	if (draw_clock)
	{
		uint32_t clockx = 180;
		uint32_t clocky = 1;
		framebuffer_fillrect(buffer,180,1,70,10,0,0,0);
		framebuffer_drawrect(buffer,179,0,72,12,255,255,255);
		
		uint32_t hr = 0,min = 0,sec = 0;
		systime_unix_to_timestamp(systime_unix(),NULL,NULL,NULL,&hr,&min,&sec);
		
		
		framebuffer_write10pdigit(buffer,clockx+60,clocky,255,0,0,sec%10);
		framebuffer_write10pdigit(buffer,clockx+50,clocky,255,0,0,(sec/10)%10);
		
		framebuffer_setpixel(buffer,clockx+47,clocky+3,255,0,0);
		framebuffer_setpixel(buffer,clockx+47,clocky+7,255,0,0);
		
		
		framebuffer_write10pdigit(buffer,clockx+35,clocky,255,0,0,min%10);
		framebuffer_write10pdigit(buffer,clockx+25,clocky,255,0,0,(min/10)%10);
		
		
		framebuffer_setpixel(buffer,clockx+23,clocky+3,255,0,0);
		framebuffer_setpixel(buffer,clockx+23,clocky+7,255,0,0);
		
		
		framebuffer_write10pdigit(buffer,clockx+10,clocky,255,0,0,hr%10);
		framebuffer_write10pdigit(buffer,clockx,clocky,255,0,0,(hr/10)%10);
	}
}



void module_end()
{
	if (! unregister_draw_function(drawfunc))
	{
		panic("unable to unregister drawfunc of the clock module!");
	}
	if (! unregister_file_function(filefunc))
	{
		panic("unable to unregister filefunc of the clock module!");
	}
}










