#include "kernel.h"



// addresses of the exception vectors
// replace with function pointer to handler to use
static volatile uint32_t *undef_addrs = (volatile uint32_t*) 0x24;
static volatile uint32_t *prefetch_addrs = (volatile uint32_t*) 0x2c; // and bkpt
static volatile uint32_t *data_addrs = (volatile uint32_t*) 0x30;







static uint32_t undef_stack[256];
static uint32_t prefetch_stack[256];
static uint32_t data_stack[256];




static void *orig_undef = NULL;
static void *orig_prefetch = NULL;
static void *orig_data = NULL;



void install_exception_handlers()
{
	orig_undef = (void*) *undef_addrs;
	orig_prefetch = (void*) *prefetch_addrs;
	orig_data = (void*) *data_addrs;
	
	
}




































