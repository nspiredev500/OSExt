#include "kernel.h"






asm(
".extern (main)\n"
"__init_lr: .word 0 \n"
"__init_sp: .word 0 \n"
"__entry: .global __entry\n"
//" .long 0xE1212374\n" // bkpt
" str lr, __init_lr\n"
" str sp, __init_sp\n"
" b main"
);








// some variables to test relocation
static uint32_t test = 20;

static uint32_t *test2 = &test;

static uint32_t *test3 = NULL;
// stack grows downwards!
/*
void teststack(void* t)
{
	int h = 1;
	if (&h < t)
	{
		uart_printf("stack grows downwards!\n");
	}
	else
	{
		uart_printf("stack grows upwards!\n");
	}
}
*/
// because we return with main, every error here is still recoverable without a kernel panic
int main(int argsn,char **argc)
{
	// no need to make the kernel resident, allocated memory isn't freed by ndless, so we can just copy the kernel and it will stay
	
	//int t = 0;
	//teststack(&t);
	
	
	
	
	if (argsn == 1 && ((unsigned int) argc) == 0x53544c41) //STandaloneLAunch
	{
		DEBUGPRINTF_1("relocated\n")
		// relocation finished or already done by loader
		initialize();
	}
	else
	{
		//test3 = ti_malloc(8);
		//uart_printf("test3: %d\n",test3);
		relocate_self();
	}
	return 0;
}



// because we return with main after this, every error here is still recoverable without a kernel panic
void initialize()
{
	DEBUGPRINTF_1("initializing\n")
	//TODO initialize physical and virtual memory manager properly, put large page descriptors for kernel space in dma memory
	
	
	
	
	allocPageblock(128);
	allocPageblock(128); // allocate 1mb
	
	
	
	
	
	initializeKernelSpace();
	
	
	
	
	// TODO move the descriptors to a permanent location
	//free_init_pds();
	
	
	
	
	
}




























