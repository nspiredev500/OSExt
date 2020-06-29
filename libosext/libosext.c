#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>




#define SYSCALL_MASK 0xf80000

// returns the current unix time (as set by the user), in seconds
int64_t osext_unix_time()
{
	register uint32_t bottom asm("r0") = 0;
	register uint32_t top asm("r1") = 0;
	asm volatile(
		"swi %[nr]\n"
		: "+r" (bottom), "+r" (top)
		: [nr] "i" (SYSCALL_MASK + 2), "r" (bottom), "r" (top)
		: "memory", "r3", "r4", "r12", "lr");
	int64_t time = (int64_t) ((((uint64_t) top) << 32) | ((uint64_t) bottom));
	return time;
}


// returns the current unix time (as set by the user), in milliseconds
int64_t osext_unix_time_milli()
{
	register uint32_t bottom asm("r0") = 0;
	register uint32_t top asm("r1") = 0;
	asm volatile(
		"swi %[nr]\n"
		: "+r" (bottom), "+r" (top)
		: [nr] "i" (SYSCALL_MASK + 3), "r" (bottom), "r" (top)
		: "memory", "r3", "r4", "r12", "lr");
	int64_t time = (int64_t) ((((uint64_t) top) << 32) | ((uint64_t) bottom));
	return time;
}



// returns the current unix time (as set by the user), in microseconds
int64_t osext_unix_time_micro()
{
	register uint32_t bottom asm("r0") = 0;
	register uint32_t top asm("r1") = 0;
	asm volatile(
		"swi %[nr]\n"
		: "+r" (bottom), "+r" (top)
		: [nr] "i" (SYSCALL_MASK + 4), "r" (bottom), "r" (top)
		: "memory", "r3", "r4", "r12", "lr");
	int64_t time = (int64_t) ((((uint64_t) top) << 32) | ((uint64_t) bottom));
	return time;
}


// sets the unix time from a value of seconds
void osext_set_unix_time(int64_t unix)
{
	register uint32_t bottom asm("r0") = (uint32_t) unix;
	register uint32_t top asm("r1") = (uint32_t) (unix >> 32);
	asm volatile(
		"swi %[nr]\n"
		: 
		: [nr] "i" (SYSCALL_MASK + 5), "r" (bottom), "r" (top)
		: "memory", "r3", "r4", "r12", "lr");
}


// sets the unix time from a value of milliseconds
void osext_set_unix_time_milli(int64_t unix)
{
	register uint32_t bottom asm("r0") = (uint32_t) unix;
	register uint32_t top asm("r1") = (uint32_t) (unix >> 32);
	asm volatile(
		"swi %[nr]\n"
		: 
		: [nr] "i" (SYSCALL_MASK + 6), "r" (bottom), "r" (top)
		: "memory", "r3", "r4", "r12", "lr");
}




























