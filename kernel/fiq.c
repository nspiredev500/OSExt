#include "kernel.h"



static volatile uint32_t *fiq_address = (volatile uint32_t*) 0x30;
//static const int MAXFIQ = 30;
#define MAXFIQ 30
static int fiqindex = 0;
void (*fiqtable[MAXFIQ])(void);




asm(
".global fiq_wrapper \n"
"fiq_wrapper: \n"
".long 0xE1212374 \n" // breakpoint
" \n"
" \n"
" \n"
" \n"
" \n"
"subs pc, lr, #4 \n");





int addFIQFunction(void (*function)())
{
	if (fiqindex >= MAXFIQ-1)
	{
		return -1;
	}
	register uint32_t scpsr asm("r0");
	asm volatile("mrs %[scpsr], cpsr":[scpsr] "=r" (scpsr)::);
	scpsr = scpsr & (~ (1 << 6)); // clear the fiq bit
	asm volatile("msr cpsr, %[scpsr]"::[scpsr] "r" (scpsr):);
	fiqtable[fiqindex] = function;
	fiqindex++;
	scpsr = scpsr | (1 << 6); // set the fiq bit
	asm volatile("msr cpsr, %[scpsr]"::[scpsr] "r" (scpsr):);
	return 0;
}

// TODO but the page with the fiq handler in the lockdown-tlb, by making an offset variable in the linker script













