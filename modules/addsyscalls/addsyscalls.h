#ifndef ADDSYSCALLS_H
#define ADDSYSCALLS_H
#include <nucleus.h>

//from ndless ints.c
//#define INTS_SWI_HANDLER_ADDR 0x28
static const unsigned int INTS_SWI_HANDLER_ADDR = 0x28;
/*
*(void**)(OS_BASE_ADDRESS + INTS_SWI_HANDLER_ADDR) = &ints_swi_handler;
ints_next_descriptor_ptr = &ut_next_descriptor;
*/


static const unsigned int hookoffset = 0x1293f114-0x1293f0d8;

HOOK_DEFINE(swi_hook)
{
	unsigned int r1 = HOOK_SAVED_REGS(swi_hook)[1]; // syscall number for comparison
	//uart_printf("syscall number: %x",r1); // doesn't work in syscalls
	
	//right place for custom task switcher
	
	
	
	
	
	bkpt();
	HOOK_RESTORE_RETURN(swi_hook);
}


void extendSWIHandler()
{
	unsigned int *swihandler = (unsigned int*) (OS_BASE_ADDRESS + INTS_SWI_HANDLER_ADDR);
	
	HOOK_INSTALL(*swihandler+hookoffset,swi_hook);
	
	
	
	
	
	
}








#endif