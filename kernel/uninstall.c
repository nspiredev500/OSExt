#include "kernel.h"



uint32_t uninstall_osext(uint32_t* regs)
{
	
	
	disableFIQ();
	
	
	void* fb_unaligned = ti_malloc(320*240*2+256);
	uint16_t *framebuffer = (uint16_t*) (((uint32_t) fb_unaligned & (~ 0b111))+0b1000);
	
	k_memset(framebuffer,0,320*240*2);
	
	void* oldupbase = (void*) *LCD_UPBASE;
	*LCD_UPBASE = framebuffer;
	clear_caches();
	
	debug_shell_reset();
	setShellFramebuffer(framebuffer);
	
	debug_shell_println("uninstalling osext");
	
	
	debug_shell_println("resetting exception vectors");
	set_exception_vectors(false);
	
	
	
	
	
	// TODO give processes time to exit
	
	debug_shell_println("freeing memory");
	freeAllPageblocks();
	
	
	
	
	
	
	debug_shell_println_rgb("osext uninstalled",0,255,0);
	
	debug_shell_println_rgb("press any key to continue",0,255,0);
	keypad_press_release_barrier();
	
	*LCD_UPBASE = oldupbase;
	
	ti_free(fb_unaligned);
	
	
	
	
	
	
	
	
	
	
	
	
	return (uint32_t) getKernelMallocedPointer();
}







