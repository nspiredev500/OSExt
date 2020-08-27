#ifndef PROCESSOR_MODE_SWITCHES
#define PROCESSOR_MODE_SWITCHES

#define MODE_SVC 0b10011
#define MODE_IRQ 0b10010
#define MODE_FIQ 0b10001
#define MODE_ABT 0b10111
#define MODE_UDF 0b11011
#define MODE_USR 0b10000
#define MODE_SYS 0b11111

#define PSR_MODE(spr) (spr & 0b11111)

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