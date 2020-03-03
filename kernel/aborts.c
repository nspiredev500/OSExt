#include "kernel.h"



// addresses of the exception vectors
// replace with function pointer to handler to use
static volatile void **undef_addrs = (volatile void**) 0x24;
static volatile void **prefetch_addrs = (volatile void**) 0x2c; // and bkpt
static volatile void **data_addrs = (volatile void**) 0x30;


bool probe_address = false;

uint32_t undef_stack[256];
uint32_t abort_stack[256];


static void *orig_abort_stack = NULL;
static void *orig_undef_stack = NULL;

static void *orig_undef = NULL;
static void *orig_prefetch = NULL;
static void *orig_data = NULL;







void install_exception_handlers()
{
	orig_undef = (void*) *undef_addrs;
	orig_prefetch = (void*) *prefetch_addrs;
	orig_data = (void*) *data_addrs;
	//asm(".long 0xE1212374"); // bkpt
	register void* orig_stack asm("r0") = NULL;
	register void* new_stack asm("r3") = abort_stack+sizeof(abort_stack)/4-4;
	asm volatile(
	"mrs r1, cpsr \n"
	"mov r2, r1 \n"
	"bic r1, r1, #31 \n" // clear the mode bits
	"orr r1, r1, #23 \n" // set the mode to abort
	"msr cpsr, r1\n"
	"mov r0, sp \n"
	"mov sp, r3 \n"
	"msr cpsr, r2 \n":"=r" (orig_stack):"r" (new_stack):"r1", "r2");
	orig_abort_stack = orig_stack;
	new_stack = undef_stack+sizeof(undef_stack)/4-4;
	
	asm volatile(
	"mrs r1, cpsr \n"
	"mov r2, r1 \n"
	"bic r1, r1, #31 \n" // clear the mode bits
	"orr r1, r1, #27 \n" // set the mode to undef
	"msr cpsr, r1\n"
	"mov r0, sp \n"
	"mov sp, r3 \n"
	"msr cpsr, r2 \n":"=r" (orig_stack):"r" (new_stack):"r1", "r2");
	orig_undef_stack = orig_stack;
	extern void prefetch_wrapper();
	extern void undef_wrapper();
	extern void data_wrapper();
	
	
	/*
	
	*undef_addrs = undef_wrapper;
	*data_addrs = data_wrapper;
	*prefetch_addrs = prefetch_wrapper;
	*/
	
	
	
	
	set_exception_vectors(true);
	
	
}






void set_exception_vectors(bool high)
{
	register uint32_t control_reg asm("r0");
	asm volatile("mrc p15, 0, r0, c1, c0, 0":"=r" (control_reg)::);
	if (high)
	{
		control_reg |= 1 << 13;
	}
	else
	{
		control_reg &= ~ (1 << 13);
	}
	asm volatile("mcr p15, 0, r0, c1, c0, 0"::"r" (control_reg):);
}




























