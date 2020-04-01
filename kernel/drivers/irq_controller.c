#include "../kernel.h"

static volatile uint32_t *remapped_vic = (volatile uint32_t*) (0xe9300000);

/*
	remapped_vic[0] = irq status register
	remapped_vic[1] = fiq status register
	remapped_vic[2] = raw interrupt status register
	remapped_vic[3] = interrupt select register
	remapped_vic[4] = interrupt enable register
	remapped_vic[5] = interrupt enable clear register
*/


uint32_t vic_irq_status()
{
	return remapped_vic[0];
}

uint32_t vic_fiq_status()
{
	return remapped_vic[1];
}

void vic_enable(uint32_t irq)
{
	uint32_t enable = remapped_vic[4];
	enable |= 0b1 << irq;
	remapped_vic[4] = enable;
}


void vic_disable(uint32_t irq)
{
	remapped_vic[5] = 0b1 << irq;
}


void vic_disable_all()
{
	remapped_vic[5] = 0xffffffff;
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







