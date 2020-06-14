#ifndef SYSCALLS_H
#define SYSCALLS_H

#define SYSCALL_MASK 0xf80000

void init_syscall_table();

bool syscall_in_progress();


void syscall_set_reg(uint32_t *regs,uint32_t reg,uint32_t value);
uint32_t syscall_get_reg(uint32_t *regs,uint32_t reg);



































#endif