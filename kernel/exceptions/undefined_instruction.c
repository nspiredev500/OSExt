#include "../kernel.h"




asm(
".global undef_wrapper \n"
"undef_wrapper: \n"
"push {r0-r12,r14} \n"
"	mrs r0, cpsr \n"
"	push {r0,r1} \n" // save the cpsr, and also r1 to make the stack 8 byte aligned
"	orr r0, #192 \n"
"	msr cpsr, r0 \n" // disable fiq and irq
"	mov r0, lr \n"
"	mrs r1, spsr \n"
"	mov r2, sp \n"
"	bl undefined_instruction_handler \n"
"   cmp r0, #1 \n"
"	pop {r0,r1} \n"
"	msr cpsr, r0 \n" // restore the cpsr
"pop {r0-r12,r14} \n"
"bne undef_noskip \n"
"movs pc, lr \n"
"undef_noskip: subs pc, lr, #4");







uint32_t undefined_instruction_handler(uint32_t* address,uint32_t spsr,uint32_t *regs) // regs[0] is the old abort cpsr, regs[1] is a dummy value, the rest are the registers
{
	uint32_t thumb = (spsr >> 5) & 0b1;
	if ((spsr & 0b11111) == 0b10011 || (spsr & 0b11111) == 0b11111)
	{
		/*
		if (lcd_undef_breakpoint)
		{
			
			lcd_undef_breakpoint = false;
			
			*lcd_undef_adr = lcd_undef_inst;
			
			register uint32_t tt_base asm("r0");
			asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
			
			tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
			uint32_t *tt = (uint32_t*) tt_base;
			
			tt[0xC0000000 >> 20] = lcd_undef_section;
			clear_caches();
			invalidate_TLB();
			
			return 0;
		}
		*/
		if (thumb == 1)
		{
			panic("undefined instruction in thumb privileged mode!");
		}
		// not currently trying to probe an address from an other handler, and an exception occurred
		// we have to assume the worst about the integrity of the code and data
		// unlock the recovery kernel and jump to it
		
		// TODO the recovery kernel
		
		panic("Undefined instruction in privileged mode!");
		
	}
	if ((spsr & 0b11111) == 0b10000) // user mode
	{
		struct thread* running_thread = scheduler_running();
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
			" bic r1, r1, #4 \n"
			" msr cpsr, r1 \n" // switch back to abort mode
			"  \n"::"r" (t_regs):"r1");
		}
		else
		{
			panic("abort from user mode, but no thread is running!\n");
		}
		scheduler_return();
	}
	
	
	return 1;
}

































