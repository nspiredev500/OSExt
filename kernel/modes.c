#include "kernel.h"




static void* exec_start = NULL;
void init_call_with_stack(void* start)
{
	exec_start = start;
}




// WARNING: NOT REENTRANT!
// function does not support arguments
uint32_t call_with_stack(void* stack,void* function)
{
	if (exec_start == NULL)
	{
		panic("call_with_stack not initialized!\n");
	}
	register void* sp_var asm("r0") = stack;
	register void* func_var asm("r1") = (function-exec_start)+virtual_base_address;
	//asm(".long 0xE1212374"); // bkpt
	asm volatile(
	" b continue \n"
	" saved_sp: .long 0 \n"
	" continue: str sp, saved_sp \n"
	" mov sp, r0 \n"
	" blx r1 \n"
	" ldr sp, saved_sp":"=r" (sp_var),"=r" (func_var):"r" (sp_var),"r" (func_var):"r3","r4","r5","r6","r7","r8","r9","r10","r11","r12","r13","r14","sp","lr","memory");
	return (uint32_t) sp_var;
}


















