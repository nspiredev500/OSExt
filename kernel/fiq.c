#include "fiq.h"
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>

static volatile uint32_t *fiq_address = (volatile uint32_t*) 0x30;
//static const int MAXFIQ = 30;
#define MAXFIQ 30
static int fiqindex = 0;
void (*fiqtable[MAXFIQ])(void);

int addFIQFunction(void (*function)())
{
	if (fiqindex >= MAXFIQ-1)
	{
		return -1;
	}
	uint32_t cpsr asm("r0");
	asm volatile("mrs r0, cpsr");
	cpsr = cpsr & (~ (1 << 6)); // clear the fiq bit
	asm volatile("msr cpsr, r0");
	fiqtable[fiqindex] = function;
	fiqindex++;
	cpsr = cpsr | (1 << 6); // set the fiq bit
	asm volatile("msr cpsr, r0");
	return 0;
}


void installFIQHandler()
{
	// TODO make the fiq handler
	//*fiq_address = fiqhandler;
}












