#include "../kernel.h"




asm(
".global prefetch_wrapper \n"
"prefetch_wrapper: \n"
"push {r0-r12,r14} \n"
"	mrs r0, cpsr \n"
"	push {r0,r1} \n" // save the cpsr, and also r1 to make the stack 8 byte aligned
"	orr r0, #192 \n"
"	msr cpsr, r0 \n" // disable fiq and irq
"	mov r0, lr \n"
"	mrs r1, spsr \n"
"	mov r2, sp \n"
"	bl prefetch_abort_handler \n"
"	cmp r0, #1 \n"
"	pop {r0} \n"
"	msr cpsr, r0 \n" // restore the cpsr
"pop {r0-r12} \n"
"beq skip_instruction_prefetch \n" // should be set if it's an breakpoint that didn't replace an instruction
"subs pc, lr, #4 \n"
"skip_instruction_prefetch: movs pc, lr \n");




uint32_t prefetch_abort_handler(uint32_t* address,uint32_t spsr,uint32_t *regs) // regs[0] is the old abort cpsr, regs[1] is a dummy value, the rest are the registers
{
	uint32_t thumb = (spsr >> 5) & 0b1;
	probe_address = true;
	uint32_t instruction = *address;
	probe_address = false;
	if (((spsr & 0b11111) == 0b10011 || (spsr & 0b11111) == 0b11111))
	{
		if (thumb == 1)
		{
			panic("prefetch abort in thumb privileged mode!");
		}
		if (instruction == 0xE1212374) // this is a breakpoint, so it's probably alright
		{
			
			return 1;
		}
		else
		{
			// not currently trying to probe an address from an other handler, and an exception occurred
			// we have to assume the worst about the integrity of the code and data
			// unlock the recovery kernel and jump to it
			
			// TODO the recovery kernel
			
			panic("Prefetch abort in privileged mode!");
		}
	}
	if ((spsr & 0b11111) == 0b10000) // user mode
	{
		if (running_thread != NULL)
		{
			// updating the thread's registers
			running_thread->regs[16] = spsr;
			running_thread->regs[15] = (uint32_t) address;
			for (uint8_t i = 0;i<=12;i++)
			{
				running_thread->regs[i] = regs[i+2];
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
		register uint32_t *prefetch_stack_start asm("r0") = abort_stack+sizeof(abort_stack)/4-4;
		asm volatile(
		" mov sp, r0 \n" // reset the abort stack to the start
		" mov r0, #2 \n" // 1 indicates a prefetch abort
		" bic r1, r1, #31 \n" // clear the mode bits
		" orr r1, r1, #19 \n" // set the mode to svc
		" msr cpsr, r1 \n"
		" bx lr \n" // setting the right other cpsr bits is done in thread.c
		:"+r" (prefetch_stack_start):"r" (prefetch_stack_start):"r1");
		__builtin_unreachable();
	}
	
	
	
	
	return 0;
}




































