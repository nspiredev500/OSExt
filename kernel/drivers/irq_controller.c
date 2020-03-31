#include "../kernel.h"

static volatile uint32_t *remapped_vic = (volatile uint32_t*) (0xe9300000);

/*
	remapped_vic[3] = interrupt select
	remapped_vic[3] = interrupt enable
	
*/


void vic_enable(uint32_t irq)
{
	uint32_t enable = remapped_vic[4];
	enable |= 0b1 << irq;
	remapped_vic[4] = enable;
}


void vic_disable(uint32_t irq)
{
	uint32_t enable = remapped_vic[4];
	enable &= ~(0b1 << irq);
	remapped_vic[4] = enable;
}


void vic_disable_all()
{
	remapped_vic[4] = 0;
}


void vic_enable_all()
{
	remapped_vic[4] = 0xffffffff;
}


void vic_set_fiq(uint32_t irq)
{
	uint32_t select = remapped_vic[3];
	select |= 0b1 << irq;
	remapped_vic[3] = select;
}


void vic_set_irq(uint32_t irq)
{
	uint32_t select = remapped_vic[3];
	select &= ~(0b1 << irq);
	remapped_vic[3] = select;
}







