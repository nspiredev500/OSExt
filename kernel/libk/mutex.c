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



// ld complaints about undefined reference to __sync_synchronize
/*
void unlockMutex(uint32_t *mutex)
{
	__atomic_clear(mutex,__ATOMIC_SEQ_CST);
}



// returns false if the mutex is locked and true if the mutex wasn't locked and was acquired
bool tryMutex(uint32_t *mutex)
{
	bool b = __atomic_test_and_set(mutex,__ATOMIC_SEQ_CST);
	if (b)
		return false;
	else
		return true;
}
*/


// apparentrly the arm9ej-s doesn't have ldrex and strex, but only swp
/*
// returns false if the mutex is locked and true if the mutex wasn't locked and was acquired
bool tryMutex(uint32_t *mutex)
{
	register uint32_t *mut asm("r0") = mutex;
	register uint32_t mutex_val asm("r2"); // will hold the value of the mutex
	asm volatile("ldrex r2, [r0]":"=r" (mutex_val):"r" (mut));
	if (mutex_val == 0)
	{
		mutex_val = 1;
		asm volatile("retry: strex r3, r2, [r0]\n"
		"cmp r3, #1\n"
		"beq retry"::"r" (mut),"r" (mutex_val):"r3","cc","memory");
		return true;
	}
	else
	{
		return false;
	}
}
*/













