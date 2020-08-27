#ifndef SYSCALLS_H
#define SYSCALLS_H

#define SYSCALL_MASK 0xf80000

void init_syscall_table();

bool syscall_in_progress();
uint32_t nested_syscalls();

void syscall_set_reg(volatile uint32_t *regs,uint32_t reg,uint32_t value);
uint32_t syscall_get_reg(volatile uint32_t *regs,uint32_t reg);



// marks a syscall as finished by decrementing __syscall_in_progress
// should only be used when the syscall uses another way to return than the syscall handler, to net mess up the syscall count
void syscall_mark_as_finished();































#endif