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













