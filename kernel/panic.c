#include "kernel.h"




static void __attribute__ ((noreturn)) reset(void) {
	*(volatile unsigned*)0x900A0008 = 2;
	__builtin_unreachable();
}


void __attribute__ ((noreturn)) panic(char *msg)
{
	uart_send_string(msg);
	reset();
}



uint32_t _starting_regs[16];

// if returnable is true, the kernel can crash without a kernel panic
static bool returnable = true;

void setNotReturnable()
{
	returnable = false;
}



void abort(void)
{
	panic("aborting!\n");
	
	
	
	
	
	
	
}














