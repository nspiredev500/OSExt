#ifdef MODULE_DYNLINKER
#ifndef DYNLINKER_H
#define DYNLINKER_H
#include <nucleus.h>

//from ndless ints.c
//#define INTS_SWI_HANDLER_ADDR 0x28
static const unsigned int INTS_SWI_HANDLER_ADDR = 0x28;
/*
*(void**)(OS_BASE_ADDRESS + INTS_SWI_HANDLER_ADDR) = &ints_swi_handler;
ints_next_descriptor_ptr = &ut_next_descriptor;
*/





void initializeDynlinker()
{
	unsigned int *swihandler = (unsigned int*) (OS_BASE_ADDRESS + INTS_SWI_HANDLER_ADDR);
	uart_printf("swi handler address: %d",*swihandler);
	
	
}


void dynlinkerSelftest()
{
	
}
















#endif
#endif