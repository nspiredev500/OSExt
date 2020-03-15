#ifndef PROCESSOR_MODE_SWITCHES
#define PROCESSOR_MODE_SWITCHES




void init_call_with_stack(void* start);

// WARNING: NOT REENTRANT!
// function does not support arguments
uint32_t call_with_stack(void* stack,void* function);


void disableIRQ();
void enableIRQ();
void disableFIQ();
void enableFIQ();	














#endif