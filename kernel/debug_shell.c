#include "kernel.h"


static uint32_t line = 0;

void debug_shell_println(char* string,...)
{
	if (line >= 240)
	{
		keypad_press_release_barrier(); // clear the screen after the user pressed a key to make place for new lines
		k_memset((void*) *LCD_UPBASE,0,320*240*2);
		line = 0;
	}
	char str[200];
	k_memset(str,'\0',190);
	va_list va;
	va_start(va,string);
	sprintf_safe_va(str,string,180,va);
	va_end(va);
	framebuffer_write10pstring(str,(void*) *LCD_UPBASE,0,line,255,255,255,chars10p,digits10p);
	line += 10;
	char *strpart = str+32;
	while (k_strlen(strpart,190) > 0)
	{
		framebuffer_write10pstring(strpart,(void*) *LCD_UPBASE,10,line,255,255,255,chars10p,digits10p);
		line += 10;
		if (line >= 240)
		{
			keypad_press_release_barrier(); // clear the screen after the user pressed a key to make place for new lines
			k_memset((void*) *LCD_UPBASE,0,320*240*2);
			line = 0;
		}
		uint32_t add = k_strlen(strpart,190);
		if (add > 31)
			add = 31;
		strpart += add;
	}
	// also print via uart when debug enabled
	DEBUGPRINTLN_1(str)
}




























