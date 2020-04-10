#include "../kernel.h"




void unlockMutex(uint32_t *mutex)
{
	volatile uint32_t *mut = (volatile uint32_t*) mutex;
	*mut = 0; // unlocking doesn't have to be an atomic operation, something other than zero will be interpreted as a lock
}

// returns false if the mutex is locked and true if the mutex wasn't locked and was acquired
bool tryMutex(uint32_t *mutex)
{
	register uint32_t value asm("r0") = 1;
	register uint32_t *address asm("r1") = mutex;
	asm volatile("swp r0, r0, [r1]":"=r" (value):"r" (address):"memory"); // TODO maybe constrain it to the value of the mutex instead of memory in general
	if (value == 0)
	{
		return true;
	}
	return false;
}










