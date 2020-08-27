#include "../kernel.h"






struct svc_thread* create_svc_thread(void* stack, uint32_t stacksize, void* entry)
{
	struct svc_thread* t = request_svc_thread();
	for (uint32_t i = 0;i<18;i++)
	{
		t->regs[i] = 0;
	}
	t->regs[13] = ((((uint32_t) stack) & (~ 0b111))-0b1000) + stacksize; // align to 8 bytes by clearing the bottom 3 bits, and then also subtract 8
	t->regs[14] = (uint32_t) scheduler_svc_thread_destroy_self; // so it won't just return to 0 after it's finished
	t->regs[16] = 0b10010011; // set mode to svc, irq disabled and fiq enabled
	t->regs[15] = (uint32_t) entry; // set the pc to the entry point
	
	t->main = false;
	t->stack = stack;
	t->stacksize = stacksize;
	t->status = 0;
	t->status_data = NULL;
	t->waiting = 0;
	t->next = NULL;
	return t;
}

void destroy_svc_thread(struct svc_thread* thread)
{
	free_svc_thread(thread);
}

/// WARNING: should only be called from the fiq handler, no way to return
/// call with fiqs disabled
void __attribute__ ((noreturn)) resume_svc_thread(struct svc_thread* thread)
{
	//DEBUGPRINTLN_1("resuming thread with pc: 0x%x\n",thread->regs[15]);
	register volatile uint32_t* regs asm("r0") = thread->regs;
	asm volatile(
	"mrs r1, cpsr \n"
	"bic r1, #0b11111 \n" // clear the mode bits
	"orr r1, r1, #0b10011 \n" // set the mode to svc
	"msr cpsr, r1 \n"
	"ldr r1, [r0, #68] \n" // load the spsr
	"msr spsr, r1 \n" // set the svc spsr
	"ldr r8, [r0, #32] \n" // load r8-r14
	"ldr r9, [r0, #36] \n"
	"ldr r10, [r0, #40] \n"
	"ldr r11, [r0, #44] \n"
	"ldr r12, [r0, #48] \n"
	"ldr r13, [r0, #52] \n"
	"ldr r14, [r0, #56] \n"
	"mrs r1, cpsr \n"
	"bic r1, #0b11111 \n" // clear the mode bits
	"orr r1, r1, #0b10001 \n" // set the mode to fiq (should be already, but for example scheduler_svc_thread_destroy_self is in svc mode)
	"msr cpsr, r1 \n" // the fiq mode is just needed to have some registers hidden from svc mode
	"mov r8, r0 \n" // move the base address into one of the protected fiq registers
	"ldr r0, [r8] \n" // load r0-r7
	"ldr r1, [r8, #4] \n"
	"ldr r2, [r8, #8] \n"
	"ldr r3, [r8, #12] \n"
	"ldr r4, [r8, #16] \n"
	"ldr r5, [r8, #20] \n"
	"ldr r6, [r8, #24] \n"
	"ldr r7, [r8, #28] \n"
	"ldr r9, [r8, #64] \n" // load the cpsr
	"msr spsr, r9 \n" // put the svc cpsr into the spsr, so movs pc can set it at return
	"ldr r9, [r8, #60] \n" // load the pc
	"movs pc, r9 \n" // jump to the thread with it's cpsr
	" \n"::"r" (regs):"memory");
	__builtin_unreachable();
	/*
	asm(
	" .global svc_thread_return_point \n"
	" b resume_svc_thread_actual_start \n" // we store variables here, it isn't code
	" svc_saved_sp: .long 0 \n"
	" resume_svc_thread_actual_start: \n"
	" push {r0-r12,lr} \n"
	" mrs r1, cpsr \n"
	" push {r1} \n"
	" mrs r1, spsr \n"
	" push {r1} \n"
	" str sp, svc_saved_sp \n" // all registers saved now
	" ldr r2, [r0, #8] \n" // now load the registers
	" ldr r3, [r0, #12] \n"
	" ldr r4, [r0, #16] \n"
	" ldr r5, [r0, #20] \n"
	" ldr r6, [r0, #24] \n"
	" ldr r7, [r0, #28] \n"
	" ldr r8, [r0, #32] \n"
	" ldr r9, [r0, #36] \n"
	" ldr r10, [r0, #40] \n"
	" ldr r11, [r0, #44] \n"
	" ldr r12, [r0, #48] \n"
	" ldr r14, [r0, #56] \n"
	" ldr r1, [r0, #64] \n"
	" msr cpsr, r1 \n"
	" ldr r1, [r0, #68] \n"
	" msr spsr, r1 \n"
	//" ldr r1, [r0 + #52] \n"
	//" str r1, thread_saved_sp \n"
	" ldr sp, [r0, #52] \n" // change the stack to the threads stack
	" ldr r1, [r0, #60] \n" // load the threads pc
	" push {r1} \n" // push it, so we can jump via a pop after restoring r1 and r0
	" ldr r1, [r0, #4] \n"
	" ldr r0, [r0] \n"
	" pop {pc} \n"
	" svc_thread_return_point: \n"
	" ldr sp, svc_saved_sp \n"
	" pop {r1} \n"
	" msr spsr, r1 \n"
	" pop {r1} \n"
	" msr cpsr, r1 \n"
	" pop {r0-r12,lr} \n"
	"  \n"::"r" (regs):"memory");
	*/
}














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
	register volatile uint32_t *regs asm("r0") = t->regs;
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









































