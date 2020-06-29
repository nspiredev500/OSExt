#include "../../kernel.h"





void syscall_unix_time(uint32_t* regs)
{
	int64_t time = systime_set_unix();
	syscall_set_reg(regs,0,(uint32_t) time);
	syscall_set_reg(regs,1,(uint32_t)(time >> 32));
}



void syscall_unix_time_milli(uint32_t* regs)
{
	int64_t time = systime_set_unix_milli();
	syscall_set_reg(regs,0,(uint32_t) time);
	syscall_set_reg(regs,1,(uint32_t)(time >> 32));
}



void syscall_set_unix_time(uint32_t* regs)
{
	uint32_t bottom,top;
	bottom = syscall_get_reg(regs,0);
	top = syscall_get_reg(regs,1);
	int64_t time = (int64_t) ((((uint64_t)top) << 32) | ((uint64_t) bottom));
	systime_set_unix(time);
}



void syscall_set_unix_time_milli(uint32_t* regs)
{
	uint32_t bottom,top;
	bottom = syscall_get_reg(regs,0);
	top = syscall_get_reg(regs,1);
	int64_t time = (int64_t) ((((uint64_t)top) << 32) | ((uint64_t) bottom));
	systime_set_unix_milli(time);
}



void syscall_unix_time_micro(uint32_t* regs)
{
	int64_t time = systime_set_unix_micro();
	syscall_set_reg(regs,0,(uint32_t) time);
	syscall_set_reg(regs,1,(uint32_t)(time >> 32));
}













