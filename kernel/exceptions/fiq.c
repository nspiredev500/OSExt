#include "../kernel.h"



static volatile uint32_t *fiq_address = (volatile uint32_t*) 0x30;


asm(
".global relay_watchdog_fiq_to_ndless \n"
"relay_watchdog_fiq_to_ndless: .long 0 \n"
".global fiq_wrapper \n"
"fiq_wrapper: \n" // don't push all registers, because the fiq mode has private r8-r14 registers
"push {r0-r7,r14} \n"
"mrs r0, cpsr \n"
"push {r0} \n" // push the cpsr
"mrs r0, spsr \n"
"sub r1, lr, #4 \n"
"mov r2, sp \n"
"bl fiq_handler \n"
"ldr r0, relay_watchdog_fiq_to_ndless \n"
"cmp r0, #1 \n"
"beq _relay_watchdog_ndless \n"
"pop {r0} \n" // pop the cpsr
"msr cpsr, r0 \n"
"pop {r0-r7,r14} \n" // pop the rest of the registers
"subs pc, lr, #4 \n"
"_relay_watchdog_ndless: \n"
"mov r0, #0 \n"
"str r0, relay_watchdog_fiq_to_ndless \n"
"pop {r0} \n" // pop the cpsr
"msr cpsr, r0 \n"
"pop {r0-r7,r14} \n" // pop the rest of the registers
"mov pc, #0x1c \n");

extern volatile uint32_t relay_watchdog_fiq_to_ndless;

static void __attribute__ ((noreturn)) reset(void) {
	*(volatile unsigned*)0x900A0008 = 2;
	__builtin_unreachable();
}

static void fiq_return_thread(uint32_t spsr,void* address,uint32_t *regs)
{
	struct thread* running_thread = scheduler_running();
	if (running_thread != NULL)
	{
		// updating the thread's registers
		running_thread->regs[16] = spsr;
		running_thread->regs[15] = (uint32_t) address;
		for (uint8_t i = 0;i<=7;i++)
		{
			running_thread->regs[i] = regs[i];
		}
		register volatile uint32_t *t_regs asm("r0") = running_thread->regs;
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
	scheduler_return(SCHEDULER_OTHER);
}

void fiq_handler(uint32_t spsr,void* address, uint32_t *regs) // regs is the old r0-r7
{
	uint32_t fiq_status = vic_fiq_status();
	//debug_shell_println("fiq: %d",fiq_status);
	//debug_shell_println("rtc irq status: %d",rtc_irq_status());
	if (fiq_status & (0b1 << 1)) // uart port
	{
		panic("fiq from uart port!\n");
	}
	if (fiq_status & (0b1 << 3)) // watchdog timer
	{
		if (watchdog_function() == WATCHDOG_INVALID)
		{
			// if an fiq occurs in invalid watchdog mode, assume it's Ndless lcd compatibility feature and relay it
			/// TODO do not relay in standalone mode
			relay_watchdog_fiq_to_ndless = 1;
			return;
		}
		watchdog_irq_clear();
	}
	if (fiq_status & (0b1 << 4)) // RTC
	{
		//debug_shell_println("rtc interrupt");
		systime_rtc_overflow();
		rtc_irq_clear();
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
		timer_irq_clear(0,0);
		timer_irq_clear(0,1);
	}
	if (fiq_status & (0b1 << 18)) // first timer
	{
		if (timer_irq_status(SYSTIME_TIMER))
		{
			//debug_shell_println("milis timer overflow!");
			systime_timer_overflow();
		}
		if (timer_irq_status(SCHEDULER_TIMER) && ! syscall_in_progress())
		{
			/// TODO schedule another thread now
			
			
		}
		DEBUGPRINTLN_1("fiq from first timer!")
		//debug_shell_println("fiq from first timer!");
		timer_irq_clear(1,0);
		timer_irq_clear(1,1);
	}
	if (fiq_status & (0b1 << 19)) // second timer
	{
		if (timer_irq_status(SYSTIME_TIMER))
		{
			systime_timer_overflow();
		}
		//DEBUGPRINTLN_1("fiq from second timer!")
		timer_irq_clear(2,0);
		timer_irq_clear(2,1);
	}
	if (fiq_status & (0b1 << 20)) // I2C
	{
		panic("fiq from I2C controller!\n");
	}
	if (fiq_status & (0b1 << 21)) // LCD controller
	{
		panic("fiq from LCD controller!\n");
	}
	//debug_shell_println("fiq after: %d",vic_fiq_status());
	
	
	
	
	
	
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













