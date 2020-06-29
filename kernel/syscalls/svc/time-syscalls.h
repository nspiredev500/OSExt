#ifndef TIME_SYSCALLS_H
#define TIME_SYSCALLS_H



void syscall_unix_time(uint32_t* regs);
void syscall_unix_time_milli(uint32_t* regs);
void syscall_set_unix_time(uint32_t* regs);
void syscall_set_unix_time_milli(uint32_t* regs);
void syscall_unix_time_micro(uint32_t* regs);









#endif