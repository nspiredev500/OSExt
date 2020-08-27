#ifndef FIQ_H
#define FIQ_H


void fiq_handler(uint32_t spsr,void* address,volatile uint32_t *regs);





#endif