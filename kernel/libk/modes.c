#include "../kernel.h"



void irq_save_state(struct irq_state *s)
{
	s->irq = isIRQ();
	s->fiq = isFIQ();
}

void irq_disable()
{
	disableFIQ();
	disableIRQ();
}


void irq_restore_state(struct irq_state *s)
{
	if (s->irq)
	{
		enableIRQ();
	}
	if (s->fiq)
	{
		enableFIQ();
	}
}



bool isIRQ()
{
	register uint32_t cpsr asm("r0") = 0;
	asm("mrs r0, cpsr":"=r" (cpsr)::);
	if ((cpsr >> 7 & 0b1) == 1)
	{
		return false;
	}
	return true;
}
bool isFIQ()
{
	register uint32_t cpsr asm("r0") = 0;
	asm("mrs r0, cpsr":"=r" (cpsr)::);
	if ((cpsr >> 6 & 0b1) == 1)
	{
		return false;
	}
	return true;
}
void enableIRQ()
{
	asm("mrs r0, cpsr \n"
	"bic r0, r0, #0x80 \n"
	"msr cpsr, r0":::"r0");
}
void disableIRQ()
{
	asm("mrs r0, cpsr \n"
	"orr r0, r0, #0x80 \n"
	"msr cpsr, r0":::"r0");
}

void enableFIQ()
{
	asm("mrs r0, cpsr \n"
	"bic r0, r0, #0x40 \n"
	"msr cpsr, r0":::"r0");
}
void disableFIQ()
{
	asm("mrs r0, cpsr \n"
	"orr r0, r0, #0x40 \n"
	"msr cpsr, r0":::"r0");
}




// WARNING: NOT REENTRANT!
// function does not support arguments
uint32_t call_with_stack(const void* stack,void* function)
{
	register void* sp_var asm("r0") = (void*) stack;
	register void* func_var asm("r1") = function;
	asm volatile(
	" b continue \n"
	" saved_sp: .long 0 \n"
	" continue: str sp, saved_sp \n"
	" mov sp, r0 \n"
	" blx r1 \n"
	" ldr sp, saved_sp":"=r" (sp_var),"=r" (func_var):"r" (sp_var),"r" (func_var):"r2","r3","r4","r5","r6","r7","r8","r9","r10","r11","r12","r14","lr","memory");
	return (uint32_t) sp_var;
}






















