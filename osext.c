
#include <stdarg.h>
#include <hook.h>
#include <SDL/SDL_config.h>
#include <SDL/SDL.h>
#include <ngc.h>
#include <os.h>
#include <usbdi.h>
#include <usb.h>
#include <time.h>
#include <nspireio2.h> //printf doesn't default to serial port with nio2, use uart_printf









#define precisetime



#ifdef precisetime
	volatile unsigned int *timer;
	unsigned int lasttime = 0;
#else
	time_t lasthook;
#endif



void osgctest();

#include "config.h"






#include "modules/definitions.h"






/* 
keypad reads: 
os 4.5: 0x100e0f64	0x1070bd68
noncas 4.5: 0x100e1128


screen writes:
os 4.5: 0x10023798	0x100237d4	0x100237cc 0x10023808   ??: 0x1002380c 
noncas 4.5: 0x10023808 0x10023840	








*/



// from ndless sdk utils.c
void ut_disable_watchdog(void) {
	// Disable the watchdog on CX that may trigger a reset
	*(volatile unsigned*)0x90060C00 = 0x1ACCE551; // enable write access to all other watchdog registers
	*(volatile unsigned*)0x90060008 = 0; // disable reset, counter and interrupt
	*(volatile unsigned*)0x90060C00 = 0; // disable write access to all other watchdog registers
}





void hookfunc();



HOOK_DEFINE(testhook)
{
	
	
	ut_disable_watchdog();
	int intmask = TCT_Local_Control_Interrupts(-1);
	
	hookfunc();
	
	TCT_Local_Control_Interrupts(intmask);
	HOOK_RESTORE_RETURN(testhook);
};
#ifdef MODULE_CLOCK
	#ifdef precisetime
		unsigned int lastclock = 0;
	#else
		int hookcount = 0;
	#endif
#endif
bool cr4 = true;
void hookfunc()
{
	/*
	if (syscallsready)
	{
		bkpt();
		unsigned int nr = 0xF80000;
		register int r0 asm("r0");
		bkpt();
		asm volatile(
			"swi %[nr]\n"
			: "=r" (r0)
			: [nr] "i" (nr)
			: "memory", "r1", "r2", "r3", "r4", "r12", "lr");
			bkpt();
		uart_printf("return value: %d",r0);
		bkpt();
	}
	*/
	
	
	
	// for now draw the clock here if the screen isn't 240*320, because the miniclock hook doesn't get called
	if (! cr4)
	{
		#ifdef MODULE_CLOCK
			#ifdef precisetime
				unsigned int time = *timer;
				if (abs(time-lastclock) > 200)
				{
					drawclock();
					lastclock = *timer;
				}
			#else
				if (hookcount > 200)
				{
					hookcount++;
				}
				else
				{
					hookcount = 0;
					drawclock();
				}
			#endif
		#endif
	}
	
	#ifdef MODULE_SECURITY
		if (isKeyPressed(KEY_NSPIRE_ESC) && (isKeyPressed(KEY_NSPIRE_HOME) || on_key_pressed()))
		{
			ptt_pressed();
			wait_no_key_pressed();
		}
	#endif
	#ifndef precisetime
		time_t chook = time(NULL);
		if (chook-lasthook > 0)
		{
			return;
		}
		lasthook = chook;
	#else
		unsigned int time = *timer;
		if (abs(time-lasttime) < 500)
		{
			return;
		}
		lasttime = time;
	#endif
	bool ctrl = isKeyPressed(KEY_NSPIRE_CTRL);
	#ifdef MODULE_SECURITY
		if (ctrl && isKeyPressed(KEY_NSPIRE_SPACE))
		{
			lockScreen();
			wait_no_key_pressed();
			return;
		}
		testScreentime();
	#endif
	
	
	#ifdef MODULE_DESKTOP
		if (ctrl && isKeyPressed(KEY_NSPIRE_COMMA))
		{
			desktop();
			wait_no_key_pressed();
			return;
		}
	#endif
	#ifdef MODULE_SHELL
		if (ctrl && isKeyPressed(KEY_NSPIRE_PI))
		{
			shell();
			wait_no_key_pressed();
			return;
		}
	#endif
	#ifdef MODULE_CLOCK
		bool ee = isKeyPressed(KEY_NSPIRE_EE);
		if (ctrl && ee && isKeyPressed(KEY_NSPIRE_G))
		{
			settime();
			wait_no_key_pressed();
			return;
		}
		if (ctrl && ee)
		{
			miniclock_enabled = ! miniclock_enabled;
			return;
		}
	#endif
	
	
	
	
}



void osgctest() // to get osgc address
{
	Gc gc = gui_gc_global_GC();
	gui_gc_begin(gc);
	gui_gc_setColorRGB(gc,255,0,0);
	bkpt();
	gui_gc_fillRect(gc,0,0,100,100);
	bkpt();
	gui_gc_finish(gc);
}




static const unsigned int hook_addrs[] =
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



/*
// 100230dc		10011174
const unsigned int hook_addrs[] =
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x100e112C, 0x100e0f68,		//0x10011178
 0x0, 0x0
};*/
/* ndless adresses
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0x1002DE38, 0x1002DDC8, 0x1002D388, 0x1002D348,
 0x1002E2C0, 0x1002E224, 0x0, 0x0,
 0x0, 0x0, 0x1002D804, 0x1002D798,
 0x1002D818, 0x1002D7C0,
 0x1002F4EC, 0x1002F494,
 0x1002F92C, 0x1002F8D4,
 0x1002FF24, 0x1002FEC0,
 0x1003108C, 0x10031034,
 0x100310FC, 0x100310A4,
 0x100311C4, 0x10031164
};*/




int main()
{
	assert_ndless_rev(2014);
	
	
	
	initOSGCBUFF();
	#ifdef precisetime
		// init timer from nsSDL
		*(volatile unsigned *)0x900B0018 &= ~(1 << 11);
		*(volatile unsigned *)0x900C0080 = 0xA;
		volatile unsigned *control = (unsigned *)0x900C0008;
		*control = 0b10000010;
		timer = (unsigned *)0x900C0004;
	#else
		lasthook = time(NULL);
	#endif
	
	
	
	
	if (lcd_type() != SCR_240x320_565)
	{
		cr4 = false;
	}
	
	
	
	
	
	#ifdef MODULE_SETTINGS
		loadSettings();
	#endif
	
	#ifdef MODULE_SECURITY
		initlogin();
		lockScreen();
	#endif
	#ifdef MODULE_CLOCK
		hook_minicklock();
	#endif
	
	HOOK_INSTALL(nl_osvalue(hook_addrs,32),testhook);
	
	
	
	
	
	
	
	
	#ifdef MODULE_DISABLENAVNET
		#ifdef MODULE_SETTINGS
			int navnetstatus = getSetting("navnet");
			if (navnetstatus != -1 && IntSetting(navnetstatus) == 0)
			{
				int navnetnextboot = getSetting("navnet_next");
				if (navnetnextboot != -1)
				{
					if (IntSetting(navnetnextboot) == 0)
					{
						disablenavnet();
					}
					else
					{
						changeSetting("navnet_next",0);
						saveSettings();
					}
					
				}
			}
		#else
			disablenavnet();
		#endif
	#endif
	
	
	#ifdef MODULE_ADDSYSCALLS
		extendSWIHandler();
	#endif
	
	
	
	nl_set_resident();
	#ifdef USBTEST_H
		ums_register();
	#endif
	
	clear_cache();
	
	unsigned int nr = 0x7c000;
	register int r0 asm("r0");
	bkpt();
	asm volatile(
		"swi %[nr]\n"
		: "=r" (r0)
		: [nr] "i" (nr)
		: "memory", "r1", "r2", "r3", "r4", "r12", "lr");
	//bkpt();
	uart_printf("return value: %d",r0);
	
	return 0;
}


















































