#ifndef NOFLICKER_H
#define NOFLICKER_H

// will remap the lcd so the os can't write to it anymore
// also no other programm can write to it if it doesn't know the location, so we have to expose it in system calls
// there will be system calls to get the main gc and to force a blit to the screen
// there has to be an option to disable it, because it breaks programms that don't know about it







static uint32_t lcd_mirror_ptr[] = {0, 0, 0, 0, 0, 0,
                                                     0, 0, 0, 0,
                                                     0, 0, 0, 0,
                                                     0, 0, 0, 0,
                                                     0, 0,
                                                     0, 0,
                                                     0x110ED6D4, 0x111516D4,
						     0x110FD6DC, 0x111616DC,
						     0x113356DC, 0x113996DC,
						     0x113496E4, 0x113B16E4,
						     0x1134D6E4, 0x113B16E4};
//




//uint32_t *real_lcdc = (uint32_t*) 0xE0000000;
uint32_t volatile *real_lcdc = (uint32_t*) 0xe0000000;
volatile uint32_t *lcd_control_real = 0xe0000018;


uint16_t *buffupreal = NULL;

uint16_t *current_lcd_mirror_nonaligned = NULL;
uint16_t *real_lcd_framebuffer_nonaligned = NULL;
uint16_t *current_lcd_mirror = NULL;
uint16_t *real_lcd_framebuffer = NULL;

static volatile uint32_t lcd_control = 0;


bool noflicker = false;
static uint32_t old_abort_handler;


extern uint32_t sp_svc, lr_svc;
extern uint32_t sp_svc_undef, lr_svc_undef, undef_inst_addr, undef_sp;

bool undef_breakpoint = false;
uint32_t replaced_instruction = 0;


asm("undefined: udf #0");



bool use_undef_access = true;
int intmask = 0;

int lcd_compat_abort(uint32_t *regs)
{
    // Get the address that was tried to access
    uintptr_t fault_addr;
    asm volatile("mrc p15, 0, %[fault_addr], c6, c0, 0" : [fault_addr] "=r" (fault_addr));
	
	
	
	
    // Tried to access LCD controller?
    if(fault_addr >> 28 != 0xC)
        asm volatile("udf #0"); // Crash!
    
	
	if (! noflicker)
		asm volatile("udf #0"); // Crash!
	
	
    register uint32_t *translated_addr = 0;
    if(fault_addr == 0xC0000010)
        translated_addr = (uint32_t*) &current_lcd_mirror;
    else
	{
		//uart_printf("fault_adr: 0x%x\n",fault_addr);
		
		
		/*
		uint32_t inst = *(uint32_t*)(regs[13] - 8);
		
		
		
		uint32_t *tt_base;
		asm volatile("mrc p15, 0, %[tt_base], c2, c0, 0" : [tt_base] "=r" (tt_base));
		// Map real lcdc at 0xC0000000
		tt_base[0xC00] = 0xC0000C12;
		// Flush TLB for 0xC0000000
		asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (0xC0000000));
		
		
		return 2;
		uint32_t *regsasm asm("r0");
		uint32_t savedlr asm("r1");
		uint32_t savedsp asm("r2");
		savedlr = lr_svc;
		savedsp = sp_svc;
		regsasm = regs;
		
		extern uint32_t instruction_executed;
		instruction_executed = inst;
		
		clear_cache();
		// this makes everything blue yellow and yellow blue?
		asm volatile(
		"bkpt\n"
		"mov lr, r1\n"
		"mov sp, r2\n"
		"ldmia r0, {r0-r12, lr}\n"
		"bkpt #0\n"
		"instruction_executed: .word 0\n"
		"\n":::"r0","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10","r11","r12","r13","r14","cpsr");
		
		clear_cache();
		
		
		tt_base[(uint32_t) real_lcdc >> 20] = tt_base[0xC00];
		tt_base[0xC00] = 0;
		// Flush TLB for 0xC0000000 and real_lcdc
		asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (0xC0000000));
		asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (real_lcdc));
		return 0;
		*/
		
		
		if (use_undef_access)
		{
			// remap the lcd for the next instruction, because we care only about the framebuffer in this case and I don't know whether this handler can emulate all instructions (It probably doesn't)
			undef_breakpoint = true;
			
			//bkpt();
			
			/*
			asm("mrs r0, spsr\n"
			"bic r0, #0x40\n" // clear interrupt bit
			"msr spsr, r0\n"
			"mrs r0, cpsr\n"
			"bic r0, #0x40\n"
			"msr cpsr, r0":::"r0");
			*/
			//intmask = TCT_Local_Control_Interrupts(-1);
			//bkpt();
			
			uint32_t *tt_base;
			asm volatile("mrc p15, 0, %[tt_base], c2, c0, 0" : [tt_base] "=r" (tt_base));
			// Map real lcdc at 0xC0000000
			tt_base[0xC00] = 0xC0000C12;
			// Flush TLB for 0xC0000000
			asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (0xC0000000));
			
			
			clear_cache();
			
			void undefined();
			
			uint32_t *inst_adr_next = (uint32_t*)(regs[13] - 4);
			replaced_instruction = *inst_adr_next;
			*inst_adr_next = *((uint32_t*)undefined);
			
			return 1;
		}
		else
			translated_addr = (uint32_t*) (fault_addr - 0xC0000000 + (uintptr_t) real_lcdc);
	}
	//translated_addr = (uint32_t*) (fault_addr - 0xC0000000 + (uintptr_t) real_lcdc);
	
	
	
	
	
    // Read instruction that caused fault
    uint32_t inst = *(uint32_t*)(regs[13] - 8);
	
	/*
	uint32_t spsr asm("r0");
	asm volatile("mrs r0, spsr");
	uint32_t n = spsr & 0b10000000000000000000000000000000;
	uint32_t z = spsr & 0b01000000000000000000000000000000;
	uint32_t c = spsr & 0b00100000000000000000000000000000;
	uint32_t v = spsr & 0b00010000000000000000000000000000;
	uint32_t q = spsr & 0b00001000000000000000000000000000;
	*/
	
	
	
    // Get rd and type (store, load) of instruction
    if((inst & 0xC000000) != 0x4000000) // Not ldr or str?
    {
        if(inst == 0xE88C07F8) // Used by gbc4nspire (stmia r12, {r3-r10})
        {
            *translated_addr++ = regs[3];
            *translated_addr++ = regs[4];
            *translated_addr++ = regs[5];
            *translated_addr++ = regs[6];
            *translated_addr++ = regs[7];
            *translated_addr++ = regs[8];
            *translated_addr++ = regs[9];
            *translated_addr++ = regs[10];
        }
        else
             asm volatile("bkpt #0"); // For debugging
    }
    else
    {
        if((inst & (1 << 24)) == 0 || (inst & (1 << 21))) // Post-indexed
            asm volatile("bkpt #1");

        if((inst & (1 << 22))) // Byte
            asm volatile("bkpt #2");

        int rd = (inst >> 12) & 0xF;
	uint32_t *reg = 0;
        if(rd <= 12) // Not a banked register?
            reg = regs + rd;
        else if(rd == 13)
            reg = &sp_svc;
        else if(rd == 14)
            reg = &lr_svc;
        else // PC?
            asm volatile("bkpt #3");

        if(inst & (1 << 20)) // Load
		{
            *reg = *translated_addr;
		}
        else
		{
			if(fault_addr == 0xC0000018) // LCD control (mode etc.)
			{
				lcd_control = *translated_addr = *reg;
				
			}
			else
			{
				// commenting this fixed the flickering!
				//if(fault_addr > 0xC000000C) // Don't change the LCD timings
				//{
					if (translated_addr != 0xe0000010 && translated_addr != 0xe0000014 && translated_addr != 0xe000001c) // preventing from modifying the frambuffer pointer via the old address
						*translated_addr = *reg;
				//}
			}
		}
    }
	//volatile uint32_t *buffup = 0xe0000010;
	//*buffup = real_lcd_framebuffer;
	
	
	
	return 0;
}


void undef_handler(uint32_t *regs)
{
	
	
	
	if (! undef_breakpoint)
	{
		ut_calc_reboot();
	}
	
	
	// demap the lcd
	// Get address of translation table
	//asm volatile("bkpt");
	uint32_t *tt_base;
	asm volatile("mrc p15, 0, %[tt_base], c2, c0, 0" : [tt_base] "=r" (tt_base));
	// Map LCDC from 0xC0000000 to real_lcdc
	tt_base[(uint32_t) real_lcdc >> 20] = tt_base[0xC00];
	tt_base[0xC00] = 0;
	// Flush TLB for 0xC0000000 and real_lcdc
	asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (0xC0000000));
	asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (real_lcdc));
	
	clear_cache();
	
	
	// replace the undefined instruction with the real one
	
	
	//asm volatile("bkpt");
	uint32_t *inst_adr = (uint32_t*) undef_inst_addr;
	//asm volatile("bkpt");
	*inst_adr = replaced_instruction;
	//asm volatile("bkpt");
	undef_breakpoint = false;
	
	//TCT_Local_Control_Interrupts(intmask);
}






bool framebufferschanged = false;



char undef_stack[1024];
char *undef_sp_ptr = undef_stack+512;


asm(
"sp_svc: .word 0\n"
"lr_svc: .word 0\n"
"lcd_compat_abort_handler:\n"
"sub sp, sp, #8\n" // Somehow the OS uses this...
    "push {r0-r12, lr}\n"
        "mrs r0, spsr\n"
        "orr r0, r0, #0xc0\n"
        "msr cpsr_c, r0\n" // To SVC mode
            "str sp, sp_svc\n" // Save sp_svc
            "str lr, lr_svc\n" // Save lr_svc
        "msr cpsr_c, #0xd7\n" // Back to ABT mode
        "mov r0, sp\n" // First arg, array of r0-12, lr
        "bl lcd_compat_abort\n"
		"cmp r0, #1\n"
		"beq return_to_instruction\n"
		//"cmp r0, #2\n"
		//"beq execute_instruction\n"
        "mrs r0, spsr\n"
        "orr r0, r0, #0xc0\n"
        "msr cpsr_c, r0\n" // To SVC mode
            "ldr sp, sp_svc\n" // Restore sp_svc
            "ldr lr, lr_svc\n" // Restore lr_svc
        "msr cpsr_c, #0xd7\n" // Back to ABT mode
    "pop {r0-r12, lr}\n"
"add sp, sp, #8\n"
"subs pc, lr, #4\n"


		"return_to_instruction: mrs r0, spsr\n"
        "orr r0, r0, #0xc0\n"
        "msr cpsr_c, r0\n" // To SVC mode
            "ldr sp, sp_svc\n" // Restore sp_svc
            "ldr lr, lr_svc\n" // Restore lr_svc
        "msr cpsr_c, #0xd7\n" // Back to ABT mode
    "pop {r0-r12, lr}\n"
"add sp, sp, #8\n"
"subs pc, lr, #8"
);


asm(
"undef_sp: .word 0\n"
"undef_inst_addr: .word 0\n"
"sp_svc_undef: .word 0\n"
"lr_svc_undef: .word 0\n"
"lcd_undef_handler:\n"
//"bkpt\n"
	"ldr sp, undef_sp\n"
    "push {r0-r12, lr}\n"
		"sub lr, #4\n"
		"str lr, undef_inst_addr\n"
        "mrs r0, spsr\n"
        "orr r0, r0, #0xc0\n"
        "msr cpsr_c, r0\n" // To SVC mode
            "str sp, sp_svc_undef\n" // Save sp_svc
            "str lr, lr_svc_undef\n" // Save lr_svc
        "msr cpsr_c, #0b11011011\n" // Back to UNDEF mode
        "mov r0, sp\n" // First arg, array of r0-12, lr
		//"bkpt\n"
        "bl undef_handler\n"
		//"bkpt\n"
        "mrs r0, spsr\n"
        "orr r0, r0, #0xc0\n"
        "msr cpsr_c, r0\n" // To SVC mode
            "ldr sp, sp_svc_undef\n" // Restore sp_svc
            "ldr lr, lr_svc_undef\n" // Restore lr_svc
        "msr cpsr_c, #0b11011011\n" // Back to UNDEF mode
    "pop {r0-r12, lr}\n"
"add sp, sp, #8\n"
//"bkpt\n"
"subs pc, lr, #4");

/*
asm(
"sp_svc: .word 0\n"
"lr_svc: .word 0\n"
"lcd_compat_abort_handler:\n"
//"bkpt\n"
"sub sp, sp, #8\n" // Somehow the OS uses this...
    "push {r0-r12, lr}\n"
        "mrs r0, spsr\n"
        "orr r0, r0, #0xc0\n"
        "msr cpsr_c, r0\n" // To SVC mode
            "str sp, sp_svc\n" // Save sp_svc
            "str lr, lr_svc\n" // Save lr_svc
        "msr cpsr_c, #0xd7\n" // Back to ABT mode
        "mov r0, sp\n" // First arg, array of r0-12, lr
        "bl lcd_compat_abort\n"
        "mrs r0, spsr\n"
        "orr r0, r0, #0xc0\n"
        "msr cpsr_c, r0\n" // To SVC mode
            "ldr sp, sp_svc\n" // Restore sp_svc
            "ldr lr, lr_svc\n" // Restore lr_svc
        "msr cpsr_c, #0xd7\n" // Back to ABT mode
    "pop {r0-r12, lr}\n"
"add sp, sp, #8\n"
"subs pc, lr, #4");
*/


void blitMirrorToScreen()
{
	
	
	if (noflicker)
	{
		memcpy(real_lcd_framebuffer,current_lcd_mirror,320*240*2);
		//memcpy(real_lcd_framebuffer,buffupreal,320*240*2);
		
		
		
		/*
		uint16_t *tmp = current_lcd_mirror;
		current_lcd_mirror = real_lcd_framebuffer;
		real_lcd_framebuffer = tmp;
		volatile uint32_t *buffup = 0xe0000010;
		*buffup = real_lcd_framebuffer;
		*/
		
		
	}
	
	
	
	
}


//memcpy(real_lcd_framebuffer,current_lcd_mirror,320*240*2);
		
		
		
		
		/*
		uint16_t *tmp = current_lcd_mirror;
		current_lcd_mirror = real_lcd_framebuffer;
		real_lcd_framebuffer = tmp;
		volatile uint32_t *buffup = 0xe0000010;
		*buffup = real_lcd_framebuffer;
		*/



static volatile uint32_t *watchdog_load = (uint32_t*) 0x90060000,
                         *watchdog_control = (uint32_t*) 0x90060008,
                         *watchdog_intclear = (uint32_t*) 0x9006000C,
                         *watchdog_lock = (uint32_t*) 0x90060C00;

void enableNoflicker() // somehow flickers horribly on the calculator
{
	if (noflicker)
		return;
	
	void lcd_compat_abort_handler();
	
	uart_printf("abort handler address: %x\n",lcd_compat_abort_handler);
	
	current_lcd_mirror_nonaligned = malloc(sizeof(uint16_t)*320*240+8);
	real_lcd_framebuffer_nonaligned = malloc(sizeof(uint16_t)*320*240+8);
	
	
	real_lcd_framebuffer = real_lcd_framebuffer_nonaligned;
	current_lcd_mirror = current_lcd_mirror_nonaligned;
	
	/*
	char *tmp = current_lcd_mirror_nonaligned;
	while ((((uint32_t)tmp) & 0b111) != 0)
	{
		tmp = tmp+1;
	}
	current_lcd_mirror = tmp;
	
	
	tmp = real_lcd_framebuffer_nonaligned;
	while ((((uint32_t)tmp) & 0b111) != 0)
	{
		tmp = tmp+1;
	}
	real_lcd_framebuffer = tmp;
	*/
	
	
	
	if (current_lcd_mirror == NULL)
	{
		uart_printf("could not allocate lcd mirror!\n");
		return;
	}
	
	//current_lcd_mirror = *(uint16_t**)nl_osvalue(lcd_mirror_ptr,32);
	uart_printf("lcd mirror: %x\n",current_lcd_mirror);
	uart_printf("lcd real framebuffer: %x\n",real_lcd_framebuffer);
	
	
	// Get address of translation table
	uint32_t *tt_base;
	asm volatile("mrc p15, 0, %[tt_base], c2, c0, 0" : [tt_base] "=r" (tt_base));
	// Map LCDC from 0xC0000000 to real_lcdc
	tt_base[(uint32_t) real_lcdc >> 20] = tt_base[0xC00];
	tt_base[0xC00] = 0;
	// Flush TLB for 0xC0000000 and real_lcdc
	asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (0xC0000000));
	asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (real_lcdc));
	
	
	old_abort_handler = *((volatile uint32_t*)0x30);
	*((volatile uint32_t*)0x30) = (uint32_t) lcd_compat_abort_handler;
	
	
	
	void lcd_undef_handler();
	
	undef_sp = undef_sp_ptr;
	
	volatile uint32_t *undefined_instruction_address = 0x24;
	*undefined_instruction_address = lcd_undef_handler;
	
	
	noflicker = true;
	
	lcd_control = real_lcdc[6];
	
	//msleep(1000);
	
	
	volatile uint32_t *inter = 0xe000001c;
	*inter = 0;
	
	
	
	volatile uint32_t *buffup = 0xe0000010;
	buffupreal = *buffup;
	memcpy(real_lcd_framebuffer,buffupreal,320*240*2);
	memcpy(current_lcd_mirror,buffupreal,320*240*2);
	*buffup = real_lcd_framebuffer;
	volatile uint32_t *buffdown = 0xe0000014;
	*buffdown = real_lcd_framebuffer;
	
	
	
	
	
	/*
	//disabling lcd interrupts
	uint32_t cont = *lcd_control_real;
	cont &= (~ 0b00000000000011000000000000000000);
	*lcd_control_real = cont;
	*/
	
	
	//*(real_lcdc+0x14) = current_lcd_mirror;
	
	
	/*
	*(volatile uint32_t*) 0xDC00000C &= ~(1 << 3);
    // Deactivate watchdog IRQ
    *(volatile uint32_t*) 0xDC000014 = 1 << 3;
    *watchdog_lock = 0x1ACCE551;
    *watchdog_control = 0;
	*/
	
	
	ut_disable_watchdog();
	
	
	
	
	clear_cache();
	
	
	
	
	uart_printf("noflicker enabled\n");
	
	
	
	/*
	*(volatile uint32_t*)0x3C = (uint32_t) framebuffer_change_fiq;
	
	//enable lcd irq and set it to fiq
	*(volatile uint32_t*) 0xDC00000C = 1 << 21;
	*(volatile uint32_t*) 0xDC000010 = 1 << 21;
	
	
	// enable next base address update interrupt
	volatile uint32_t *lcdintmask = 0xe000001c;
	*lcdintmask = *lcdintmask | (1 << 2);
	
	
	
	// Enable FIQs
    uint32_t spsr;
	//asm volatile("msr cpsr, spsr");
    asm volatile("mrs %[spsr], spsr" : [spsr] "=r" (spsr));
    spsr &= ~0x40;
    asm volatile("msr spsr_c, %[spsr]" :: [spsr] "r" (spsr));
	//asm volatile("mrs cpsr, spsr");
	*/
	
	
	
	
}





void disableNoflicker()
{
	if (! noflicker)
		return;
	
	/*
	*(volatile uint32_t*) 0xDC00000C &= ~(1 << 21);
    *(volatile uint32_t*) 0xDC000014 = 1 << 21;
	*/
	
	
	volatile uint32_t *buffup = 0xe0000010;
	*buffup = buffupreal;
	
	
	uint32_t *tt_base;
	asm volatile("mrc p15, 0, %[tt_base], c2, c0, 0" : [tt_base] "=r" (tt_base));
	// Map real lcdc at 0xC0000000
	tt_base[0xC00] = 0xC0000C12;
	// Flush TLB for 0xC0000000
	asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (0xC0000000));
	
	
	free(current_lcd_mirror_nonaligned);
	free(real_lcd_framebuffer_nonaligned);
	current_lcd_mirror = NULL;
	real_lcd_framebuffer = NULL;
	current_lcd_mirror_nonaligned = NULL;
	real_lcd_framebuffer_nonaligned = NULL;
	
	
	
	
	//*((volatile uint32_t*)0x30) = old_abort_handler;
	clear_cache();
	noflicker = false;
}


































#endif
