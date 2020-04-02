#include "../kernel.h"

bool syscall_tracker = false;
extern uint32_t syscall_mask;

asm(
".global delegate_system_calls \n"
".global swi_wrapper \n"
".global syscall_mask \n"
"syscall_mask: .long 0xf80000 \n"
"swi_address: .long 0x8 \n"
"swi_wrapper: \n"


"push {r0} \n"
"mrs r0, cpsr \n"
"push {r0} \n"
"mrs r0, spsr \n"
"and r0, r0, #0b100000 \n"
"cmp r0, #0b100000 \n"
"beq thumb_syscall \n"
" \n" // arm syscall
"mrs r0, spsr \n"
"and r0, r0, #0b11111 \n" // extract the mode bits
"cmp r0, #0b10000 \n"
"beq arm_usermode \n"
" \n" // arm svc
"ldr r0, [lr, #-4] \n" // load instruction
"bic r0, r0, #0xff000000 \n" // extract swi number
"push {r1} \n"
"ldr r1, syscall_mask \n"
"cmp r0, r1 \n" // check against the mask
"popne {r1} \n"
"bne delegate \n" // delegate if not right
" \n" // only osext syscalls here
" \n"
"bic r0, r0, r1\n"
"mrs r1, spsr \n"
"push {r1} \n" // push the spsr, so it isn't lost in recursive syscalls
"push {r2-r12,r14} \n"
"mov r1, sp \n" // pointer to the register array
//".long 0xE1212374 \n" // breakpoint
"bl swi_handler \n"
"pop {r2-r12,r14} \n"
"pop {r0} \n"
"msr spsr, r0 \n" // restore the spsr
"pop {r1} \n" // pop the unused r1 pushed earlier
"pop {r0} \n"
"msr cpsr, r0 \n" // restore the cpsr
"pop {r0} \n" // remove the saved r0 from the stack
"movs pc, lr \n" // return from the swi
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
"arm_usermode: \n"
"ldr r0, [lr, #-4] \n" // load instruction
"bic r0, r0, #0xff000000 \n" // extract swi number
//".long 0xE1212374 \n" // breakpoint
"push {r1} \n"
"mrs r1, spsr \n"
"push {r1} \n" // push the spsr, so it isn't lost in recursive syscalls
"push {r2-r12,r14} \n"
"mov r1, sp \n" // pointer to the register array
"bl swi_handler_usr \n"
" \n"
" \n"
"pop {r2-r12,r14} \n"
"pop {r1} \n"
"msr spsr, r1 \n"
"pop {r1} \n"
"pop {r0} \n"
"msr cpsr, r0 \n"
"pop {r0} \n"
"movs pc, lr \n" // return from the swi
" \n"
" \n"
" \n"
"thumb_syscall: \n"
"mrs r0, spsr \n"
"and r0, r0, #0b11111 \n" // extract the mode bits
"cmp r0, #0b10000 \n"
"beq thumb_usermode \n"
" \n" // thumb svc
"b delegate \n" // just delegate, because the thumb swi instruction doesn't support high enough swi values to reach the mask
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
"thumb_usermode: \n"
"ldrh r0, [lr, #-2] \n" // load instruction
"bic r0, r0, #0xff00 \n" // extract swi number
".long 0xE1212374 \n" // breakpoint
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
"delegate: \n"
"pop {r0} \n" // restore the cpsr
"msr cpsr, r0 \n"
"pop {r0} \n" // restore r0
"ldr pc, swi_address \n"
" \n"
" \n"
" \n");



#define SYSCALL_SIZE 1
uint32_t (*swi_table[SYSCALL_SIZE])(uint32_t* regs);



#define USER_SYSCALL_SIZE 0
uint32_t (*user_swi_table[USER_SYSCALL_SIZE])(uint32_t* regs);

/*
	User Mode System Calls:
	0: exit
	1: open
	2: close
	3: read
	4: write
	5: execve
	6: fork
	7: fstat
	8: getpid
	9: isatty
	10: kill
	11: link
	12: lseek
	13: sbrk
	14: stat
	15: times
	16: unlink
	17: wait
	18: gettimeofday
	
	
*/




void init_syscall_table()
{
	swi_table[0] = uninstall_osext;
	
	
	
	
}


uint32_t swi_handler_usr(uint32_t swi_number, uint32_t* regs) // regs is r2-r12, spsr, r1, cpsr, r0
{
	//asm(".long 0xE1212374"); // bkpt
	uint32_t max_swi = sizeof(user_swi_table)/sizeof(uint32_t (*)());
	if (swi_number >= max_swi)
	{
		DEBUGPRINTF_1("unknown syscall: %d\n",swi_number)
		panic("unknown syscall!");
	}
	if (syscall_tracker)
	{
		DEBUGPRINTF_1("syscall: %d\n",swi_number)
	}
	
	
	//  TODO stop the timeslice timer
	
	// TODO use system time to record the time spend in the system call
	uint32_t ret = user_swi_table[swi_number](regs);
	
	
	
	
	// TODO restart the timeslice timer
	
	
	return ret;
}



uint32_t swi_handler(uint32_t swi_number, uint32_t* regs) // regs is r2-r12, spsr, r1, cpsr, r0
{
	//asm(".long 0xE1212374"); // bkpt
	uint32_t max_swi = sizeof(swi_table)/sizeof(uint32_t (*)());
	if (swi_number >= max_swi)
	{
		DEBUGPRINTF_1("unknown syscall: %d\n",swi_number)
		panic("unknown syscall!");
	}
	if (syscall_tracker)
	{
		DEBUGPRINTF_1("syscall: %d\n",swi_number)
	}
	return swi_table[swi_number](regs);
}
































