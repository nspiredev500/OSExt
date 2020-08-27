#include "../../kernel.h"



void syscall_enable_kernel_thread_scheduling(volatile uint32_t* regs)
{
	scheduler_enable_kernel_thread_scheduling();
}

void syscall_disable_kernel_thread_scheduling(volatile uint32_t* regs)
{
	scheduler_disable_kernel_thread_scheduling();
}


void syscall_kernel_thread_wait_millis(volatile uint32_t* regs)
{
	//DEBUGPRINTLN_1("syscall count: %d",nested_syscalls());
	if (syscall_get_reg(regs,0) == 0)
	{
		return;
	}
	scheduler_kernel_thread_wait(scheduler_get_running_kernel_thread(),syscall_get_reg(regs,0),regs);
}



void syscall_kernel_thread_wait_for(volatile uint32_t* regs)
{
	if (syscall_get_reg(regs,0) == 0)
	{
		syscall_set_reg(regs,0,0);
		return;
	}
	scheduler_kernel_thread_wait_thread(scheduler_get_running_kernel_thread(),(struct svc_thread*)syscall_get_reg(regs,0),regs);
}



void syscall_kernel_thread_create(volatile uint32_t* regs)
{
	if (syscall_get_reg(regs,0) == 0 || syscall_get_reg(regs,1) == 0 || syscall_get_reg(regs,2) == 0)
	{
		syscall_set_reg(regs,0,0);
		return;
	}
	struct svc_thread* t = create_svc_thread((void*) syscall_get_reg(regs,0),syscall_get_reg(regs,1),(void*) syscall_get_reg(regs,2));
	scheduler_add_kernel_thread(t);
	syscall_set_reg(regs,0,(uint32_t) t);
}


void syscall_kernel_thread_delete(volatile uint32_t* regs)
{
	if (syscall_get_reg(regs,0) == 0)
	{
		syscall_set_reg(regs,0,0);
		return;
	}
	scheduler_remove_kernel_thread((struct svc_thread*)syscall_get_reg(regs,0));
}















