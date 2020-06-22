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
	remapped_timer[3] = timer 1 interrupt clear
	remapped_timer[4] = timer 1 raw interrupt status
	remapped_timer[6] = timer1 background load
	remapped_timer[8] = timer2 load register
	remapped_timer[9] = timer2 value
	remapped_timer[10] = timer2 control
	remapped_timer[14] = timer2 background load
	remapped_timer[11] = timer 2 interrupt clear
	remapped_timer[12] = timer 2 raw interrupt status
*/


/*
	Timer IDs
	fast timer = 0
	first timer = 1
	second timer = 2
*/
/*
	first timer doesn't seem to be used by the OS, so we can use it for out system time
	
	
*/




void timer_save_state(uint32_t timermodule,uint32_t timer,struct timer_state *state)
{
	if (timer > 1)
		return;
	if (timermodule > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	state->load = remapped_timer[0+timer*8];
	state->control = remapped_timer[2+timer*8];
	state->bgload = remapped_timer[6+timer*8];
}

void timer_resume_state(uint32_t timermodule,uint32_t timer,struct timer_state *state)
{
	if (timer > 1)
		return;
	if (timermodule > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	remapped_timer[2+timer*8] &= ~(0b1 << 7); // disable the timer
	remapped_timer[0+timer*8] = state->load;
	remapped_timer[6+timer*8] = state->bgload;
	remapped_timer[2+timer*8] = (state->control & (~(0b1 << 7)));
	remapped_timer[2+timer*8] |= state->control & (0b1 << 7); // set the timer to the control state
}


void timer_return_os(uint32_t timermodule,uint32_t timer,struct timer_state *state)
{
	if (timer > 1)
		return;
	if (timermodule > 2)
		return;
	
	vic_disable(17+timermodule);
	vic_set_irq(17+timermodule);
	
	timer_resume_state(timermodule,timer,state);
	
	vic_enable(17+timermodule);
}




void timer_enable(uint32_t timermodule,uint32_t timer)
{
	if (timer > 1)
		return;
	if (timermodule > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	vic_set_fiq(17+timermodule);
	vic_enable(17+timermodule);
	remapped_timer[2+timer*8] |= 0b10100010;
}

void timer_disable(uint32_t timermodule,uint32_t timer)
{
	if (timer > 1)
		return;
	if (timermodule > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	vic_disable(17+timermodule);
	vic_set_irq(17+timermodule);
	remapped_timer[2+timer*8] &= ~0b10100000;
}

uint32_t timer_value(uint32_t timermodule,uint32_t timer)
{
	if (timer > 1)
		return 0;
	if (timermodule > 2)
		return 0;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	return remapped_timer[1+timer*8];
}
void timer_set_load(uint32_t timermodule,uint32_t timer,uint32_t load)
{
	if (timer > 1)
		return;
	if (timermodule > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	remapped_timer[timer*8] = load;
}

void timer_set_bg_load(uint32_t timermodule,uint32_t timer,uint32_t bgload)
{
	if (timer > 1)
		return;
	if (timermodule > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	remapped_timer[timer*8+6] = bgload;
}


void timer_set_size(uint32_t timermodule,uint32_t timer,uint32_t size)
{
	if (timer > 1)
		return;
	if (timermodule > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	uint32_t enabled = (remapped_timer[2+timer*8] >> 7) & 0b1;
	if (enabled)
		remapped_timer[2+timer*8] &= ~0b10000000; // timer has to be disabled before changing this setting
	remapped_timer[2+timer*8] &= 0b1100;
	remapped_timer[2+timer*8] |= (size & 0b1) << 1;
	if (enabled)
		remapped_timer[2+timer*8] |= 0b10000000;
}


void timer_set_prescaler(uint32_t timermodule,uint32_t timer,uint8_t prescale)
{
	if (timer > 1)
		return;
	if (timermodule > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	uint32_t enabled = (remapped_timer[2+timer*8] >> 7) & 0b1;
	if (enabled)
		remapped_timer[2+timer*8] &= ~0b10000000; // timer has to be disabled before changing this setting
	remapped_timer[2+timer*8] &= 0b1100;
	remapped_timer[2+timer*8] |= (prescale & 0b11) << 2;
	if (enabled)
		remapped_timer[2+timer*8] |= 0b10000000;
}
void timer_set_mode(uint32_t timermodule,uint32_t timer,uint8_t mode)
{
	if (timer > 1)
		return;
	if (timermodule > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	uint32_t enabled = (remapped_timer[2+timer*8] >> 7) & 0b1;
	if (enabled)
		remapped_timer[2+timer*8] &= ~0b10000000; // timer has to be disabled before changing this setting
	remapped_timer[2+timer*8] &= ~(0b1 << 6);
	remapped_timer[2+timer*8] |= (mode & 0b1) << 6;
	if (enabled)
		remapped_timer[2+timer*8] |= 0b10000000;
}


bool timer_irq_status(uint32_t timermodule,uint32_t timer)
{
	if (timer > 1)
		return false;
	if (timermodule > 2)
		return false;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	/*
	if (remapped_misc[4+timermodule*2] != 0)
		return true;
	else
		return false;
	*/
	if ((remapped_timer[4+timer*8] & 0b1) == 0b1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
void timer_irq_clear(uint32_t timermodule,uint32_t timer)
{
	if (timer > 1)
		return;
	if (timermodule > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	remapped_timer[3+timer*8] = 0;
	//remapped_misc[4+timermodule*2] = 0xffffffff;
}



bool timer_irq_enabled(uint32_t timermodule,uint32_t timer)
{
	if (timer > 1)
		return false;
	if (timermodule > 2)
		return false;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	if (((remapped_timer[2+timer*8] >> 5) & 0b1) == 0b1)
		return true;
	else
		return false;
}
void timer_set_irq_enabled(uint32_t timermodule,uint32_t timer,bool irq)
{
	if (timer > 1)
		return;
	if (timermodule > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	if (irq)
	{
		remapped_timer[2+timer*8] |= 0b100000;
	}
	else
	{
		remapped_timer[2+timer*8] &= ~0b100000;
	}
}




void timer_set_oneshot(uint32_t timermodule,uint32_t timer,bool oneshot)
{
	if (timer > 1)
		return;
	if (timermodule > 2)
		return;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	uint32_t enabled = (remapped_timer[2+timer*8] >> 7) & 0b1;
	if (enabled)
		remapped_timer[2+timer*8] &= ~0b10000000; // timer has to be disabled before changing this setting
	
	remapped_timer[2+timer*8] &= ~ 0b1;
	if (oneshot)
	{
		remapped_timer[2+timer*8] |= 0b1;
	}
	
	if (enabled)
		remapped_timer[2+timer*8] |= 0b10000000;
}

bool timer_is_oneshot(uint32_t timermodule,uint32_t timer)
{
	if (timer > 1)
		return false;
	if (timermodule > 2)
		return false;
	volatile uint32_t *remapped_timer = remapped_fast_timer;
	if (timermodule == 1)
		remapped_timer = remapped_first_timer;
	if (timermodule == 2)
		remapped_timer = remapped_second_timer;
	
	power_enable_device(11);
	power_enable_device(12);
	power_enable_device(13);
	
	
	if ((remapped_timer[2+timer*8] & 0b1) == 0b1)
	{
		return true;
	}
	else
	{
		return false;
	}
}


























