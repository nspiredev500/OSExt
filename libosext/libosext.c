#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>



int osext_errno = 0;

static bool osext_checked = false;
static bool osext_installed_bool = false;

static void check_osext()
{
	register uint32_t control_reg asm("r0");
	asm volatile("mrc p15, 0, r0, c1, c0, 0":"=r" (control_reg)::); // reading co-processor register 1
	if ((control_reg & (1 << 13)) == (1 << 13)) // if bit 13 is a 1, interrupt vector are mapped high. I don't think anything other than OSExt does this
	{
		register uint32_t tt_base asm("r0");
		asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base)); // get the translation table base register
		
		tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
		uint32_t *tt = (uint32_t*) tt_base;
		if (tt[(0xe0100000) >> 20] != 0) // if there is something mapped at virtual address 0xe0000000 it should be OSExt, I haven't seen any other program map memory like that
		{
			osext_checked = true;
			osext_installed_bool = true;
		}
	}
	osext_checked = true;
}



bool osext_installed()
{
	if (! osext_checked)
	{
		check_osext();
	}
	osext_errno = 0;
	return osext_installed_bool;
}



#define SYSCALL_MASK 0xf80000

// returns the current unix time (as set by the user), in seconds
int64_t osext_unix_time()
{
	if (! osext_installed())
	{
		osext_errno = -1;
		return -1;
	}
	register uint32_t bottom asm("r0") = 0;
	register uint32_t top asm("r1") = 0;
	asm volatile(
		"swi %[nr]\n"
		: "+r" (bottom), "+r" (top)
		: [nr] "i" (SYSCALL_MASK + 2), "r" (bottom), "r" (top)
		: "memory", "r3", "r4", "r12", "lr");
	int64_t time = (int64_t) ((((uint64_t) top) << 32) | ((uint64_t) bottom));
	osext_errno = 0;
	return time;
}


// returns the current unix time (as set by the user), in milliseconds
int64_t osext_unix_time_milli()
{
	if (! osext_installed())
	{
		osext_errno = -1;
		return -1;
	}
	register uint32_t bottom asm("r0") = 0;
	register uint32_t top asm("r1") = 0;
	asm volatile(
		"swi %[nr]\n"
		: "+r" (bottom), "+r" (top)
		: [nr] "i" (SYSCALL_MASK + 3), "r" (bottom), "r" (top)
		: "memory", "r3", "r4", "r12", "lr");
	int64_t time = (int64_t) ((((uint64_t) top) << 32) | ((uint64_t) bottom));
	osext_errno = 0;
	return time;
}



// returns the current unix time (as set by the user), in microseconds
int64_t osext_unix_time_micro()
{
	if (! osext_installed())
	{
		osext_errno = -1;
		return -1;
	}
	register uint32_t bottom asm("r0") = 0;
	register uint32_t top asm("r1") = 0;
	asm volatile(
		"swi %[nr]\n"
		: "+r" (bottom), "+r" (top)
		: [nr] "i" (SYSCALL_MASK + 4), "r" (bottom), "r" (top)
		: "memory", "r3", "r4", "r12", "lr");
	int64_t time = (int64_t) ((((uint64_t) top) << 32) | ((uint64_t) bottom));
	osext_errno = 0;
	return time;
}


// sets the unix time from a value of seconds
void osext_set_unix_time(int64_t unix)
{
	if (! osext_installed())
	{
		osext_errno = -1;
		return;
	}
	register uint32_t bottom asm("r0") = (uint32_t) unix;
	register uint32_t top asm("r1") = (uint32_t) (unix >> 32);
	asm volatile(
		"swi %[nr]\n"
		: 
		: [nr] "i" (SYSCALL_MASK + 5), "r" (bottom), "r" (top)
		: "memory", "r3", "r4", "r12", "lr");
	osext_errno = 0;
}


// sets the unix time from a value of milliseconds
void osext_set_unix_time_milli(int64_t unix)
{
	if (! osext_installed())
	{
		osext_errno = -1;
		return;
	}
	register uint32_t bottom asm("r0") = (uint32_t) unix;
	register uint32_t top asm("r1") = (uint32_t) (unix >> 32);
	asm volatile(
		"swi %[nr]\n"
		: 
		: [nr] "i" (SYSCALL_MASK + 6), "r" (bottom), "r" (top)
		: "memory", "r3", "r4", "r12", "lr");
	osext_errno = 0;
}




























