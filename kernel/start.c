#include "kernel.h"






asm(
".extern (main)\n"
"__init_lr: .word 0 \n"
"__init_sp: .word 0 \n"
"__entry: .global __entry\n"
//" str lr, __init_lr\n"
//" str sp, __init_sp\n"
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
	
	DEBUGPRINTLN_1("virt base: 0x%x",virtual_base_address)
	
	void *got_offset = (void*) ((&_GOT_START)-(&_EXEC_START));
	
	DEBUGPRINTLN_1("\ngot offset: 0x%x",got_offset)
	DEBUGPRINTLN_1("start address: 0x%x",(&_EXEC_START))
	DEBUGPRINTLN_1("got address: 0x%x\n",(&_GOT_START))
	
	
	/*
	void *got_offset = (void*) ((&_GOT_START)-(&_EXEC_START));
	uint32_t *got_entry = (uint32_t*) ((uint32_t)(&_EXEC_START)+(uint32_t)got_offset);
	
	while (*got_entry != 0xFFFFFFFF)
	{
		DEBUGPRINTLN_1("got entry: 0x%x, address: 0x%x",*got_entry,got_entry);
		got_entry++;
	}
	*/
	
	if (argsn == 1 && ((unsigned int) argc) == 0x1234abcd) //test for running elf files
	{
		return 100;
	}
	
	if (argsn == 1 && ((unsigned int) argc) == 0x53544c41) //STandaloneLAunch
	{
		DEBUGPRINTF_1("relocated\n")
		// relocation finished or already done by loader
		initialize();
	}
	else
	{
		
		// detecting OSExt:
		/*
		register uint32_t control_reg asm("r0");
		asm volatile("mrc p15, 0, r0, c1, c0, 0":"=r" (control_reg)::); // reading co-processor register 1
		if ((control_reg & (1 << 13)) == (1 << 13)) // if bit 13 is a 1, interrupt vector are mapped high. I don't think anything other than OSExt does this
		{
			register uint32_t tt_base asm("r0");
			asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base)); // get the translation table base register
			
			tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
			uint32_t *tt = (uint32_t*) tt_base;
			if (tt[(0xe0000000) >> 20] != 0) // if there is something mapped at virtual address 0xe0000000 it should be OSExt, I haven't seen any other program map memory like that
			{
				// OSExt is installed
			}
		}
		*/
		
		
		
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
	
	
	
	
	
	
	
	//usb_print_id_registers();
	// usb driver in-progress
	/*
	debug_shell_println("saving usb state");
	struct usb_state state;
	usb_save_state(&state);
	
	debug_shell_println("resetting usb controller");
	usb_reset();
	
	
	debug_shell_println("restoring usb state");
	usb_restore_state(&state);
	*/
	
	/*
	NUC_DIR* dir = nuc_opendir("/documents/");
	debug_shell_println("iterating over documents");
	if (dir != NULL)
	{
		struct nuc_dirent* dirent;
		while ((dirent = nuc_readdir(dir)) != NULL)
		{
			struct nuc_stat stat;
			if (k_streq(dirent->d_name,".",1) || k_streq(dirent->d_name,"..",2))
			{
				continue;
			}
			debug_shell_println("entry: %s",dirent->d_name);
			char buffer[100];
			k_memset(buffer,'\0',100);
			k_memcpy(buffer,"/documents/",k_strlen("/documents/",100));
			uint32_t index = k_strlen("/documents/",100);
			k_memcpy(buffer+index,dirent->d_name,k_strlen(dirent->d_name,50));
			if (nuc_stat(buffer,&stat) == 0)
			{
				debug_shell_println("size: %d",stat.st_size);
			}
		}
		nuc_closedir(dir);
	}
	debug_shell_println("done iterating over documents");
	*/
	
	//debug_shell_println("VSYS: %Lf",adc_read_channel(CHANNEL_VSYS));
	
	
	
	int64_t milis = systime_unix_milis(), micro = systime_unix_micro();
	uint32_t ticks = 0xffffffff - timer_value(SYSTIME_TIMER);
	
	debug_shell_println("unix milliseconds: %lld",milis);
	debug_shell_println("unix microseconds: %lld",micro);
	debug_shell_println("ticks: %d",ticks);
	
	
	
	
	debug_shell_println_rgb("osext installed",0,255,0);
	debug_shell_println_rgb("press any key to exit",0,255,0);
	
	
	
	#ifndef RELEASE
		// to be able to read the messages
		keypad_press_release_barrier();
	#endif
	
	
	
	
	
	enableIRQ();
}




























