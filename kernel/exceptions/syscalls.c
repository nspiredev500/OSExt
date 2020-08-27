#include "../kernel.h"
#include "../syscalls/usr/syscalls.h"

bool syscall_tracker = false;
extern uint32_t syscall_mask;

void syscall_log(uint32_t *instruction)
{
	uint32_t inst = *(instruction-1);
	uint32_t swi = inst & 0xffffff;
	//if (swi != 14  && swi != 75 && swi != 8 && swi != 2097160) // exclude TCT_Local_Control_Interrupts, because it's called so much in the hooks, Touchpad_read is called too much in isKeyPressed, also memcpy
		DEBUGPRINTLN_1("Syscall: %d",swi);
}


asm(
".extern syscall_log\n"
".global __syscall_in_progress \n"
".global delegate_system_calls \n"
".global swi_wrapper \n"
".global syscall_mask \n"
"__syscall_in_progress: .long 0 \n"
"syscall_mask: .long 0xf80000 \n"
"swi_address: .long 0x8 \n"


"swi_wrapper: \n"
//BREAKPOINT_ASM
"push {r0} \n"
"ldr r0, __syscall_in_progress \n"
"add r0, r0, #1 \n"
"str r0, __syscall_in_progress \n" // indicate that a syscall is in progress

/*
// for logging syscalls
"push {r0-r12,r14} \n"
"mov r0, lr \n"
"bl syscall_log\n"
"pop {r0-r12,r14} \n"
*/

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
"poplo {r1} \n"
"blo delegate \n" // delegate if not right
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
"ldr r0, __syscall_in_progress \n"
"sub r0, r0, #1 \n"
"str r0, __syscall_in_progress \n" // indicate that the syscall ended
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
"ldr r0, __syscall_in_progress \n"
"sub r0, r0, #1 \n"
"str r0, __syscall_in_progress \n" // indicate that the syscall ended
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
"ldr r0, __syscall_in_progress \n"
"sub r0, r0, #1 \n"
"str r0, __syscall_in_progress \n" // indicate that the syscall ended
"pop {r0} \n"
"movs pc, lr \n"
" \n"
" \n"
" \n"
" \n"
" \n"
"delegate: \n"

//"pop {r0} \n" // restore the cpsr
//"msr cpsr, r0 \n"
//"pop {r0} \n" // restore r0
//"ldr pc, swi_address \n"





"b real_delegate\n"
"saved_lr1: .word 0\n"
"saved_lr2: .word 0\n"
"saved_spsr1:.word 0\n"
"saved_spsr2:.word 0\n"

/// TODO convert the delegate to not using the stack, as functions such as fprintf expect the arguments to be there

"real_delegate: \n"
//BREAKPOINT_ASM
"ldr r0, saved_lr1 \n"
"cmp r0, #0 \n"
"ldr r0, saved_lr1 \n"
"cmp r0, #0 \n"
"strne lr, saved_lr2 \n"
"streq lr, saved_lr1 \n"
"mrs r0, spsr \n"
"strne r0, saved_spsr2 \n"
"streq r0, saved_spsr1 \n"
" \n"
"and r0, r0, #0b100000 \n"
"cmp r0, #0b100000 \n" // test for thumb bit
"beq real_delegate_thumb \n"
"pop {r0} \n" // pop the cpsr
"mrs r0, cpsr \n"
"msr spsr, r0 \n" // pretend our cpsr is the callers cpsr
"pop {r0} \n" // pop the original r0
"ldr lr, [lr, #-4] \n"
"str lr, SWI_INST_ARM \n"
"add lr, pc, #4 \n"
"mov pc, #0x8 \n" // jump to the system call vector
"SWI_INST_ARM: .word 0 \n" // the swi instruction will be put here, so the ndless swi handler can extract the swi number
"b delegate_end \n"


"real_delegate_thumb: \n"
//BREAKPOINT_ASM
"pop {r0} \n" // pop the cpsr
"mrs r0, cpsr \n"
"orr r0,r0, #0b100000 \n" // set the thumb bit
"msr spsr, r0 \n" // pretend our cpsr is the callers cpsr, and set the thumb bit
"pop {r0} \n" // pop the original r0
"ldrh lr, [lr, #-2] \n"
"str lr, SWI_INST_THUMB \n"
" \n"
"mov lr, pc\n"
"mov pc, #0x8 \n" // jump to the system call vector
" \n"
" \n"
"SWI_INST_THUMB: .short 0 \n"
".thumb \n" // the swi handler thinks we're in thumb mode, so returns to us in thumb mode
"blx delegate_end \n"
".align 4\n" // go back to arm mode
".arm \n"
"delegate_end: \n"
//BREAKPOINT_ASM
"push {r0} \n"
	"ldr lr, saved_lr2 \n"
	"cmp lr, #0 \n" // check if returning from a recursive syscall
	"mov r0, #0 \n"
	"beq after_not \n"
	//BREAKPOINT_ASM
	"strne r0, saved_lr2 \n" // saved_lr2 not 0, so recursive syscall, use the 2. variables
	"ldrne r0, saved_spsr2 \n"
	"msrne spsr, r0 \n"
	"movne r0, #0 \n"
	"strne r0, saved_spsr2 \n"
	"after_not: \n"
	"ldreq lr, saved_lr1 \n" // not a recursive syscall, use the first variables
	"streq r0, saved_lr1 \n"
	"ldreq r0, saved_spsr1 \n"
	"msreq spsr, r0 \n"
	"moveq r0, #0 \n"
	"streq r0, saved_spsr1 \n"
	"push {r1} \n"
		"mrs r0, cpsr \n"
		"mrs r1, spsr \n"
		"and r0, r0, #0x80 \n" // extract the I bit
		"bic r1, #0x80 \n" // clear the I bit in the spsr
		"orr r1, r1, r0 \n" // put the I bit of the cpsr into the spsr
		"msr spsr, r1 \n"
	"pop {r1} \n"
	"ldr r0, __syscall_in_progress \n"
	"sub r0, r0, #1 \n"
	"str r0, __syscall_in_progress \n" // indicate that the syscall ended
"pop {r0} \n"
"movs pc, lr \n"
/*
"real_delegate: push {lr} \n" // lr gets overwritten in the Ndless swi handler
	"ldr lr, [lr, #-4] \n" // load the original instruction
	"str lr, __SYSCALL_INST \n" // save the instruction where the Ndless syscall handler thinks the syscall originated from, to let it extract the swi number
	"mrs lr, spsr \n"
	"push {lr} \n" // push the original spsr
		"mrs lr, cpsr \n"
		"msr spsr, lr \n" // pretend our cpsr is the syscall cpsr
		" \n" // now we have saved the lr and spsr registers, which get lost in the syscall handler and restored all old registers
		//BREAKPOINT_ASM
		"mov lr, pc \n" // set the return address
		"add lr, #8 \n"
		"mov pc, #0x8 \n" // now jump to the syscall vector
		"__SYSCALL_INST: .long 0 \n" // place the original syscall instruction here, so the Ndless swi handler can correctly extract the swi number
		//BREAKPOINT_ASM
		"push {r0} \n" // return here, save r0
			"ldr r0, __syscall_in_progress \n"
			"sub r0, r0, #1 \n"
			"str r0, __syscall_in_progress \n" // indicate that the syscall ended
		"pop {r0} \n" // restore r0
	"pop {lr} \n"
	"msr spsr, lr \n" // restore the original spsr
"pop {lr} \n" // restore the lr
"movs pc, lr \n" // return from the syscall
*/
" \n");

extern volatile uint32_t __syscall_in_progress;

bool syscall_in_progress()
{
	if (__syscall_in_progress != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t nested_syscalls()
{
	return __syscall_in_progress;
}


// marks a syscall as finished by decrementing __syscall_in_progress
// should only be used when the syscall uses another way to return than the syscall handler, to net mess up the syscall count
void syscall_mark_as_finished()
{
	bool fiq = isFIQ();
	disableFIQ();
	__syscall_in_progress--;
	if (fiq)
	{
		enableFIQ();
	}
}


#define SYSCALL_SIZE 13
void (*swi_table[SYSCALL_SIZE])(volatile uint32_t* regs);



#define USER_SYSCALL_SIZE 1
void (*user_swi_table[USER_SYSCALL_SIZE])(volatile uint32_t* regs);

/*
	User Mode System Calls:
	0: exit
	1: mmap
	
	
	newlib:
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


void syscall_set_reg(volatile uint32_t *regs,uint32_t reg,uint32_t value)
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
	if (reg == 15)
	{
		regs[11] = value;
	}
}
uint32_t syscall_get_reg(volatile uint32_t *regs,uint32_t reg)
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
	if (reg == 15)
	{
		return regs[11];
	}
	if (reg == 13)
	{
		return (uint32_t) (regs+16); // the stack uses stmdb, so increment after means the original value of sp pointed to is where r0 now is +4
	}
	if (reg == 16) // my index for the cpsr
	{
		return regs[12];
	}
	return 0;
}

static void dummy_syscall() {};


void init_syscall_table()
{
	swi_table[0] = uninstall_osext;
	swi_table[1] = syscall_version;
	swi_table[2] = syscall_unix_time;
	swi_table[3] = syscall_unix_time_milli;
	swi_table[4] = syscall_unix_time_micro;
	swi_table[5] = syscall_set_unix_time;
	swi_table[6] = syscall_set_unix_time_milli;
	swi_table[7] = syscall_enable_kernel_thread_scheduling;
	swi_table[8] = syscall_disable_kernel_thread_scheduling;
	swi_table[9] = syscall_kernel_thread_wait_millis;	
	swi_table[10] = syscall_kernel_thread_wait_for;
	swi_table[11] = syscall_kernel_thread_create;
	swi_table[12] = syscall_kernel_thread_delete;
	
	
	
	
	
	user_swi_table[0] = dummy_syscall;
	
	
	
}


void swi_handler_usr(uint32_t swi_number,volatile  uint32_t* regs) // regs is r2-r12, svc lr, spsr, r1, cpsr, r0
{
	uint32_t max_swi = sizeof(user_swi_table)/sizeof(uint32_t (*)());
	if (swi_number >= max_swi)
	{
		DEBUGPRINTF_1("unknown syscall: %d\n",swi_number)
		//panic("unknown syscall!");
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



void swi_handler(uint32_t swi_number,volatile  uint32_t* regs) // regs is r2-r12, svc lr, spsr, r1, cpsr, r0
{
	/*
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
	*/
	
	uint32_t max_swi = sizeof(swi_table)/sizeof(uint32_t (*)());
	if (swi_number >= max_swi)
	{
		DEBUGPRINTF_1("unknown syscall: %d\n",swi_number)
		panic("unknown syscall!");
	}
	//DEBUGPRINTF_1("syscall: %d\n",swi_number)
	swi_table[swi_number](regs);
	
	
	/*
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
	*/
}
































