#include "../kernel.h"



asm(
".global relay_data_abort_to_ndless \n"
"relay_data_abort_to_ndless: .long 0\n"
".global data_wrapper \n"
//BREAKPOINT_ASM
"data_wrapper: \n"
"push {r0-r12,r14} \n"
"	mrs r0, cpsr \n"
"	push {r0,r1} \n" // save the cpsr, and also r1 to make the stack 8 byte aligned
"	orr r0, #192 \n"
"	msr cpsr, r0 \n" // disable fiq and irq
"	mov r0, lr \n"
"	mrs r1, spsr \n"
"	mov r2, sp \n"
"	bl data_abort_handler \n"
"   ldr r0, relay_data_abort_to_ndless \n"
"   cmp r0, #1 \n"
"   beq _relay_ndless \n"
"	pop {r0,r1} \n"
"	msr cpsr, r0 \n" // restore the cpsr
"pop {r0-r12,r14} \n"
"subs pc, lr, #8 \n"
"_relay_ndless: \n"
"mov r0, #0 \n"
"str r0, relay_data_abort_to_ndless \n"
"pop {r0,r1} \n"
"msr cpsr, r0 \n" // restore the cpsr
"pop {r0-r12,r14} \n"
"mov pc, #0x10 \n");


extern uint32_t relay_data_abort_to_ndless;

/*
bool lcd_undef_breakpoint = false;
uint32_t lcd_undef_inst = 0;
uint32_t* lcd_undef_adr = NULL;
uint32_t lcd_undef_section = 0;
*/

uint32_t data_abort_handler(uint32_t* address,uint32_t spsr,uint32_t *regs) // regs[0] is the old abort cpsr, regs[1] is a dummy value, the rest are the registers
{
	uint32_t thumb = (spsr >> 5) & 0b1;
	register uint32_t abort_address asm("r1") = 0;
	asm volatile("mrc p15, 0, r1, c6, c0, 0":"=r" (abort_address)::);
	
	if (((spsr & 0b11111) == 0b10011 || (spsr & 0b11111) == 0b11111) && abort_address >> 28 == 0xC) // privileged mode and tried to access LCD controller
	{
		/// TODO: do not relay in standalone mode
		relay_data_abort_to_ndless = 1;
		return 0;
	}
	if (((spsr & 0b11111) == 0b10011 || (spsr & 0b11111) == 0b11111) && ! probe_address)
	{
		
		
		if (thumb == 1)
		{
			panic("data abort in thumb privileged mode!");
		}
		// not currently trying to probe an address from an other handler, and an exception occurred
		// we have to assume the worst about the integrity of the code and data
		// unlock the recovery kernel and jump to it
		
		/// TODO the recovery kernel
		
		panic("Data abort in privileged mode!");
		
	}
	if ((((spsr & 0b11111) == 0b10011 || (spsr & 0b11111) == 0b11111) && probe_address))
	{
		return 1;
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
			" bic r1, r1, #8 \n"
			" msr cpsr, r1 \n" // switch back to abort mode
			"  \n"::"r" (t_regs):"r1");
		}
		else
		{
			panic("abort from user mode, but no thread is running!\n");
		}
		scheduler_return(SCHEDULER_DATA_ABORT);
	}
	
	
	
	
	
	return 0;
}








































