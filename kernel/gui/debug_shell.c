#include "../kernel.h"


static uint32_t line = 0;
static void *shell_framebuffer = NULL;


void setShellFramebuffer(void *framebuffer)
{
	shell_framebuffer = framebuffer;
}

void debug_shell_println_rgb(char* string,uint32_t r, uint32_t g,uint32_t b,...)
{
	#ifndef RELEASE
	if (line >= 240)
	{
		keypad_press_release_barrier(); // clear the screen after the user pressed a key to make place for new lines
		k_memset((void*) shell_framebuffer,0,320*240*2);
		line = 0;
	}
	char str[200];
	k_memset(str,'\0',190);
	va_list va;
	va_start(va,b);
	sprintf_safe_va(str,string,180,va);
	va_end(va);
	framebuffer_write10pstring_ascii(str,shell_framebuffer,0,line,r,g,b,ascii10p);
	line += 10;
	char *strpart = str+32;
	while (k_strlen(strpart,190) > 0)
	{
		framebuffer_write10pstring_ascii(strpart,shell_framebuffer,10,line,r,g,b,ascii10p);
		line += 10;
		if (line >= 240)
		{
			keypad_press_release_barrier(); // clear the screen after the user pressed a key to make place for new lines
			k_memset(shell_framebuffer,0,320*240*2);
			line = 0;
		}
		uint32_t add = k_strlen(strpart,190);
		if (add > 31)
			add = 31;
		strpart += add;
	}
	// also print via uart when debug enabled
	clear_caches();
	DEBUGPRINTLN_1(str)
	#endif
}
void debug_shell_reset()
{
	line = 0;
}
void debug_shell_println(char* string,...)
{
	#ifndef RELEASE
	if (line >= 240)
	{
		keypad_press_release_barrier(); // clear the screen after the user pressed a key to make place for new lines
		k_memset(shell_framebuffer,0,320*240*2);
		line = 0;
	}
	char str[200];
	k_memset(str,'\0',190);
	va_list va;
	va_start(va,string);
	sprintf_safe_va(str,string,180,va);
	va_end(va);
	framebuffer_write10pstring_ascii(str,shell_framebuffer,0,line,255,255,255,ascii10p);
	line += 10;
	char *strpart = str+32;
	while (k_strlen(strpart,190) > 0)
	{
		framebuffer_write10pstring_ascii(strpart,shell_framebuffer,10,line,255,255,255,ascii10p);
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
	clear_caches();
	// also print via uart when debug enabled
	DEBUGPRINTLN_1(str)
	#endif
}



void shell_println(char* string,...) // to allow showing text in release kernels
{
	if (line >= 240)
	{
		keypad_press_release_barrier(); // clear the screen after the user pressed a key to make place for new lines
		k_memset(shell_framebuffer,0,320*240*2);
		line = 0;
	}
	char str[200];
	k_memset(str,'\0',190);
	va_list va;
	va_start(va,string);
	sprintf_safe_va(str,string,180,va);
	va_end(va);
	framebuffer_write10pstring_ascii(str,shell_framebuffer,0,line,255,255,255,ascii10p);
	line += 10;
	char *strpart = str+32;
	while (k_strlen(strpart,190) > 0)
	{
		framebuffer_write10pstring_ascii(strpart,shell_framebuffer,10,line,255,255,255,ascii10p);
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
	clear_caches();
	// also print via uart when debug enabled
	DEBUGPRINTLN_1(str)
}


void shell_println_rgb(char* string,uint32_t r, uint32_t g,uint32_t b,...)
{
	if (line >= 240)
	{
		keypad_press_release_barrier(); // clear the screen after the user pressed a key to make place for new lines
		k_memset((void*) shell_framebuffer,0,320*240*2);
		line = 0;
	}
	char str[200];
	k_memset(str,'\0',190);
	va_list va;
	va_start(va,b);
	sprintf_safe_va(str,string,180,va);
	va_end(va);
	framebuffer_write10pstring_ascii(str,shell_framebuffer,0,line,r,g,b,ascii10p);
	line += 10;
	char *strpart = str+32;
	while (k_strlen(strpart,190) > 0)
	{
		framebuffer_write10pstring_ascii(strpart,shell_framebuffer,10,line,r,g,b,ascii10p);
		line += 10;
		if (line >= 240)
		{
			keypad_press_release_barrier(); // clear the screen after the user pressed a key to make place for new lines
			k_memset(shell_framebuffer,0,320*240*2);
			line = 0;
		}
		uint32_t add = k_strlen(strpart,190);
		if (add > 31)
			add = 31;
		strpart += add;
	}
	// also print via uart when debug enabled
	clear_caches();
	DEBUGPRINTLN_1(str)
}


















