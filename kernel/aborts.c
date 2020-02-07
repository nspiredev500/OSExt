#include "kernel.h"



// addresses of the exception vectors
// replace with function pointer to handler to use
static volatile uint32_t *undef_addrs = (volatile uint32_t*) 0x24;
static volatile uint32_t *prefetch_addrs = (volatile uint32_t*) 0x2c; // and bkpt
static volatile uint32_t *data_addrs = (volatile uint32_t*) 0x30;
















void install_exception_handlers()
{
	
	
	
	
	
}




































