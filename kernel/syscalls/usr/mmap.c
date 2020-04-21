#include "../../kernel.h"
#include "../../include/sys/mman.h"


void mmap(uint32_t *regs)
{
	
	
	
	
	syscall_set_reg(regs,0,MAP_FAILED);
}








