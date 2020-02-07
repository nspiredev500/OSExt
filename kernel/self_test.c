#include "kernel.h"



asm(
" usermode_test: add r0, r0, #2\n"
" ldr r1, [r0]"); // breakpoint





bool run_self_test()
{
	struct address_space *space = createAddressSpace();
	DEBUGPRINTLN_1("userspace tt: 0x%x",space->tt)
	void *page = usePage();
	if (page == NULL)
	{
		debug_shell_println("no page for userspace test!");
		return false;
	}
	
	uint32_t sp_old = 0;
	
	
	// TODO change the stack into the remapped zone, to avoid data aborts on stack actions
	
	
	DEBUGPRINTLN_1("mapping the page in the usermode translation table")
	addVirtualPage(space,page,(void*) (SECTION_SIZE*10));
	
	
	
	// copy the usermode test into userspace
	extern void* usermode_test;
	k_memcpy(page,usermode_test,4*2);
	
	
	struct Thread *t = createThread(0,SECTION_SIZE*10);
	
	
	DEBUGPRINTLN_1("changing into user space")
	changeAddressSpace(space);
	debug_shell_println("running userspace test");
	runThread(t);
	
	
	debug_shell_println("finished userspace test");
	
	
	destroyThread(t);
	
	intoKernelSpace();
	destroyAddressSpace(space);
	
	
	
	
	return true;
}








