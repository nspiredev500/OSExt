#ifndef TIMER_H
#define TIMER_H



/*
	Timer IDs
	first timer = 0
	second timer = 1
	fast timer = 2
*/

struct timer_state {
	uint32_t load;
	uint32_t control;
	uint32_t bgload;
};

void timer_save_state(uint32_t timermodule,uint32_t timer,struct timer_state *state);
void timer_resume_state(uint32_t timermodule,uint32_t timer,struct timer_state *state);

void timer_return_os(uint32_t timermodule,uint32_t timer,struct timer_state *state);


void timer_enable(uint32_t timermodule,uint32_t timer);
void timer_disable(uint32_t timermodule,uint32_t timer);

uint32_t timer_value(uint32_t timermodule,uint32_t timer);
void timer_set_load(uint32_t timermodule,uint32_t timer,uint32_t load);


void timer_set_prescaler(uint32_t timermodule,uint32_t timer,uint8_t prescale);
void timer_set_mode(uint32_t timermodule,uint32_t timer,uint8_t mode);


bool timer_irq_status(uint32_t timermodule,uint32_t timer);
void timer_irq_clear(uint32_t timermodule,uint32_t timer);



bool timer_irq_enabled(uint32_t timermodule,uint32_t timer);
void timer_set_irq_enabled(uint32_t timermodule,uint32_t timer,bool irq);

void timer_set_oneshot(uint32_t timermodule,uint32_t timer,bool oneshot);
bool timer_is_oneshot(uint32_t timermodule,uint32_t timer);



#endif