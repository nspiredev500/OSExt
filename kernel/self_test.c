#ifndef RELEASE
#include "kernel.h"


/*
asm(
" usermode_test: add r0, r0, #2\n"
" ldr r1, [r0]");
*/

asm(
" usermode_test: \n"
"add r0, r0, #2 \n"
"ldr r1, [r0] \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n");


bool run_self_test()
{
	// TODO write a new userspace self-test with the scheduler
	
	
	
	
	
	
	
	
	
	
	
	/*
	struct address_space *space = createAddressSpace();
	DEBUGPRINTLN_1("userspace tt: 0x%x",space->tt)
	void *page = usePage();
	DEBUGPRINTLN_1("page used: 0x%x",page)
	if (page == NULL)
	{
		debug_shell_println("no page for userspace test!");
		return false;
	}
	
	DEBUGPRINTLN_1("mapping the page in the usermode translation table")
	addVirtualPage(space,page,(void*) (SECTION_SIZE*5));
	
	
	// copy the usermode test into userspace
	extern void* usermode_test;
	DEBUGPRINTLN_1("user mode test: 0x%x",&usermode_test)
	
	
	k_memcpy(page,&usermode_test,4*2);
	
	
	
	
	struct Thread *t = createThread(0,SECTION_SIZE*5);
	
	
	DEBUGPRINTLN_1("changing into user space")
	changeAddressSpace(space);
	
	
	
	debug_shell_println("running userspace test");
	runThread(t);
	
	
	debug_shell_println("finished userspace test");
	
	
	
	
	intoKernelSpace();
	
	destroyThread(t);
	
	
	destroyAddressSpace(space);
	
	DEBUGPRINTLN_1("finished")
	
	
	*/
	return true;
}








#endif