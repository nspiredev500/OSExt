#include "../../kernel.h"




void syscall_version(volatile uint32_t* regs)
{
	syscall_set_reg(regs,0,OSEXT_VERSION);
}







