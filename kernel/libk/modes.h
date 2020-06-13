#ifndef PROCESSOR_MODE_SWITCHES
#define PROCESSOR_MODE_SWITCHES


struct irq_state {
	bool irq;
	bool fiq;
};


void irq_disable(); // disables fiq and irq
void irq_save_state(struct irq_state *s); // saves irq and fiq state
void irq_restore_state(struct irq_state *s); // restores irq and fiq state


void init_call_with_stack(void* start);

// WARNING: NOT REENTRANT!
// function does not support arguments
uint32_t call_with_stack(const void* stack,void* function);

bool isIRQ();
bool isFIQ();
void disableIRQ();
void enableIRQ();
void disableFIQ();
void enableFIQ();	














#endif