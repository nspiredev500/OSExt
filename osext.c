
#include <hook.h>
#include <time.h>
#include <SDL/SDL_config.h>
#include <SDL/SDL.h>
#include <ngc.h>
#include <os.h>
#include <usbdi.h>
#include <usb.h>
#include <nspireio2.h>

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



time_t lasthook;
HOOK_DEFINE(testhook)
{
	//bkpt();
	time_t chook = time(NULL);
	if (chook-lasthook >= 0 && chook-lasthook <= 2)
	{
		HOOK_RESTORE_RETURN(testhook);
	}
	lasthook = chook;
	#ifdef MODULE_DESKTOP
	if (isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_COMMA))
	{
		desktop();
		HOOK_RESTORE_RETURN(testhook);
	}
	#endif
	#ifdef MODULE_SHELL
	if (isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_PI))
	{
		shell();
		HOOK_RESTORE_RETURN(testhook);
	}
	#endif
	#ifdef MODULE_CLOCK
	if (isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_EE) && isKeyPressed(KEY_NSPIRE_G))
	{
		settime();
		HOOK_RESTORE_RETURN(testhook);
	}
	if (isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_EE))
	{
		miniclock_enabled = ! miniclock_enabled;
		HOOK_RESTORE_RETURN(testhook);
	}
	#endif
	
	
	HOOK_RESTORE_RETURN(testhook);
};


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
};
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



void test()
{
	bkpt();
}



int main()
{
	
	lasthook = time(NULL);
	hook_minicklock();
	
	
	
	HOOK_INSTALL(nl_osvalue(hook_addrs,32),testhook);
	
	
	
	
	
	
	#ifdef DISABLENAVNET_H
		disablenavnet();
	#endif
	
	
	
	clear_cache();
	
	nl_set_resident();
	#ifdef USBTEST_H
		ums_register();
	#endif
	
	
	return 0;
}


















































