

#include <libndls.h>


unsigned int table[] = {assert_ndless_rev,any_key_pressed,clear_cache,enable_relative_paths,file_each,idle,msleep,locate,on_key_pressed,refresh_osscr,set_cpu_speed,_show_msgbox,show_msg_user_input,touchpad_getinfo,touchpad_scan,touchpad_arrow_pressed,usbd_set_idle,usbd_set_protocol,isKeyPressed,wait_key_pressed,wait_no_key_pressed,cfg_open,cfg_open_file,cfg_close,cfg_get,cfg_register_fileext,cfg_register_fileext_file,0,0,0,0,0,lcd_init,lcd_blit,lcd_type,_is_touchpad,hwtype,0};

#include "include/osext.h"
void main(int argsn, char* argc)
{
	assertOSExt();
	initRecursiveDynlinker();
	registerLibrary_r("dynlibndless",table);

	return 0;
}