#include "kernel.h"






asm(
".extern (main)\n"
"__init_lr: .word 0 \n"
"__init_sp: .word 0 \n"
"__entry: .global __entry\n"
//" .long 0xE1212374\n" // bkpt
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
				asm volatile("swi 0xf80000":"=r" (control_reg)::); // use the uninstall syscall
				if (control_reg != 0)
				{
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
					// uninstallation could not be done
					return 0xDEAD;
				}
			}
		}
		
		
		
		
		relocate_self();
	}
	return 0;
}



// because we return with main after this, every error here is still recoverable without a kernel panic
void initialize()
{
	void* framebuffer = (void*) *LCD_UPBASE;
	framebuffer_fillrect(framebuffer,0,0,320,240,0,0,0);
	setShellFramebuffer(framebuffer);
	debug_shell_println("finished relocating");
	debug_shell_println("initializing");
	
	
	
	debug_shell_println("malloc pointer: 0x%x",getKernelMallocedPointer());
	
	debug_shell_println("kernel_start: 0x%x",&_EXEC_START);
	init_call_with_stack(&_EXEC_START);
	
	
	debug_shell_println("performing physical memory manager self-test");
	bool b = physical_mm_self_test();
	if (! b)
	{
		debug_shell_println_rgb("error in physical memory manager self-test         aborting",255,0,0);
		keypad_press_release_barrier();
		free_init_pds();
		return;
	}
	
	debug_shell_println("allocating memory");
	allocPageblock(128);
	allocPageblock(128);
	allocPageblock(128);
	allocPageblock(128); // allocate 2mb
	debug_shell_println("done");
	
	
	/*
	register uint32_t domains asm("r0") = 0;
	asm volatile("mrc p15, 0, r0, c3, c0, 0":"=r" (domains));
	DEBUGPRINTF_3("domains: 0x%x\n",domains); // domain 0 is client, so we can use it for everything, because access permissions are checked
	*/
	
	debug_shell_println("performing virtual memory manager self-test");
	b = virtual_mm_self_test();
	if (! b)
	{
		debug_shell_println_rgb("error in virtual memory manager self-test         aborting",255,0,0);
		keypad_press_release_barrier();
		free_init_pds();
		return;
	}
	debug_shell_println("performing slab allocator self-test");
	b = slab_allocator_self_test_pre_initialization();
	if (! b)
	{
		debug_shell_println_rgb("error in slab allocator self-test         aborting",255,0,0);
		keypad_press_release_barrier();
		free_init_pds();
		return;
	}
	
	debug_shell_println("initializing kernel space");
	
	
	initializeKernelSpace();
	
	free_init_pds();
	
	
	debug_shell_println("done");
	
	
	if (! install_exception_handlers())
	{
		debug_shell_println_rgb("no page for exception vectors         aborting",255,0,0);
		keypad_press_release_barrier();
		return;
	}
	
	
	//asm(".long 0xE1212374"); // bkpt
	
	debug_shell_println("switching framebuffer, press any key");
	keypad_press_release_barrier();
	
	
	
	
	//keypad_press_release_barrier();
	
	
	
	
	initLCDDriver();
	claimLCD();
	
	//keypad_press_release_barrier();
	
	debug_shell_reset();
	setShellFramebuffer(get_front_framebuffer_address());
	debug_shell_println("new framebuffer: 0x%x",*LCD_UPBASE);
	
	
	debug_shell_println("running general self-test");
	
	
	//keypad_press_release_barrier();
	// construct the pages for the remapped stack
	void* page = usePage();
	if (page == NULL)
	{
		debug_shell_println_rgb("no page for general self test         aborting",255,0,0);
		keypad_press_release_barrier();
		return;
	}
	addVirtualKernelPage(page,(void*) 0xe8000000);
	page = usePage();
	if (page == NULL)
	{
		debug_shell_println_rgb("no page for general self test         aborting",255,0,0);
		keypad_press_release_barrier();
		return;
	}
	addVirtualKernelPage(page,(void*) (0xe8000000+SMALL_PAGE_SIZE));
	
	b = (bool) call_with_stack((void*)(0xe8000000+SMALL_PAGE_SIZE-8),run_self_test);
	
	//setPageUsedBit(page,false);
	
	
	//b = true;
	if (! b)
	{
		debug_shell_println_rgb("error in general self-test         aborting",255,0,0);
		keypad_press_release_barrier();
		return;
	}
	
	
	
	
	debug_shell_println("installing hooks");
	install_hooks();
	
	
	/*
	debug_shell_println("double: %Lf",12.0);
	debug_shell_println("vbatt: %Lfv",adc_read_channel(1));
	debug_shell_println("vsys: %Lfv",adc_read_channel(2));
	debug_shell_println("b12: %Lfv",adc_read_channel(4));
	*/
	
	
	/*
	debug_shell_println("vbatt: %dv",(uint32_t)adc_read_channel(1));
	debug_shell_println("vsys: %dv",(uint32_t)adc_read_channel(2));
	debug_shell_println("b12: %dv",(uint32_t)adc_read_channel(4));
	*/
	
	debug_shell_println_rgb("osext installed",0,255,0);
	debug_shell_println_rgb("press any key to exit",0,255,0);
	// to be able to read the messages
	keypad_press_release_barrier();
	
	//freeLCD();
	
	
	// unmap the lcd controller to test when the os modifies LCD_UPBASE
	/*
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	uint32_t *tt = (uint32_t*) tt_base;
	
	
	tt[((uint32_t)LCD_UPBASE)>>20] = 0;
	*/
	/*
	uint32_t* os_lcd_writes = (uint32_t*) 0x100237c8;
	os_lcd_writes[0] = 0xe1a00000; // nop
	os_lcd_writes[1] = 0xe1a00000; // nop
	os_lcd_writes[2] = 0xe1a00000; // nop
	os_lcd_writes[3] = 0xe1a00000; // nop
	*/
	
	
	
	
	
	
	
	
	
	//asm(".long 0xE1212374"); // bkpt
}




























