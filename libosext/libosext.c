#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include "libosext.h"

#define SYSCALL_MASK 0xf80000



#define VERSION_MAYOR(ver) (ver& 0xff)




int osext_errno = 0;

static bool osext_checked = false;
static bool osext_installed_bool = false;
static uint32_t osext_version_num = 0;

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


uint32_t osext_version()
{
	if (! osext_installed())
	{
		osext_errno = -1;
		return 0;
	}
	if (osext_version_num == 0)
	{
		register uint32_t ver asm("r0") = 0;
		asm volatile(
		"swi %[nr]"
		: "=r" (ver)
		: [nr] "i" (SYSCALL_MASK + 1)
		: "memory", "r1", "r2", "r3", "r4", "r12", "lr");
		osext_version_num = ver;
	}
	return osext_version_num;
}

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
		"swi %[nr]"
		: "+r" (bottom), "+r" (top)
		: [nr] "i" (SYSCALL_MASK + 2), "r" (bottom), "r" (top)
		: "memory", "r2", "r3", "r4", "r12", "lr");
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
		"swi %[nr]"
		: "+r" (bottom), "+r" (top)
		: [nr] "i" (SYSCALL_MASK + 3), "r" (bottom), "r" (top)
		: "memory", "r2", "r3", "r4", "r12", "lr");
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
		"swi %[nr]"
		: "+r" (bottom), "+r" (top)
		: [nr] "i" (SYSCALL_MASK + 4), "r" (bottom), "r" (top)
		: "memory", "r2", "r3", "r4", "r12", "lr");
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
		"swi %[nr]"
		: 
		: [nr] "i" (SYSCALL_MASK + 5), "r" (bottom), "r" (top)
		: "memory", "r2", "r3", "r4", "r12", "lr");
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
		"swi %[nr]"
		: 
		: [nr] "i" (SYSCALL_MASK + 6), "r" (bottom), "r" (top)
		: "memory", "r2", "r3", "r4", "r12", "lr");
	osext_errno = 0;
}



// this has to be called before any of the thread functions work
void osext_init_threads()
{
	if (! osext_installed() || VERSION_MAYOR(osext_version()) < 5)
	{
		osext_errno = -1;
		return;
	}
	asm volatile("swi %[nr]":: [nr] "i" (SYSCALL_MASK + 7) : "memory","r0" , "r1", "r2", "r3", "r4", "r12", "lr");
}


// this has to be called before your program exits, if you called osext_init_threads
// it can only be called from your main thread
// it immediately stops all other threads
void osext_exit_threads()
{
	if (! osext_installed() || VERSION_MAYOR(osext_version()) < 5)
	{
		osext_errno = -1;
		return;
	}
	asm volatile("swi %[nr]":: [nr] "i" (SYSCALL_MASK + 8) : "memory","r0" , "r1", "r2", "r3", "r4", "r12", "lr");
}



// creates a thread with the specified stack, stack size and entry point.
// the entry point cannot get arguments
// the specified stack region has to be freed by the program
osext_thread* osext_create_thread(void* stack,uint32_t stacksize,void (*entry)(void))
{
	if (! osext_installed() || VERSION_MAYOR(osext_version()) < 5)
	{
		osext_errno = -1;
		return NULL;
	}
	register void* t asm("r0") = stack;
	register uint32_t stsize asm("r1") = stacksize;
	register void (*ent)(void) asm("r2") = entry;
	asm volatile("swi %[nr]":"+r" (t): [nr] "i" (SYSCALL_MASK + 11),"r" (t),"r" (stsize),"r" (ent): "memory" , "r3", "r4", "r12", "lr");
	if (t == NULL)
	{
		osext_errno = -1;
		return NULL;
	}
	else
	{
		return t;
	}
}


// destroys the thread, waking up any other threads waiting for it.
// the original thread cannot be destroyed
// the stack still has to be freed by the program
void osext_destroy_thread(osext_thread* thread)
{
	if (! osext_installed() || VERSION_MAYOR(osext_version()) < 5)
	{
		osext_errno = -1;
		return;
	}
	register osext_thread* t asm("r0") = thread;
	asm volatile("swi %[nr]":"+r" (t): [nr] "i" (SYSCALL_MASK + 12),"r" (t): "memory" , "r1", "r2", "r3", "r4", "r12", "lr");
}




// lets the current thread sleep for the specified amount of milliseconds.
// the accuracy is dependent on the timeslice length, and gets rounded down to the nearest timeslice (currently 2 milliseconds)
void osext_thread_wait_milli(uint32_t milli)
{
	if (! osext_installed() || VERSION_MAYOR(osext_version()) < 5)
	{
		osext_errno = -1;
		return;
	}
	register uint32_t mil asm("r0") = milli;
	asm volatile("swi %[nr]":"+r" (mil): [nr] "i" (SYSCALL_MASK + 9),"r" (mil): "memory" , "r1", "r2", "r3", "r4", "r12", "lr");
}

// lets the current thread sleep until the specified thread has finished
void osext_thread_wait_for_thread(osext_thread* thread)
{
	if (! osext_installed() || VERSION_MAYOR(osext_version()) < 5)
	{
		osext_errno = -1;
		return;
	}
	register osext_thread* t asm("r0") = thread;
	asm volatile("swi %[nr]":"+r" (t): [nr] "i" (SYSCALL_MASK + 10),"r" (t): "memory" , "r1", "r2", "r3", "r4", "r12", "lr");
}

























