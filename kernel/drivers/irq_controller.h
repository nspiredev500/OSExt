#ifndef IRQ_CONTROLLER_H
#define IRQ_CONTROLLER_H




uint32_t vic_irq_status();
uint32_t vic_fiq_status();

void vic_enable(uint32_t irq);
void vic_disable(uint32_t irq);

void vic_disable_all();
void vic_enable_all();

void vic_set_fiq(uint32_t irq);
void vic_set_irq(uint32_t irq);

bool vic_is_fiq(uint32_t fiq);








#endif