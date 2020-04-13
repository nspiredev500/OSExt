#include "kernel.h"



struct Thread* createThread(uint16_t tid,uint32_t pc)
{
	struct Thread *t = requestThread();
	for (int i = 0;i<16;i++)
	{
		t->regs[i] = 0;
	}
	t->regs[15] = pc;
	t->regs[16] = 0b00000000000000000000000010010000; // set the cpsr to irq disabled, and set mode to user mode
	t->tid = tid;
	t->status = 0;
	return t;
}



void destroyThread(struct Thread *t)
{
	freeThread(t);
}
struct Thread *running_thread = NULL;


uint32_t runThread(struct Thread *t)
{
	// assumes the context switch (virtual address space) is already done
	running_thread = t;
	register uint32_t *regs asm("r0") = t->regs;
	
	
	
	
	
	asm volatile(
	" push {r1-r12,lr} \n" // push the svc registers (except r0, it is used as a status code, and sp)
	" ldr r1, [r0, #64] \n" // load the cpsr
	" msr spsr, r1 \n" // put it into the spsr
	" mrs r1, cpsr \n"
	" orr r1, r1, #31 \n"
	" msr cpsr, r1 \n" // go into system mode to manipulate user mode sp and lr
	" ldr sp, [r0, #52] \n" // set the sp
	" ldr lr, [r0, #56] \n" // set the lr
	" bic r1, r1, #12 \n"
	" msr cpsr, r1 \n" // switch back to svc mode
	" push {r1} \n" // push the cpsr on the stack
	" add r0, r0, #4 \n"
	" ldmia r0, {r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12} \n" // load the thread's registers
	" sub r0, r0, #4 \n"
	" ldr lr, [r0, #60] \n" // load the user mode pc
	" ldr r0, [r0] \n" // load the user mode r0
	" push {r0-r12,r14} \n" // push the user mode registers
	" mov lr, pc \n"
	" ldmia sp!, {r0-r12,pc}^ \n" // pop the user mode registers, copy spsr into cpsr
	" pop {r1} \n" // return address, pop the cpsr to r1
	" msr cpsr, r1 \n" // restore the cpsr
	" pop {r1-r12,lr} \n" //pop the svc registers
	" \n":"+r" (regs):"r" (regs):"memory","r1");
	
	
	
	running_thread = NULL;
	return (uint32_t) regs;
}















































