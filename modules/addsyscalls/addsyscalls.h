#ifndef ADDSYSCALLS_H
#define ADDSYSCALLS_H
#include <nucleus.h>
//#include <syscall.h>


//from ndless ints.c
//#define INTS_SWI_HANDLER_ADDR 0x28
static const unsigned int INTS_SWI_HANDLER_ADDR = 0x28;
/*
*(void**)(OS_BASE_ADDRESS + INTS_SWI_HANDLER_ADDR) = &ints_swi_handler;
ints_next_descriptor_ptr = &ut_next_descriptor;
*/
//static const int have_addrs_offset = 0x1295bfe0-0x1295bf74; == 6c
static const int have_addrs_offset = 0x6c;
//static const unsigned int hookoffset = 0x1293f110-0x1293f0d8; == 0x38


static const unsigned int hookoffset = 0x4c;



static const unsigned int jmptosyscalloffset= 0x80;

static const unsigned int mysyscall_mask = 0x7c000;




int testsyscall()
{
	bkpt();
	return 1;
}



static unsigned int syscall_table_size = 1;
unsigned int my_syscall_table[10] = {(unsigned int) testsyscall};



extern unsigned int haveaddress;


bool syscallsready = false;





//
HOOK_DEFINE(swi_hook)
{
	unsigned int r0 = HOOK_SAVED_REGS(swi_hook)[0]; // syscall number for comparison
	//uart_printf("syscall number: %x",r1); // doesn't work in syscalls
	//bkpt();
	syscallsready = true;
	//bkpt();
	if ((r0 & mysyscall_mask) != 0)
	{
		//bkpt();
		unsigned int number = r0 & (~ mysyscall_mask);
		if (number < syscall_table_size)
		{
			bkpt();
			HOOK_SAVED_REGS(swi_hook)[0] =  my_syscall_table[number];
			HOOK_RESTORE(swi_hook);
			asm volatile(
				"ldr pc, haveaddress\n"
				"haveaddress: .word\n");
		}
	}
	//bkpt();
	HOOK_RESTORE_RETURN(swi_hook);
}






void extendSWIHandler()
{
	unsigned int *swihandler = (unsigned int*) (OS_BASE_ADDRESS + INTS_SWI_HANDLER_ADDR);
	haveaddress = *swihandler+jmptosyscalloffset;
	uart_printf("swihandler: %x",*swihandler);
	HOOK_INSTALL(*swihandler+hookoffset,swi_hook);
	clear_cache();
	
	
	
	
	
}















/*

asm("jumptohandler: ldr pc, [pc-#4]\n");



asm("myswihandler: \n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n");



void extendSWIHandler()
{
	unsigned int *swihandler = (unsigned int*) (OS_BASE_ADDRESS + INTS_SWI_HANDLER_ADDR);
	unsigned int *placehandler =  (unsigned int*) (*swihandler+hookoffset)
	haveaddress = *swihandler+have_addrs_offset;
	uart_printf("swihandler: %x",*swihandler);
	*(placehander) = jumptohandler;
	*(placehander+1) = handlerAddress;
	
	
	
	//HOOK_INSTALL(*swihandler+hookoffset,swi_hook);
	clear_cache();
	
}
*/




















#endif