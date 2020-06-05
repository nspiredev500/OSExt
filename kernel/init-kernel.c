#include "kernel.h"








bool init_kernel()
{
	charset_load();
	void* framebuffer = (void*) *LCD_UPBASE;
	#ifndef RELEASE
		framebuffer_fillrect(framebuffer,0,0,320,240,0,0,0);
	#endif
	setShellFramebuffer(framebuffer);
	debug_shell_println("finished relocating");
	debug_shell_println("initializing");
	
	
	
	debug_shell_println("malloc pointer: 0x%x",getKernelMallocedPointer());
	
	debug_shell_println("kernel_start: 0x%x",&_EXEC_START);
	
	
	#ifndef RELEASE
		debug_shell_println("performing physical memory manager self-test");
		bool b = physical_mm_self_test();
		if (! b)
		{
			debug_shell_println_rgb("error in physical memory manager self-test         aborting",255,0,0);
			keypad_press_release_barrier();
			free_init_pds();
			enableIRQ();
			return false;
		}
	#endif
	
	debug_shell_println("allocating memory");
	allocPageblock(128);
	allocPageblock(128);
	allocPageblock(128);
	allocPageblock(128); // allocate 2mb
	debug_shell_println("done");
	
	
	#ifndef RELEASE
		debug_shell_println("performing virtual memory manager self-test");
		b = virtual_mm_self_test();
		if (! b)
		{
			debug_shell_println_rgb("error in virtual memory manager self-test         aborting",255,0,0);
			keypad_press_release_barrier();
			free_init_pds();
			enableIRQ();
			return false;
		}
		debug_shell_println("performing slab allocator self-test");
		b = slab_allocator_self_test_pre_initialization();
		if (! b)
		{
			debug_shell_println_rgb("error in slab allocator self-test         aborting",255,0,0);
			keypad_press_release_barrier();
			free_init_pds();
			enableIRQ();
			return false;
		}
	#endif
	
	debug_shell_println("initializing kernel space");
	
	
	initializeKernelSpace();
	
	init_drivers_remapped_io();
	init_drivers();
	
	free_init_pds();
	
	
	debug_shell_println("done");
	
	
	if (! install_exception_handlers())
	{
		debug_shell_println_rgb("no page for exception vectors         aborting",255,0,0);
		keypad_press_release_barrier();
		enableIRQ();
		return false;
	}
	
	
	
	
	debug_shell_println("switching framebuffer, press any key");
	#ifndef RELEASE
		keypad_press_release_barrier();
	#endif
	
	
	
	
	
	
	
	claimLCD();
	#ifdef RELEASE
		k_memcpy(get_front_framebuffer_address(),framebuffer,320*240*2);
	#endif
	#ifndef RELEASE
		debug_shell_reset();
		setShellFramebuffer(get_front_framebuffer_address());
		debug_shell_println("new framebuffer: 0x%x",*LCD_UPBASE);
	#endif
	
	
	
		
	
	// construct the pages for the remapped stack
	void* page = usePage();
	if (page == NULL)
	{
		debug_shell_println_rgb("no page for general self test         aborting",255,0,0);
		keypad_press_release_barrier();
		enableIRQ();
		return false;
	}
	addVirtualKernelPage(page,(void*) 0xe8000000);
	page = usePage();
	if (page == NULL)
	{
		debug_shell_println_rgb("no page for general self test         aborting",255,0,0);
		keypad_press_release_barrier();
		enableIRQ();
		return false;
	}
	addVirtualKernelPage(page,(void*) (0xe8000000+SMALL_PAGE_SIZE));
	
	
	debug_shell_println("installing hooks");
	install_hooks();
	
	
	debug_shell_println("initializing module system");
	module_system_init();
	module_load_all();
	
	return true;
}


























