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
"bic r0, r0, #1 \n" // if pc+2 ends with 1 because we were in thumb mode, we have to clear that before loading
"ldrh r0, [lr, #-2] \n" // load instruction
"bic r0, r0, #0xff00 \n" // extract swi number
"push {r1} \n" // now push all registers to be the same as in the arm mode syscall handler
"mrs r1, spsr \n"
"push {r1} \n"
"push {r2-r12,r14} \n"
"mov r1, sp \n"
"bl swi_handler_usr \n"
"pop {r2-r12,r14} \n"
"pop {r1} \n"
"msr spsr, r1 \n"
"pop {r1} \n"
"pop {r0} \n"
"msr cpsr, r0 \n"
"pop {r0} \n"
"movs pc, lr \n"
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
void (*swi_table[SYSCALL_SIZE])(uint32_t* regs);



#define USER_SYSCALL_SIZE 0
void (*user_swi_table[USER_SYSCALL_SIZE])(uint32_t* regs);

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
	13: stat
	14: times
	15: unlink
	16: wait
	17: gettimeofday
	
	
*/


void syscall_set_reg(uint32_t *regs,uint32_t reg,uint32_t value)
{
	if (reg > 1 && reg < 13)
	{
		regs[reg-2] = value;
	}
	if (reg == 1)
	{
		regs[13] = value;
	}
	if (reg == 0)
	{
		regs[15] = value;
	}
}
uint32_t syscall_get_reg(uint32_t *regs,uint32_t reg)
{
	if (reg > 1 && reg < 13)
	{
		return regs[reg-2];
	}
	if (reg == 1)
	{
		return regs[13];
	}
	if (reg == 0)
	{
		return regs[15];
	}
	return 0;
}



void init_syscall_table()
{
	swi_table[0] = uninstall_osext;
	
	
	
	
}


void swi_handler_usr(uint32_t swi_number, uint32_t* regs) // regs is r2-r12, svc lr, spsr, r1, cpsr, r0
{
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
	user_swi_table[swi_number](regs);
	
	
	
	
	// TODO restart the timeslice timer
	
	
}



void swi_handler(uint32_t swi_number, uint32_t* regs) // regs is r2-r12, svc lr, spsr, r1, cpsr, r0
{
	DEBUGPRINTLN_1("r2: 0x%x",regs[0])
	DEBUGPRINTLN_1("r3: 0x%x",regs[1])
	DEBUGPRINTLN_1("r4: 0x%x",regs[2])
	DEBUGPRINTLN_1("r5: 0x%x",regs[3])
	DEBUGPRINTLN_1("r6: 0x%x",regs[4])
	DEBUGPRINTLN_1("r7: 0x%x",regs[5])
	DEBUGPRINTLN_1("r8: 0x%x",regs[6])
	DEBUGPRINTLN_1("r9: 0x%x",regs[7])
	DEBUGPRINTLN_1("r10: 0x%x",regs[8])
	DEBUGPRINTLN_1("r11: 0x%x",regs[9])
	DEBUGPRINTLN_1("r12: 0x%x",regs[10])
	DEBUGPRINTLN_1("swi lr: 0x%x",regs[11])
	DEBUGPRINTLN_1("spsr: 0x%x",regs[12])
	DEBUGPRINTLN_1("r1: 0x%x",regs[13])
	DEBUGPRINTLN_1("cpsr: 0x%x",regs[14])
	DEBUGPRINTLN_1("r0: 0x%x",regs[15])
	
	
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
	swi_table[swi_number](regs);
	
	DEBUGPRINTLN_1("r2: 0x%x",regs[0])
	DEBUGPRINTLN_1("r3: 0x%x",regs[1])
	DEBUGPRINTLN_1("r4: 0x%x",regs[2])
	DEBUGPRINTLN_1("r5: 0x%x",regs[3])
	DEBUGPRINTLN_1("r6: 0x%x",regs[4])
	DEBUGPRINTLN_1("r7: 0x%x",regs[5])
	DEBUGPRINTLN_1("r8: 0x%x",regs[6])
	DEBUGPRINTLN_1("r9: 0x%x",regs[7])
	DEBUGPRINTLN_1("r10: 0x%x",regs[8])
	DEBUGPRINTLN_1("r11: 0x%x",regs[9])
	DEBUGPRINTLN_1("r12: 0x%x",regs[10])
	DEBUGPRINTLN_1("swi lr: 0x%x",regs[11])
	DEBUGPRINTLN_1("spsr: 0x%x",regs[12])
	DEBUGPRINTLN_1("r1: 0x%x",regs[13])
	DEBUGPRINTLN_1("cpsr: 0x%x",regs[14])
	DEBUGPRINTLN_1("r0: 0x%x",regs[15])
}
































