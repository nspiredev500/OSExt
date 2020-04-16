#include "kernel.h"



struct thread* createThread(uint16_t tid,uint32_t pc)
{
	struct thread *t = requestThread();
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



void destroyThread(struct thread *t)
{
	freeThread(t);
}



uint32_t runThread(struct thread *t,struct thread_return_desc *ret)
{
	// assumes the context switch (virtual address space) is already done
	register uint32_t *regs asm("r0") = t->regs;
	register uint32_t *svc_sp asm("r2") = &(ret->sp);
	
	
	
	asm volatile(
	" .global thread_return_point \n"
	" push {r1-r12,lr} \n"
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
	" str sp, [r2] \n" // store the sp to be able to return from a thread while being in a syscall
	" add r0, r0, #4 \n"
	" ldmia r0, {r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12} \n" // load the thread's registers
	" sub r0, r0, #4 \n"
	" ldr lr, [r0, #60] \n" // load the user mode pc
	" ldr r0, [r0] \n" // load the user mode r0
	" push {r0-r12,r14} \n" // push the user mode registers
	" ldmia sp!, {r0-r12,pc}^ \n" // pop the user mode registers, copy spsr into cpsr
	" thread_return_point: pop {r1} \n"
	" msr cpsr, r1 \n" // restore the cpsr, in case it was modified
	" pop {r1-r12,lr} \n" // pop the rest of the registers
	"  \n":"+r" (regs):"r" (regs), "r" (svc_sp):"memory");
	
	
	
	/*
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
	*/
	
	
	return (uint32_t) regs;
}




void __attribute__ ((noreturn)) returnThread(struct thread_return_desc *ret,uint32_t return_value)
{
	// assumes the registers of the thread are already saved
	extern void* thread_return_point;
	register uint32_t ret_val asm("r0") = return_value;
	register void* ret_pc asm("r1") = thread_return_point;
	register uint32_t *svc_sp asm("r2") = &(ret->sp);
	asm volatile(
	" mov sp, r2 \n"
	" mov pc, r1 \n"
	"  \n"::"r" (svc_sp),"r" (ret_pc),"r" (ret_val):);
	__builtin_unreachable();
}









































