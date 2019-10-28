
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


void enableWatchdog();




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

extern volatile int savedregs;



time_t lasthook;

HOOK_DEFINE(testhook)
{
	/*
	//bkpt();
	#ifdef MODULE_CLOCK
		drawclock();
	#endif
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
	*/
	enableWatchdog();
	HOOK_RESTORE_RETURN(testhook);
};



void checker()
{
	#ifdef MODULE_CLOCK
		drawclock();
	#endif
	time_t chook = time(NULL);
	if (chook-lasthook >= 0 && chook-lasthook <= 2)
	{
		return;
	}
	lasthook = chook;
	#ifdef MODULE_DESKTOP
	if (isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_COMMA))
	{
		desktop();
		return;
	}
	#endif
	#ifdef MODULE_SHELL
	if (isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_PI))
	{
		shell();
		return;
	}
	#endif
	#ifdef MODULE_CLOCK
	if (isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_EE) && isKeyPressed(KEY_NSPIRE_G))
	{
		settime();
		return;
	}
	if (isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_EE))
	{
		miniclock_enabled = ! miniclock_enabled;
		return;
	}
	#endif
}


static volatile uint32_t *watchdog_load = (uint32_t*) 0x90060000,
                         *watchdog_control = (uint32_t*) 0x90060008,
                         *watchdog_intclear = (uint32_t*) 0x9006000C,
                         *watchdog_lock = (uint32_t*) 0x90060C00;


/*
static __attribute__ ((interrupt("FIQ"))) void checker_fiq()
{
	bkpt();
	register volatile int test asm("r8");
	test = savedregs;
	
	//asm ("mov %1, %0\n\t add $1, %0" : "=r" (dst) : "r" (src));
	asm volatile ("stmia r8, {r0,r1,r2,r3,r4,r5,r6,r7}\n");
	//asm volatile ("stmia %0, {r0-r7,sp,lr}"::"r8" (savedregs):"r8");
	
	bkpt();
	unsigned int a;
	
	//__asm__ __volatile__("");
	
	checker();
	
	
	*watchdog_lock = 0x1ACCE551;
    *watchdog_intclear = 1;
	
	
	
	
}
*/

void ackfiq()
{
	*watchdog_lock = 0x1ACCE551;
    *watchdog_intclear = 1;
	
	
}



/*
asm(
"sp_svc: .word 0\n"
"lr_svc: .word 0\n"
"lcd_compat_abort_handler:\n"
"sub sp, sp, #8\n" // Somehow the OS uses this...
    "push {r0-r12, lr}\n"
        "mrs r0, spsr\n"
        "orr r0, r0, #0xc0\n"
        "msr cpsr_c, r0\n" // To SVC mode
            "str sp, sp_svc\n" // Save sp_svc
            "str lr, lr_svc\n" // Save lr_svc
        "msr cpsr_c, #0xd7\n" // Back to ABT mode
        "mov r0, sp\n" // First arg, array of r0-12, lr
        "bl lcd_compat_abort\n"
        "mrs r0, spsr\n"
        "orr r0, r0, #0xc0\n"
        "msr cpsr_c, r0\n" // To SVC mode
            "ldr sp, sp_svc\n" // Restore sp_svc
            "ldr lr, lr_svc\n" // Restore lr_svc
        "msr cpsr_c, #0xd7\n" // Back to ABT mode
    "pop {r0-r12, lr}\n"
"add sp, sp, #8\n"
"subs pc, lr, #4");
*/

extern checker_fiq;
extern exit_to_os;
asm(
"watchdoglock: .word 90060C00\n"
"watchdoglockval: .word 0x1ACCE551\n"
"watchdogintclear: .word 9006000C\n"
"savedregs: .word 0\n"//r0
"sr1: .word 0\n"//r1...
"sr2: .word 0\n"
"sr3: .word 0\n"
"sr4: .word 0\n"
"sr5: .word 0\n"
"sr6: .word 0\n"
"sr7: .word 0\n"
"sr8: .word 0\n"
"sr9: .word 0\n"
"sr10: .word 0\n"
"sr11: .word 0\n"
"sr12: .word 0\n"
"slr: .word 0\n"//lr
"spc: .word 0\n"//pc
"checker_fiq:\n"
".word e1212374" //breakpoint
"b ackfiq\n"
"subs pc, lr, #4\n"
"\n"
"\n"
"\n"
"\n");




static const unsigned int hook_addrs[] =
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x10023810, 0x100237d4,
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


void enableWatchdog()
{
	*watchdog_lock = 0x1ACCE551;
    *watchdog_load = 33000000 / 15; // 15 Hz
    *watchdog_control = 1;

    // Install FIQ handler
    *(volatile uint32_t*)0x3C = (uint32_t) checker_fiq;

    // Set first watchdog interrupt as FIQ
    *(volatile uint32_t*) 0xDC00000C = 1 << 3;
    // Activate watchdog IRQ
    *(volatile uint32_t*) 0xDC000010 = 1 << 3;

    // Enable FIQs
    uint32_t spsr;
    asm volatile("mrs %[spsr], spsr" : [spsr] "=r" (spsr));
    spsr &= ~0x40;
    asm volatile("msr spsr_c, %[spsr]" :: [spsr] "r" (spsr));
}


int main()
{
	
	lasthook = time(NULL);
	
	
	
	
	
	
	#ifdef MODULE_SETTINGS
		loadSettings();
	#endif
	
	#ifdef MODULE_CLOCK
		hook_minicklock();
	#endif
	
	HOOK_INSTALL(nl_osvalue(hook_addrs,32),testhook);
	
	enableWatchdog();
	
	
	
	
	
	
	
	
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
	
	
	
	clear_cache();
	
	nl_set_resident();
	#ifdef USBTEST_H
		ums_register();
	#endif
	
	
	return 0;
}


















































