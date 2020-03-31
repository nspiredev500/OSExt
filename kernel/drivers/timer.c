#include "../kernel.h"


static volatile uint32_t *remapped_fast_timer = (volatile uint32_t*) 0xe9010000;
static volatile uint32_t *remapped_first_timer = (volatile uint32_t*) 0xe90c0000;
static volatile uint32_t *remapped_second_timer = (volatile uint32_t*) 0xe90d0000;

static volatile uint32_t *remapped_misc = (uint32_t*) 0xe90a0000;

// timers run at 32khz, fast timer runs at 22.5Mhz

/*
	interrupt mask for the timers is in misc
	remapped_misc[4] = fast timer interrupt status/acknowledge
	remapped_misc[5] = fast timer interrupt mask
	remapped_misc[6] = timer 1 interrupt status/acknowledge
	remapped_misc[7] = timer 1 interrupt mask
	remapped_misc[8] = timer 2 interrupt status/acknowledge
	remapped_misc[9] = timer 2 interrupt mask
	
	
	
	remapped_timer[0] = timer1 load register
	remapped_timer[1] = timer1 value
	remapped_timer[2] = timer1 control
	remapped_timer[6] = timer1 background load
	remapped_timer[8] = timer2 load register
	remapped_timer[9] = timer2 value
	remapped_timer[10] = timer2 control
	remapped_timer[14] = timer2 background load
*/


/*
	Timer IDs
	fast timer = 0
	first timer = 1
	second timer = 2
*/



void timer_enable(uint32_t timer)
{
	if (timer > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timer == 1)
		remapped_timer = remapped_first_timer;
	if (timer == 2)
		remapped_timer = remapped_second_timer;
	
	vic_set_fiq(17+timer);
	vic_enable(17+timer);
	remapped_misc[5+timer*2] |= 0b11;
	
	
	
	
	
}

void timer_disable(uint32_t timer)
{
	if (timer > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timer == 1)
		remapped_timer = remapped_first_timer;
	if (timer == 2)
		remapped_timer = remapped_second_timer;
	
	vic_disable(17+timer);
	vic_set_irq(17+timer);
	remapped_misc[5+timer*2] &= ~0b1;
	
	
	
	
}

uint32_t timer_value(uint32_t timer)
{
	if (timer > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timer == 1)
		remapped_timer = remapped_first_timer;
	if (timer == 2)
		remapped_timer = remapped_second_timer;
	
	
	
	
}
void timer_set_load(uint32_t timer,uint32_t load)
{
	if (timer > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timer == 1)
		remapped_timer = remapped_first_timer;
	if (timer == 2)
		remapped_timer = remapped_second_timer;
	
	
	
	
}

void timer_set_prescaler(uint32_t timer,uint8_t prescale)
{
	if (timer > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timer == 1)
		remapped_timer = remapped_first_timer;
	if (timer == 2)
		remapped_timer = remapped_second_timer;
	
	
	
	
}
void timer_set_mode(uint32_t timer,uint8_t mode)
{
	if (timer > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timer == 1)
		remapped_timer = remapped_first_timer;
	if (timer == 2)
		remapped_timer = remapped_second_timer;
	
	
	
	
}


bool timer_irq_status(uint32_t timer)
{
	if (timer > 2)
		return;
	
	
	
	
	
}
void timer_irq_clear(uint32_t timer)
{
	if (timer > 2)
		return;
	
	
	
	
	
}



bool timer_irq_enabled(uint32_t timer)
{
	if (timer > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timer == 1)
		remapped_timer = remapped_first_timer;
	if (timer == 2)
		remapped_timer = remapped_second_timer;
	
	
	
	
}
void timer_set_irq_enabled(uint32_t timer)
{
	if (timer > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timer == 1)
		remapped_timer = remapped_first_timer;
	if (timer == 2)
		remapped_timer = remapped_second_timer;
	
	
	
	
}































