#include "kernel.h"



asm(
".global data_wrapper \n"
"data_wrapper: \n"
"push {r0-r12} \n"
"	mrs r0, cpsr \n"
"	push {r0} \n" // save the cpsr
"	orr r0, #192 \n"
"	msr cpsr, r0 \n" // disable fiq and irq
"	mov r0, lr \n"
"	mrs r1, spsr \n"
"	mov r2, sp \n"
"	bl data_abort_handler \n"
"	cmp r0, #1 \n"
"	pop {r0} \n"
"	msr cpsr, r0 \n" // restore the cpsr
"pop {r0-r12} \n"
"beq skip_instruction_data \n" // should be set if probing an address
"subs pc, lr, #8 \n"
"skip_instruction_data: movs pc, lr \n");







uint32_t data_abort_handler(uint32_t* address,uint32_t spsr,uint32_t *regs) // regs[0] is the old abort cpsr, the rest are the registers
{
	uint32_t thumb = (spsr >> 5) & 0b1;
	if (((spsr & 0b11111) == 0b10011 || (spsr & 0b11111) == 0b11111) && ! probe_address)
	{
		if (thumb == 1)
		{
			panic("data abort in thumb privileged mode!");
		}
		// not currently trying to probe an address from an other handler, and an exception occurred
		// we have to assume the worst about the integrity of the code and data
		// unlock the recovery kernel and jump to it
		
		// TODO the recovery kernel
		
		panic("Data abort in privileged mode!");
		
	}
	if ((((spsr & 0b11111) == 0b10011 || (spsr & 0b11111) == 0b11111) && probe_address))
	{
		return 1;
	}
	if ((spsr & 0b11111) == 0b10000) // user mode
	{
		if (running_thread != NULL)
		{
			// updating the thread's registers
			running_thread->regs[16] = spsr;
			running_thread->regs[15] = (uint32_t) address;
			for (uint8_t i = 0;i<12;i++)
			{
				running_thread->regs[i] = regs[i+1];
			}
			register uint32_t *t_regs asm("r0") = running_thread->regs;
			asm volatile(
			" mrs r1, cpsr\n"
			" orr r1, r1, #31\n"
			" msr cpsr, r1 \n" // go into system mode to manipulate user mode sp and lr
			" str sp, [r0, #52]\n" // store the sp
			" str lr, [r0, #56]\n" // store the lr
			" bic r1, r1, #8 \n"
			" msr cpsr, r1 \n" // switch back to abort mode
			"  \n"::"r" (t_regs):"r1");
		}
		else
		{
			panic("abort from user mode, but no thread is running!\n");
		}
		// jump back into kernel mode, to svc_lr and use r0 to indicate an abort
		register uint32_t *data_stack_start asm("r0") = abort_stack+sizeof(abort_stack)/4-4;
		asm volatile(
		" mov sp, r0 \n" // reset the abort stack to the start
		" mov r0, #1 \n" // 1 indicates a data abort
		" bic r1, r1, #31 \n" // clear the mode bits
		" orr r1, r1, #19 \n" // set the mode to svc
		" msr cpsr, r1 \n"
		" bx lr \n" // setting the right other cpsr bits is done in thread.c
		:"+r" (data_stack_start):"r" (data_stack_start):"r1");
		__builtin_unreachable();
	}
	
	
	
	
	
	return 0;
}








































