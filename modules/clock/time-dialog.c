#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>

#include "time-dialog.h"



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






int64_t (*systime_unix)();
void (*systime_unix_to_timestamp)(uint64_t,uint32_t*,uint32_t*,uint32_t*,uint32_t*,uint32_t*,uint32_t*);
void (*keypad_press_release_barrier)();
void (*systime_set_unix)(int64_t);
int64_t (*systime_timestamp_to_unix)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
void (*msleep)(uint32_t);


bool (*isKeyPressed)();
void* (*get_back_framebuffer_address)();
void (*blitLCDBuffer)();
void (*touchpad_get_report)(struct touchpad_report*);
bool (*touchpad_is_arrow)(enum touchpad_arrow,struct touchpad_report*);



void (*framebuffer_fillrect)(void*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
void (*framebuffer_drawrect)(void*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
void (*framebuffer_write10pstring_ascii)(char* string,void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b);

void (*k_memset)(void *str,int c, size_t n);
void (*sprintf_safe)(char *result,char *str,uint32_t length,...);
uint32_t (*k_strlen)(const char *string,uint32_t max);



bool (*register_draw_function)(void*);
bool (*unregister_draw_function)(void*);
bool (*register_file_function)(void*);
bool (*unregister_file_function)(void*);
void (*debug_println_rgb)(const char*,uint32_t,uint32_t,uint32_t,...);
void* (*search)(char*);
void (*panic)(const char*);

void (*uart_printf)(const char*,...);
void set_time_dialog()
{
	
	uint32_t selected = 0;
	uint32_t numbers[6];
	numbers[0] = 2020;
	numbers[1] = 1;
	numbers[2] = 1;
	numbers[3] = 0;
	numbers[4] = 0;
	numbers[5] = 0;
	while (! isKeyPressed(KEY_ESCAPE))
	{
		struct touchpad_report rep;
		touchpad_get_report(&rep);
		void *framebuffer = get_back_framebuffer_address();
		framebuffer_fillrect(framebuffer,0,0,320,240,255,255,255);
		if (touchpad_is_arrow(ARROW_UP,&rep))
		{
			switch (selected)
			{
			case 0:
				if (numbers[selected] < 2040)
					numbers[selected]++;
				break;
			case 1:
				if (numbers[selected] < 12)
					numbers[selected]++;
				break;
			case 2:
				if (numbers[selected] < 31)
					numbers[selected]++;
				break;
			case 3:
				if (numbers[selected] < 23)
					numbers[selected]++;
				break;
			case 4:
			case 5:
				if (numbers[selected] < 60)
					numbers[selected]++;
				break;
			}
		}
		if (touchpad_is_arrow(ARROW_DOWN,&rep))
		{
			switch (selected)
			{
			case 0:
				if (numbers[selected] > 1970)
					numbers[selected]--;
				break;
			case 1:
			case 2:
				if (numbers[selected] > 1)
					numbers[selected]--;
				break;
			case 3:
			case 4:
			case 5:
				if (numbers[selected] > 0)
					numbers[selected]--;
				break;
			}
		}
		if (touchpad_is_arrow(ARROW_LEFT,&rep))
		{
			if (selected > 0)
				selected--;
		}
		if (touchpad_is_arrow(ARROW_RIGHT,&rep))
		{
			if (selected < 5)
				selected++;
		}
		
		char buffer[100];
		k_memset(buffer,'\0',90);
		sprintf_safe(buffer,"y: %d mon: %d d: %d",80,numbers[0],numbers[1],numbers[2]);
		framebuffer_write10pstring_ascii(buffer,framebuffer,320/2-k_strlen(buffer,80)*5,240/2-5,0,0,0);
		
		k_memset(buffer,'\0',90);
		sprintf_safe(buffer,"h: %d min: %d s: %d",80,numbers[3],numbers[4],numbers[5]);
		framebuffer_write10pstring_ascii(buffer,framebuffer,320/2-k_strlen(buffer,80)*5,240/2-5+10,0,0,0);
		
		
		switch (selected)
		{
		case 0:
			framebuffer_write10pstring_ascii("y",framebuffer,320/2,0,0,200,0);
			break;
		case 1:
			framebuffer_write10pstring_ascii("mon",framebuffer,320/2,0,0,200,0);
			break;
		case 2:
			framebuffer_write10pstring_ascii("d",framebuffer,320/2,0,0,200,0);
			break;
		case 3:
			framebuffer_write10pstring_ascii("h",framebuffer,320/2,0,0,200,0);
			break;
		case 4:
			framebuffer_write10pstring_ascii("min",framebuffer,320/2,0,0,200,0);
			break;
		case 5:
			framebuffer_write10pstring_ascii("s",framebuffer,320/2,0,0,200,0);
			break;
		}
		blitLCDBuffer();
		msleep(70);
	}
	
	systime_set_unix(systime_timestamp_to_unix(numbers[0],numbers[1],numbers[2],numbers[3],numbers[4],numbers[5]));
	keypad_press_release_barrier();
}




