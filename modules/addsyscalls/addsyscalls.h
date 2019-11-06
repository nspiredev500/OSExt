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


static const unsigned int hookoffset = 0x4c;

HOOK_DEFINE(swi_hook)
{
	bkpt();
	
	
}


void extendSWIHandler()
{
	unsigned int *swihandler = (unsigned int*) (OS_BASE_ADDRESS + INTS_SWI_HANDLER_ADDR);
	
	HOOK_INSTALL(*swihandler+hookoffset,swi_hook);
	
	
	
	
	
	
}








#endif