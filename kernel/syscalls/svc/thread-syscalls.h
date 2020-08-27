#ifndef THREAD_SYSCALLS_H
#define THREAD_SYSCALLS_H

void syscall_enable_kernel_thread_scheduling(volatile uint32_t* regs);
void syscall_disable_kernel_thread_scheduling(volatile uint32_t* regs);


void syscall_kernel_thread_wait_millis(volatile uint32_t* regs);
void syscall_kernel_thread_wait_for(volatile uint32_t* regs);

void syscall_kernel_thread_create(volatile uint32_t* regs);
void syscall_kernel_thread_delete(volatile uint32_t* regs);


#endif