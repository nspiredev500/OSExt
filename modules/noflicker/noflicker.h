#ifndef NOFLICKER_H
#define NOFLICKER_H

// will remap the lcd so the os can't write to it anymore
// also no other programm can write to it if it doesn't know the location, so we have to expose it in system calls
// there will be system calls to get the main gc and to force a blit to the screen
// there has to be an option to disable it, because it breaks programms that don't know about it


uint32_t *real_lcdc = (uint32_t*) 0xE0000000;













void lcd_compat_abort(uint32_t *regs)
{
    // Get the address that was tried to access
    uintptr_t fault_addr;
    asm volatile("mrc p15, 0, %[fault_addr], c6, c0, 0" : [fault_addr] "=r" (fault_addr));

    // Tried to access LCD controller?
    if(fault_addr >> 28 != 0xC)
        asm volatile("udf #0"); // Crash!
    
    
}



extern lcd_compat_abort_handler
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
	old_abort_handler = *(volatile uint32_t*)0x30;
	*(volatile uint32_t*)0x30 = (uint32_t) lcd_compat_abort_handler;
	
	
	
	
	
	// Get address of translation table
	uint32_t *tt_base;
	asm volatile("mrc p15, 0, %[tt_base], c2, c0, 0" : [tt_base] "=r" (tt_base));
	// Map LCDC from 0xC0000000 to real_lcdc
	tt_base[(uint32_t) real_lcdc >> 20] = tt_base[0xC00];
	tt_base[0xC00] = 0;
	// Flush TLB for 0xC0000000 and real_lcdc
	asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (0xC0000000));
	asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (real_lcdc));
	
	
	
	
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
