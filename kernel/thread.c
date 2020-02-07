#include "kernel.h"



struct Thread* createThread(uint16_t tid,uint32_t pc)
{
	struct Thread *t = requestThread();
	for (int i = 0;i<16;i++)
	{
		t->regs[i] = 0;
	}
	t->regs[15] = pc;
	t->regs[16] = 0b00000000000000000000000010000000; // set the cpsr to irq disabled
	t->tid = tid;
	t->status = 0;
	return t;
}



void destroyThread(struct Thread *t)
{
	freeThread(t);
}



void runThread(struct Thread *t)
{
	// assumes the context switch (virtual address space) is already done
	register uint32_t *regs asm("r0") = t->regs;
	asm volatile(
	" ldr r1, [r0, #64]\n" // load the cpsr
	" msr spsr, r1\n" // put it into the spsr
	" mrs r1, cpsr\n"
	" orr r1, r1, #31\n"
	" mrs r1, cpsr\n" // go into system mode to manipulate user mode sp and lr
	" ldr sp, [r0, #52]\n" // set the sp
	" ldr lr, [r0, #56]\n" // set the lr
	" and r1,r1, #19\n"
	" mrs r1, cpsr\n" // switch back to svc mode
	" add r0, r0, #4\n"
	" ldmia r0, {r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12}\n" // load the thread's registers
	" sub r0, r0, #4\n"
	" mov lr, pc\n" // store the return address in the svc lr
	" add r12, r12, #12\n"
	" ldr sp, [r0, #60]\n" // load the pc temporarily into the sp
	" ldr r0, [r0]\n" // load r0
	" movs pc, sp\n" // jump to the thread, replace cpsr with spsr
	" nop\n"// return address
	" nop\n":"+r" (regs):"r" (regs):"memory","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10","r11","r12","r13","r14","cc");
	
	
	
	
	
	
}















































