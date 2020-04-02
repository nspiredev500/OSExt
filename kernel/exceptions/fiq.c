#include "../kernel.h"



static volatile uint32_t *fiq_address = (volatile uint32_t*) 0x30;


asm(
".global fiq_wrapper \n"
"fiq_wrapper: \n" // don't push all registers, because the fiq mode has private r8-r14 registers
"push {r0-r8,r14} \n" // also push r8 to make the stack 8-byte-aligned
"mrs r0, spsr \n"
"sub r1, lr, #4 \n"
"mov r2, sp \n"
"bl fiq_handler \n"
"pop {r0-r8,r14} \n"
"subs pc, lr, #4 \n");

static void fiq_return_thread(uint32_t spsr,void* address,uint32_t *regs)
{
	if (running_thread != NULL)
	{
		// updating the thread's registers
		running_thread->regs[16] = spsr;
		running_thread->regs[15] = (uint32_t) address;
		for (uint8_t i = 0;i<=7;i++)
		{
			running_thread->regs[i] = regs[i];
		}
		register uint32_t *t_regs asm("r0") = running_thread->regs;
		asm volatile(
		" mrs r1, cpsr\n"
		" orr r1, r1, #31\n"
		" msr cpsr, r1 \n" // go into system mode to manipulate user mode registers
		" str sp, [r0, #32]\n" // store the r8
		" str sp, [r0, #36]\n" // store the r9
		" str sp, [r0, #40]\n" // store the r10
		" str sp, [r0, #44]\n" // store the r11
		" str sp, [r0, #48]\n" // store the r12
		" str sp, [r0, #52]\n" // store the sp
		" str lr, [r0, #56]\n" // store the lr
		" bic r1, r1, #8 \n"
		" msr cpsr, r1 \n" // switch back to fiq mode
		"  \n"::"r" (t_regs):"r1");
	}
	else
	{
		panic("fiq from user mode, but no thread is running!\n");
	}
	// jump back into kernel mode, to svc_lr and use r0 to indicate success
	register uint32_t *fiq_stack_start asm("r0") = fiq_stack+sizeof(fiq_stack)/4-4;
	asm volatile(
	" mov sp, r0 \n" // reset the fiq stack to the start
	" mov r0, #1 \n" // 0 indicates normal return
	" bic r1, r1, #31 \n" // clear the mode bits
	" orr r1, r1, #19 \n" // set the mode to svc
	" msr cpsr, r1 \n"
	" bx lr \n" // setting the right other cpsr bits is done in thread.c, svc lr is the return address
	:"+r" (fiq_stack_start):"r" (fiq_stack_start):"r1");
	__builtin_unreachable();
}

void fiq_handler(uint32_t spsr,void* address, uint32_t *regs) // regs is the old r0-r7
{
	uint32_t fiq_status = vic_fiq_status();
	if (fiq_status & (0b1 << 1)) // uart port
	{
		panic("fiq from uart port!\n");
	}
	if (fiq_status & (0b1 << 3)) // watchdog timer
	{
		if (watchdog_function() == WATCHDOG_INVALID)
		{
			panic("watchdog fiq in invalid watchdog mode!\n");
		}
		watchdog_irq_clear();
	}
	if (fiq_status & (0b1 << 4)) // RTC
	{
		panic("fiq from rtc!\n");
	}
	if (fiq_status & (0b1 << 7)) // GPIO
	{
		panic("fiq from gpio!\n");
	}
	if (fiq_status & (0b1 << 8)) // USB OTG
	{
		panic("fiq from usb otg controller!\n");
	}
	if (fiq_status & (0b1 << 9)) // USB Host
	{
		panic("fiq from usb host controller!\n");
	}
	if (fiq_status & (0b1 << 11)) // ADC
	{
		panic("fiq from uart port!\n");
	}
	if (fiq_status & (0b1 << 14)) // HDQ/1-Wire
	{
		panic("fiq from HDQ/1-Wire!\n");
	}
	if (fiq_status & (0b1 << 15)) // Power Management
	{
		panic("fiq from power management!\n");
	}
	if (fiq_status & (0b1 << 16)) // Keypad
	{
		panic("fiq from keypad!\n");
	}
	if (fiq_status & (0b1 << 17)) // Fast timer
	{
		timer_irq_clear(0);
	}
	if (fiq_status & (0b1 << 18)) // first timer
	{
		timer_irq_clear(1);
	}
	if (fiq_status & (0b1 << 19)) // second timer
	{
		timer_irq_clear(2);
	}
	if (fiq_status & (0b1 << 20)) // I2C
	{
		panic("fiq from I2C controller!\n");
	}
	if (fiq_status & (0b1 << 21)) // LCD controller
	{
		panic("fiq from LCD controller!\n");
	}
	
	
	
	
	
	
	
	/*
	if (vic_fiq_status() != 0)
		panic("fiq handler could not acknowledge all interrupts!\n");
	*/
	if ((spsr & 0b11111) == 0b10000) // usr mode
	{
		if (fiq_status & (0b1 << 3) && watchdog_function() == WATCHDOG_WAIT) // watchdog timer
		{
			fiq_return_thread(spsr,address,regs);
		}
	}
	else // privileged modes
	{
		if (fiq_status & (0b1 << 3) && watchdog_function() == WATCHDOG_WAIT) // watchdog timer
		{
			watchdog_disable();
		}
	}
}



// TODO put the page with the fiq handler in the lockdown-tlb, by making an offset variable in the linker script













