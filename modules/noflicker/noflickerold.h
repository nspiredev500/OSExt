#ifndef NOFLICKER_H
#define NOFLICKER_H

// will remap the lcd so the os can't write to it anymore
// also no other programm can write to it if it doesn't know the location, so we have to expose it in system calls
// there will be system calls to get the main gc and to force a blit to the screen
// there has to be an option to disable it, because it breaks programms that don't know about it


uint32_t *real_lcdc = (uint32_t*) 0xE0000000;





static volatile uint32_t lcd_control = 0;





extern uint32_t sp_svc, lr_svc;

void lcd_compat_abort(uint32_t *regs)
{
    // Get the address that was tried to access
    uintptr_t fault_addr;
    asm volatile("mrc p15, 0, %[fault_addr], c6, c0, 0" : [fault_addr] "=r" (fault_addr));

    // Tried to access LCD controller?
    if(fault_addr >> 28 != 0xC)
        asm volatile("udf #0"); // Crash!
    
    uint32_t *translated_addr = 0;
	/*
    if(fault_addr == 0xC0000010)
        translated_addr = (uint32_t*) &current_lcd_mirror;
    else
        translated_addr = (uint32_t*) (fault_addr - 0xC0000000 + (uintptr_t) real_lcdc);
	*/
	translated_addr = (uint32_t*) (fault_addr - 0xC0000000 + (uintptr_t) real_lcdc);
    // Read instruction that caused fault
    uint32_t inst = *(uint32_t*)(regs[13] - 8);

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
            *reg = *translated_addr;
        else if(fault_addr == 0xC0000018) // LCD control (mode etc.)
            lcd_control = *translated_addr = *reg;
        else if(fault_addr > 0xC000000C) // Don't change the LCD timings
            *translated_addr = *reg;
    }
	
	
}


extern unsigned int lcd_compat_abort_handler;
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
        "mrs r0, spsr\n"
        "orr r0, r0, #0xc0\n"
        "msr cpsr_c, r0\n" // To SVC mode
            "ldr sp, sp_svc\n" // Restore sp_svc
            "ldr lr, lr_svc\n" // Restore lr_svc
        "msr cpsr_c, #0xd7\n" // Back to ABT mode
    "pop {r0-r12, lr}\n"
"add sp, sp, #8\n"
"subs pc, lr, #4");

bool noflicker = false;
static uint32_t old_abort_handler;




void enableNoflicker()
{
	if (noflicker)
		return;
	
	
	
	
	lcd_control = real_lcdc[6];
	
	// Get address of translation table
	uint32_t *tt_base;
	asm volatile("mrc p15, 0, %[tt_base], c2, c0, 0" : [tt_base] "=r" (tt_base));
	// Map LCDC from 0xC0000000 to real_lcdc
	tt_base[(uint32_t) real_lcdc >> 20] = tt_base[0xC00];
	tt_base[0xC00] = 0;
	// Flush TLB for 0xC0000000 and real_lcdc
	asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (0xC0000000));
	asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (real_lcdc));
	
	
	old_abort_handler = *(volatile uint32_t*)0x30;
	*(volatile uint32_t*)0x30 = (uint32_t) lcd_compat_abort_handler;
	
	
	noflicker = true;
	
	
	
	
	
}





void disableNoflicker()
{
	if (! noflicker)
		return;
	
	uint32_t *tt_base;
	asm volatile("mrc p15, 0, %[tt_base], c2, c0, 0" : [tt_base] "=r" (tt_base));
	// Map real lcdc at 0xC0000000
	tt_base[0xC00] = 0xC0000C12;
	// Flush TLB for 0xC0000000
	asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (0xC0000000));
	
	
	
	
	*(volatile uint32_t*)0x30 = old_abort_handler;
	
	noflicker = false;
}


































#endif
