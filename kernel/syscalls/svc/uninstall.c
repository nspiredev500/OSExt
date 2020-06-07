#include "../../kernel.h"



void uninstall_osext(uint32_t* regs)
{
	
	
	disableFIQ();
	
	bool irq = isIRQ();
	
	if (irq)
	{
		disableIRQ();
	}
	void* fb_unaligned = ti_malloc(320*240*2+256);
	uint16_t *framebuffer = (uint16_t*) (((uint32_t) fb_unaligned & (~ 0b111))+0b1000);
	
	k_memset(framebuffer,0,320*240*2);
	
	void* oldupbase = (void*) *LCD_UPBASE;
	*LCD_UPBASE = framebuffer;
	clear_caches();
	
	debug_shell_reset();
	setShellFramebuffer(framebuffer);
	
	shell_println("uninstalling osext");
	
	
	shell_println("resetting exception vectors");
	set_exception_vectors(false);
	
	
	shell_println("uninstalling hooks");
	uninstall_hooks(); // TODO uninstaller only works if hooks are immediately reused by another instance of osext, not if is just uninstalled
	
	
	
	// TODO give processes time to exit
	
	shell_println("freeing memory");
	freeAllPageblocks();
	
	
	shell_println("kernel base pointer: 0x%x",getKernelMallocedPointer());
	
	
	
	shell_println_rgb("osext uninstalled",0,255,0);
	
	shell_println_rgb("press any key to continue",0,255,0);
	keypad_press_release_barrier();
	
	*LCD_UPBASE = get_old_framebuffer_address();
	
	
	DEBUGPRINTLN_1("freeing framebuffer")
	ti_free(fb_unaligned);
	
	
	
	if (irq)
	{
		enableIRQ();
	}
	
	
	
	
	
	
	DEBUGPRINTLN_1("returning")
	
	syscall_set_reg(regs,0,(uint32_t) getKernelMallocedPointer());
}







