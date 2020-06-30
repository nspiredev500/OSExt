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



static void testfunc()
{
	NUC_FILE* f = nuc_fopen("/documents/osext.tns","rb");
	char buffer[11];
	k_memset(buffer,'\0',11);
	if (f != NULL)
	{
		nuc_fread(buffer,10,1,f);
		debug_shell_println("buffer: %s",buffer);
		nuc_fclose(f);
	}
	debug_shell_println("syscall successfull!");
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
		bool b = (bool) call_with_stack((void*)(remapped_stack),run_self_test);
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
	
	
	/*
	int64_t milis = systime_unix_milis(), micro = systime_unix_micro();
	uint32_t ticks = 0xffffffff - timer_value(SYSTIME_TIMER);
	
	debug_shell_println("unix milliseconds: %lld",milis);
	debug_shell_println("unix microseconds: %lld",micro);
	debug_shell_println("ticks: %d",ticks);
	
	
	
	
	debug_shell_println_rgb("osext installed",0,255,0);
	debug_shell_println_rgb("press any key to exit",0,255,0);
	*/
	
	
	
	
	// to test remade msleep and the new microsleep
	/*
	debug_shell_println("testing msleep");
	
	
	void* backbuffer = get_back_framebuffer_address();
	void* frontbuffer = get_front_framebuffer_address();
	k_memcpy(backbuffer,frontbuffer,320*240*2);
	keypad_press_release_barrier();
	
	framebuffer_fillrect(frontbuffer,0,0,320,240,255,0,0);
	clear_caches();
	msleep(1000);
	
	framebuffer_fillrect(frontbuffer,0,0,320,240,0,255,0);
	clear_caches();
	msleep(1000);
	
	framebuffer_fillrect(frontbuffer,0,0,320,240,0,0,255);
	clear_caches();
	msleep(1000);
	
	k_memcpy(frontbuffer,backbuffer,320*240*2);
	
	
	
	
	debug_shell_println("testing microsleep");
	k_memcpy(backbuffer,frontbuffer,320*240*2);
	keypad_press_release_barrier();
	
	
	framebuffer_fillrect(frontbuffer,0,0,320,240,255,0,0);
	clear_caches();
	microsleep(1000000);
	
	framebuffer_fillrect(frontbuffer,0,0,320,240,0,255,0);
	clear_caches();
	microsleep(1000000);
	
	framebuffer_fillrect(frontbuffer,0,0,320,240,0,0,255);
	clear_caches();
	microsleep(1000000);
	
	
	k_memcpy(frontbuffer,backbuffer,320*240*2);
	*/
	
	
	
	
	/// Even file syscalls work in a relocated stack, even when using virtual memory buffers! 
	// testing if using a relocated stack is possible,
	// it seems it just has to be big enough for syscalls to not smash the thing under the stack
	// 20 pages left should be safe
	// if using relocated stacks, make sure to add a check for enough stack space left before using syscalls
	/*
	debug_shell_println("allocating buffer");
	uint32_t testbuff_int = (uint32_t) ti_malloc(SMALL_PAGE_SIZE*20);
	void* testbuff = (void*) ((testbuff_int & (~ 0b111))+0b1000); // forces alignment to 8 bytes
	debug_shell_println("under testbuff: 0x%x",*((uint8_t*)testbuff_int-1));
	call_with_stack(testbuff+SMALL_PAGE_SIZE*14,testfunc);
	debug_shell_println("under testbuff: 0x%x",*((uint8_t*)testbuff_int-1));
	debug_shell_println("freeing buffer");
	ti_free((void*)testbuff_int);
	*/
	
	
	// test the systime interrupt
	
	k_memset(get_front_framebuffer_address(),0,320*240*2);
	debug_shell_reset();
	
	/*
	debug_shell_println("testing systime interrupts");
	
	
	rtc_set_value(0xffffffff-1);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(500);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(500);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(100);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(100);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(300);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(200);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(200);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(100);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(200);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(200);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(200);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(200);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(200);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(200);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(200);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(200);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(200);
	debug_shell_println("seconds: 0x%llx",systime_unix());
	msleep(200);
	
	
	
	debug_shell_println_rgb("press any key to continue",0,255,0);
	keypad_press_release_barrier();
	k_memset(get_front_framebuffer_address(),0,320*240*2);
	debug_shell_reset();
	*/
	
	
	
	
	/*
	#undef TESTTIMER
	#define TESTTIMER 1,0

	debug_shell_println("testing milisecond timer interrupt:");

	timer_set_load(TESTTIMER,32*1000);
	msleep(100);
	int64_t t1 = systime_unix_milis();
	debug_shell_println("unix milliseconds: %lld",t1);
	msleep(2000);
	int64_t t2 = systime_unix_milis();
	debug_shell_println("unix milliseconds: %lld",t2);
	debug_shell_println("delta: %lld",t2-t1);
	
	
	
	debug_shell_println_rgb("press any key to continue",0,255,0);
	keypad_press_release_barrier();
	k_memset(get_front_framebuffer_address(),0,320*240*2);
	debug_shell_reset();
	
	t1 = systime_unix_milis();
	msleep(1000);
	t2 = systime_unix_milis();
	
	debug_shell_println("unix milliseconds: %lld",t1);
	debug_shell_println("unix milliseconds: %lld",systime_unix_milis()-t1);
	
	
	debug_shell_println_rgb("press any key to continue",0,255,0);
	keypad_press_release_barrier();
	k_memset(get_front_framebuffer_address(),0,320*240*2);
	debug_shell_reset();
	
	t1 = systime_unix_micro();
	msleep(1000);
	t2 = systime_unix_micro();
	debug_shell_println("unix microseconds: %lld",t1);
	debug_shell_println("unix microseconds: %lld",systime_unix_micro()-t1);
	*/
	
	/*
	k_memset(get_front_framebuffer_address(),0,320*240*2);
	debug_shell_reset();

	debug_shell_println("msleep test:");
	debug_shell_println("please use a stopwatch to time");
	debug_shell_println("the interval between the flashes");
	debug_shell_println_rgb("press any key to start the",0,255,0);
	debug_shell_println_rgb("msleep test",0,255,0);
	keypad_press_release_barrier();


	for (uint32_t i = 0;i<10;i++)
	{
		uint32_t r,g,b;
		if (i % 3 == 0)
		{
			r = 255;
			g = 0;
			b = 0;
		}
		if (i % 3 == 1)
		{
			r = 0;
			g = 255;
			b = 0;
		}
		if (i % 3 == 2)
		{
			r = 0;
			g = 0;
			b = 255;
		}
		framebuffer_fillrect(get_front_framebuffer_address(),0,0,320,240,r,g,b);
		
		
		
		msleep(1000);
	}
	
	
	
	k_memset(get_front_framebuffer_address(),0,320*240*2);
	debug_shell_reset();
	
	
	#undef TESTTIMER
	#define TESTTIMER 1,0

	debug_shell_println("first timer:");
	debug_shell_println("initial clockselect: 0x%x",timer_get_clockselect(TESTTIMER));
	
	timer_disable(TESTTIMER);
	timer_set_irq_enabled(TESTTIMER,false);
	timer_set_mode(TESTTIMER,0);
	timer_set_oneshot(TESTTIMER,false);
	timer_set_prescaler(TESTTIMER,0);
	timer_set_size(TESTTIMER,1);
	timer_set_clockselect(TESTTIMER,0);
	timer_set_load(TESTTIMER,0xffffffff);
	timer_enable(TESTTIMER);

	msleep(1000);
	{
		uint32_t t = 0xffffffff-timer_value(TESTTIMER);
		debug_shell_println("value: %d",t);
	}

	timer_disable(TESTTIMER);


	debug_shell_println_rgb("press any key to continue",0,255,0);
	keypad_press_release_barrier();


	k_memset(get_front_framebuffer_address(),0,320*240*2);
	debug_shell_reset();


	debug_shell_println("first timer with clockselect=0xA");
	


	timer_disable(TESTTIMER);
	timer_set_irq_enabled(TESTTIMER,false);
	timer_set_mode(TESTTIMER,0);
	timer_set_oneshot(TESTTIMER,false);
	timer_set_prescaler(TESTTIMER,0);
	timer_set_size(TESTTIMER,1);
	timer_set_clockselect(TESTTIMER,0xa);
	timer_set_load(TESTTIMER,0xffffffff);
	timer_enable(TESTTIMER);

	msleep(1000);
	{
		uint32_t t = 0xffffffff-timer_value(TESTTIMER);
		debug_shell_println("value: %d",t);
	}

	timer_disable(TESTTIMER);
	
	
	debug_shell_println_rgb("press any key to continue",0,255,0);
	keypad_press_release_barrier();


	k_memset(get_front_framebuffer_address(),0,320*240*2);
	debug_shell_reset();


	debug_shell_println("first timer with clockselect=0x9");


	timer_disable(TESTTIMER);
	timer_set_irq_enabled(TESTTIMER,false);
	timer_set_mode(TESTTIMER,0);
	timer_set_oneshot(TESTTIMER,false);
	timer_set_prescaler(TESTTIMER,0);
	timer_set_size(TESTTIMER,1);
	timer_set_clockselect(TESTTIMER,0x9);
	timer_set_load(TESTTIMER,0xffffffff);
	timer_enable(TESTTIMER);

	msleep(1000);
	{
		uint32_t t = 0xffffffff-timer_value(TESTTIMER);
		debug_shell_println("value: %d",t);
	}

	timer_disable(TESTTIMER);
	*/
	
	/*
	debug_shell_println_rgb("please press the reset button",0,255,0);
	while (true)
	{
		keypad_press_release_barrier();
	}
	*/
	
	
	
	
	/*
	int64_t milis = systime_unix_milis(), micro = systime_unix_micro();
	uint32_t ticks = 0xffffffff - timer_value(SYSTIME_TIMER);

	debug_shell_println("unix milliseconds: %lld",milis);
	debug_shell_println("unix microseconds: %lld",micro);
	debug_shell_println("ticks: %d",ticks);
	*/
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	debug_shell_println_rgb("osext installed",0,255,0);
	debug_shell_println_rgb("press any key to exit",0,255,0);
	
	#ifndef RELEASE
		// to be able to read the messages
		keypad_press_release_barrier();
	#endif
	
	
	
	
	
	enableIRQ();
}




























