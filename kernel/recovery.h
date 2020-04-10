#ifndef RECOVERY_H
#define RECOVERY_H





void recovery_init();




bool recovery_mode(int argsn,char **argc);


void __attribute__ ((noreturn)) recovery_enter(uint32_t cause,uint32_t *regs);


void __attribute__ ((noreturn)) recovery_shell(uint32_t cause,uint32_t *regs);










#endif