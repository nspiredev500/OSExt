
#include "include/osext.h"
#ifndef DYNLIBNDLESS
#define DYNLIBNDLESS

#define CPU_SPEED_150MHZ 0x00000002
#define CPU_SPEED_120MHZ 0x000A1002
#define CPU_SPEED_90MHZ  0x00141002

typedef struct {
	uint16_t width;
	uint16_t height;
} touchpad_info_t;

typedef struct {
	unsigned char contact; /* "touched". TRUE or FALSE */
	unsigned char proximity;
	uint16_t x;
	uint16_t y;
	unsigned char x_velocity;
	unsigned char y_velocity;
	uint16_t dummy;
	unsigned char pressed; /* "key pressed". TRUE or FALSE */
	unsigned char arrow; /* area of the pad currently touched. see tpad_arrow_t. */
} touchpad_report_t;

typedef enum {
        SCR_TYPE_INVALID=-1,
        SCR_320x240_565=0,
        SCR_320x240_4=1,
        SCR_240x320_565=2,
        SCR_320x240_16=3,
        SCR_320x240_8=4,
        SCR_320x240_555=5,
        SCR_240x320_555=6,
        SCR_TYPE_COUNT=7
} scr_type_t;




void (*assert_ndless_rev)(unsigned required_rev);
BOOL (*any_key_pressed)(void);
void (*clear_cache)(void);
int (*enable_relative_paths)(char **argv);
int (*file_each)(const char *folder, int (*callback);
void (*idle)(void);
unsigned (*msleep)(unsigned millisec);
int (*locate)(const char *filename, char *dst_path, size_t dst_path_size);
BOOL (*on_key_pressed)(void);
void (*refresh_osscr)(void);
unsigned (*set_cpu_speed)(unsigned speed);
unsigned (*_show_msgbox)(const char *title, const char *msg, unsigned button_num, ...);
int (*show_msg_user_input)(const char * title, const char * msg, const char * defaultvalue, char ** value_ref);
touchpad_info_t *(*touchpad_getinfo)(void);
int (*touchpad_scan)(touchpad_report_t *report);
BOOL (*touchpad_arrow_pressed)(tpad_arrow_t arrow);
usbd_status (*usbd_set_idle)(usbd_interface_handle iface, int duration, int id);
usbd_status (*usbd_set_protocol)(usbd_interface_handle iface, int report);
BOOL (*isKeyPressed)(const t_key *key);
void (*wait_key_pressed)(void);
void (*wait_no_key_pressed)(void);
void (*cfg_open)(void);
void (*cfg_open_file)(const char *filepath);
void (*cfg_close)(void);
char *(*cfg_get)(const char *key);
void (*cfg_register_fileext)(const char *ext, const char *prgm);
void (*cfg_register_fileext_file)(const char *fielpath, const char *ext, const char *prgm);
void (*clrscr)(void);
BOOL (*lcd_isincolor)(void);
void (*lcd_incolor)(void);
void (*lcd_ingray)(void);
unsigned (*_scrsize)(void);
bool (*lcd_init)(scr_type_t type);
void (*lcd_blit)(void *buffer, scr_type_t buffer_type);
scr_type_t (*lcd_type)();
BOOL (*_is_touchpad)(void);
unsigned (*hwtype)(void);

void loaddynlibndless()
{
	assertOSExt();
	initRecursiveDynlinker();
	unsigned int *table = requestLibrary_r("dynlibndless");
	assert_ndless_rev = table+0;
	any_key_pressed = table+1;
	clear_cache = table+2;
	enable_relative_paths = table+3;
	file_each = table+4;
	idle = table+5;
	msleep = table+6;
	locate = table+7;
	on_key_pressed = table+8;
	refresh_osscr = table+9;
	set_cpu_speed = table+10;
	_show_msgbox = table+11;
	show_msg_user_input = table+12;
	touchpad_getinfo = table+13;
	touchpad_scan = table+14;
	touchpad_arrow_pressed = table+15;
	usbd_set_idle = table+16;
	usbd_set_protocol = table+17;
	isKeyPressed = table+18;
	wait_key_pressed = table+19;
	wait_no_key_pressed = table+20;
	cfg_open = table+21;
	cfg_open_file = table+22;
	cfg_close = table+23;
	cfg_get = table+24;
	cfg_register_fileext = table+25;
	cfg_register_fileext_file = table+26;
	clrscr = table+27;
	lcd_isincolor = table+28;
	lcd_incolor = table+29;
	lcd_ingray = table+30;
	_scrsize = table+31;
	lcd_init = table+32;
	lcd_blit = table+33;
	lcd_type = table+34;
	_is_touchpad = table+35;
	hwtype = table+36;

	return;
}

#define isKeyPressed(x) isKeyPressed(&x)
#define nl_hassyscall(x) _nl_hassyscall(e_##x)
#define is_touchpad _is_touchpad()
#define show_msgbox(title, msg) _show_msgbox(title, msg, 0)
#define show_msgbox_2b(title, msg, button1, button2) _show_msgbox(title, msg, 2, button1, button2)
#define show_msgbox_3b(title, msg, button1, button2, button3) _show_msgbox(title, msg, 3, button1, button2, button3)
#define is_classic (hwtype() < 1)
#define is_cm (nl_hwsubtype() == 1)
#define has_colors (!is_classic)
#define IO_LCD_CONTROL IO(0xC000001C, 0xC0000018)
#define IO(a,b) (((volatile unsigned*[]){ (unsigned*)a, (unsigned*)b })[hwtype()])
#define REAL_SCREEN_BASE_ADDRESS     (*(void**)0xC0000010)



#endif