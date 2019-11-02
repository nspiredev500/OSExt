
#include <hook.h>
#include <time.h>
#include <SDL/SDL_config.h>
#include <SDL/SDL.h>
#include <ngc.h>
#include <os.h>
#include <usbdi.h>
#include <usb.h>
//#include <nspireio2.h> //printf doesn't default to serial port with nio2


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






//SDL doesn't work, maybe because it's in hooks?



*/

//unsigned volatile int *timer;


// from ndless sdk utils.c
void ut_disable_watchdog(void) {
	// Disable the watchdog on CX that may trigger a reset
	*(volatile unsigned*)0x90060C00 = 0x1ACCE551; // enable write access to all other watchdog registers
	*(volatile unsigned*)0x90060008 = 0; // disable reset, counter and interrupt
	*(volatile unsigned*)0x90060C00 = 0; // disable write access to all other watchdog registers
}


time_t lasthook;


int hookcount = 0;
int hookmax = 1000;
HOOK_DEFINE(testhook)
{
	//bkpt();
	/*
	FILE *f = fopen("/test","w");
	if (f != NULL)
		fclose(f);
	*/
	/*
	if (hookcount != hookmax)
	{
		hookcount++;
		HOOK_RESTORE_RETURN(testhook);
	}
	*/
	
	//bkpt();
	
	if (isKeyPressed(KEY_NSPIRE_ESC) && isKeyPressed(KEY_NSPIRE_HOME))
	{
		
	}
	time_t chook = time(NULL);
	if (chook-lasthook >= 0 && chook-lasthook <= 2)
	{
		HOOK_RESTORE_RETURN(testhook);
	}
	ut_disable_watchdog();
	int intmask = TCT_Local_Control_Interrupts(-1);
	bool ctrl = isKeyPressed(KEY_NSPIRE_CTRL);
	lasthook = chook;
	#ifdef MODULE_DESKTOP
	if (ctrl && isKeyPressed(KEY_NSPIRE_COMMA))
	{
		desktop();
		TCT_Local_Control_Interrupts(intmask);
		HOOK_RESTORE_RETURN(testhook);
	}
	#endif
	#ifdef MODULE_SHELL
	if (ctrl && isKeyPressed(KEY_NSPIRE_PI))
	{
		shell();
		TCT_Local_Control_Interrupts(intmask);
		HOOK_RESTORE_RETURN(testhook);
	}
	#endif
	#ifdef MODULE_CLOCK
	bool ee = isKeyPressed(KEY_NSPIRE_EE);
	if (ctrl && ee && isKeyPressed(KEY_NSPIRE_G))
	{
		settime(); // calc hanging in loop after short while without esc working
		TCT_Local_Control_Interrupts(intmask);
		HOOK_RESTORE_RETURN(testhook);
	}
	if (ctrl && ee)
	{
		miniclock_enabled = ! miniclock_enabled;
		TCT_Local_Control_Interrupts(intmask);
		HOOK_RESTORE_RETURN(testhook);
	}
	#endif
	
	
	
	TCT_Local_Control_Interrupts(intmask);
	HOOK_RESTORE_RETURN(testhook);
};





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
	initOSGCBUFF();
	
	/*
	*(volatile unsigned *)0x900B0018 &= ~(1 << 11);
	*(volatile unsigned *)0x900C0080 = 0xA;
	volatile unsigned *control = (unsigned *)0x900C0008;
	*control = 0b10000010;
	timer = (unsigned *)0x900C0004;
	*/
	
	lasthook = time(NULL);
	
	/*
	Gc gc = gui_gc_global_GC();
	printf("%x\n",gc);
	printf("%x\n",*gc);
	printf("%x\n",**(int**)gc);
	*/
	
	
	
	
	
	#ifdef MODULE_SETTINGS
		loadSettings();
	#endif
	
	#ifdef MODULE_CLOCK
		hook_minicklock();
	#endif
	
	HOOK_INSTALL(nl_osvalue(hook_addrs,32),testhook);
	
	
	
	
	
	
	
	
	#ifdef MODULE_DISABLENAVNET
		#ifdef MODULE_SETTINGS
			int navnetstatus = getSetting("navnet");
			//printf("%d",navnetstatus);
			if (navnetstatus != -1)
				printf("navnet %d\n",IntSetting(navnetstatus));
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
			printf("no settings module\n");
			disablenavnet();
		#endif
	#endif
	
	
	
	
	
	nl_set_resident();
	#ifdef USBTEST_H
		ums_register();
	#endif
	
	clear_cache();
	
	return 0;
}


















































