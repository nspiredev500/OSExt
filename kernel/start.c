#include "kernel.h"






asm(
".extern (main)\n"
"__init_lr: .word 0 \n"
"__init_sp: .word 0 \n"
"__entry: .global __entry\n"
" str lr, __init_lr\n"
" str sp, __init_sp\n"
" b main"
);








// some variables to test relocation
static uint32_t test = 20;

static uint32_t *test2 = &test;

static uint32_t *test3 = NULL;
// little endian!
// stack grows downwards!
/*
void teststack(void* t)
{
	int h = 1;
	if (&h < t)
	{
		uart_printf("stack grows downwards!\n");
	}
	else
	{
		uart_printf("stack grows upwards!\n");
	}
}
*/
// because we return with main, every error here is still recoverable without a kernel panic
int main(int argsn,char **argc)
{
	// no need to make the kernel resident, allocated memory isn't freed by ndless, so we can just copy the kernel and it will stay
	
	
	if (argsn == 1 && ((unsigned int) argc) == 0x53544c41) //STandaloneLAunch
	{
		DEBUGPRINTF_1("relocated\n")
		// relocation finished or already done by loader
		initialize();
	}
	else
	{
		
		register uint32_t control_reg asm("r0");
		asm volatile("mrc p15, 0, r0, c1, c0, 0":"=r" (control_reg)::);
		if ((control_reg & (1 << 13)) == (1 << 13))
		{
			register uint32_t tt_base asm("r0");
			asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
			
			tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
			uint32_t *tt = (uint32_t*) tt_base;
			if (tt[((uint32_t) virtual_base_address) >> 20] != 0)
			{
				// kernel base address already used and interrupt vector high probably means osext is already installed
				// so uninstall it
				bool irq = isIRQ();
				if (irq)
				{
					disableIRQ();
				}
				asm volatile("swi 0xf80000\n":"=r" (control_reg)::); // use the uninstall syscall
				if (control_reg != 0)
				{
					DEBUGPRINTLN_1("old kernel base: 0x%x",control_reg);
					// clean the virtual address space
					for (uint32_t i = (uint32_t) virtual_base_address;i<0xfff00000;i+=SECTION_SIZE)
					{
						tt[i >> 20] = 0;
					}
					// free the old kernel
					ti_free((void*) control_reg);
				}
				else
				{
					DEBUGPRINTLN_1("uninstall not successful");
					if (irq)
					{
						enableIRQ();
					}
					// uninstallation could not be done
					return 0xDEAD;
				}
				if (irq)
				{
					enableIRQ();
				}
			}
		}
		
		
		
		
		relocate_self();
	}
	return 0;
}
// from ndless sdk utils.c
static void ut_disable_watchdog(void)
{
	// Disable the watchdog on CX that may trigger a reset
	*(volatile unsigned*)0x90060C00 = 0x1ACCE551; // enable write access to all other watchdog registers
	*(volatile unsigned*)0x90060008 = 0; // disable reset, counter and interrupt
	*(volatile unsigned*)0x90060C00 = 0; // disable write access to all other watchdog registers
}


// because we return with main after this, every error here is still recoverable without a kernel panic
void initialize()
{
	disableIRQ();
	ut_disable_watchdog();
	
	
	if (! init_kernel())
	{
		return;
	}
	
	debug_shell_println("running general self-test");
	
	
	
	#ifndef RELEASE
		bool b = (bool) call_with_stack((void*)(0xe8000000+SMALL_PAGE_SIZE-8),run_self_test);
		if (! b)
		{
			debug_shell_println_rgb("error in general self-test         aborting",255,0,0);
			keypad_press_release_barrier();
			enableIRQ();
			return;
		}
	#endif
	
	
	
	
	
	debug_shell_println("searching background image...");
	
	NUC_FILE *f = nuc_fopen("/documents/background.bmp.tns","rb");
	if (f != NULL)
	{
		debug_shell_println("background image found");
		struct img565 *img = load_bmp_file(f);
		if (img != NULL)
		{
			debug_shell_println_rgb("background image loaded",0,0,255);
			background_set_image(img);
		}
		else
		{
			debug_shell_println_rgb("background image could not be loaded",255,0,0);
		}
		nuc_fclose(f);
	}
	background_update();
	
	
	
	rtc_set_alarm(0xffffffff);
	rtc_set_value(0xffffffff-10);
	
	while (timer_seconds_overflows == 0)
	{
		wait_for_interrupt();
	}
	
	
	debug_shell_println_rgb("osext installed",0,255,0);
	debug_shell_println_rgb("press any key to exit",0,255,0);
	
	
	
	#ifndef RELEASE
		// to be able to read the messages
		keypad_press_release_barrier();
	#endif
	
	
	
	
	
	enableIRQ();
}




























