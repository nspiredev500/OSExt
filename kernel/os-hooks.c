#include "kernel.h"

#define STRINGIFYMAGIC(x) #x
#define STRINGIFY(x) STRINGIFYMAGIC(x)

void (*clear_cache)(void) = clear_caches;
#include "hook.h"




/*
static void* drawhook_adrs[] = {
	0, 0, 0, 0, 0, 0, // 3.1 nonCAS/Cas, CX, CM
	0, 0, 0, 0, // 3.6
	0, 0, 0, 0, 0, 0,// 3.9
	0, 0, // 3.91 CX nonCAS/CAS
	0, 0, 0, 0, // 4.0
	0, 0, // 4.0.3
	0, 0, // 4.2
	0, 0, // 4.4
	0, 0, // 4.5
	0, 0, // 4.5.1
};
*/

// from ndless sdk utils.c
static void ut_disable_watchdog(void)
{
	// Disable the watchdog on CX that may trigger a reset
	*(volatile unsigned*)0x90060C00 = 0x1ACCE551; // enable write access to all other watchdog registers
	*(volatile unsigned*)0x90060008 = 0; // disable reset, counter and interrupt
	*(volatile unsigned*)0x90060C00 = 0; // disable write access to all other watchdog registers
}


static const uint32_t drawhook_addrs[] =
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,//0x10011178 works with fopen!   at osgc read
 0x10011178, 0x10011134,
 //0x100237d4,//0x10023810
 0x0, 0x0
};



static void hookfunc();

HOOK_DEFINE(drawhook)
{
	ut_disable_watchdog();
	int intmask = TCT_Local_Control_Interrupts(-1);
	
	call_with_stack((void*)(0xe8000000+SMALL_PAGE_SIZE-8),hookfunc);
	
	TCT_Local_Control_Interrupts(intmask);
	HOOK_RESTORE_RETURN(drawhook);
};


void hookfunc()
{
	
	void* framebuffer = (void*) *LCD_UPBASE;
	
	framebuffer_fillrect(framebuffer,0,0,320,240,0,0,0);
	clear_caches();
	
	
	
	
}

void install_hooks()
{
	uint32_t adr = os_value(drawhook_addrs,sizeof(drawhook_addrs)/sizeof(uint32_t));
	DEBUGPRINTLN_1("adr: 0x%x",adr)
	if (adr != 0)
	{
		HOOK_INSTALL(adr,drawhook);
	}
	
	
	
	
	
	
	
}
void uninstall_hooks()
{
	uint32_t adr = os_value(drawhook_addrs,sizeof(drawhook_addrs)/sizeof(unsigned int));
	if (adr != 0)
	{
		HOOK_UNINSTALL(adr,drawhook);
	}
	
	
	
	
	
	
}
	
	
	
	
	
	






