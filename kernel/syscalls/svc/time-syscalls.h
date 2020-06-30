#ifndef TIME_SYSCALLS_H
#define TIME_SYSCALLS_H



void syscall_unix_time(volatile uint32_t* regs);
void syscall_unix_time_milli(volatile uint32_t* regs);
void syscall_set_unix_time(volatile uint32_t* regs);
void syscall_set_unix_time_milli(volatile uint32_t* regs);
void syscall_unix_time_micro(volatile uint32_t* regs);









#endif