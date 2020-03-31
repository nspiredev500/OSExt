#ifndef IRQ_CONTROLLER_H
#define IRQ_CONTROLLER_H







void vic_enable(uint32_t irq);
void vic_disable(uint32_t irq);
void vic_disable_all();
void vic_enable_all();
void vic_set_fiq(uint32_t irq);
void vic_set_irq(uint32_t irq);














#endif