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
		//uart_printf("test3: 0x%x\n",test3);
		relocate_self();
	}
	return 0;
}



// because we return with main after this, every error here is still recoverable without a kernel panic
void initialize()
{
	void* framebuffer = (void*) *LCD_UPBASE;
	framebuffer_fillrect(framebuffer,0,0,320,240,0,0,0);
	
	debug_shell_println("finished relocating");
	debug_shell_println("initializing");
	
	debug_shell_println("kernel_start: 0x%x",&_EXEC_START);
	init_call_with_stack(&_EXEC_START);
	
	
	debug_shell_println("performing physical memory manager self-test");
	bool b = physical_mm_self_test();
	if (! b)
	{
		debug_shell_println_rgb("error in physical memory manager self-test         aborting",255,0,0);
		keypad_press_release_barrier();
		free_init_pds();
		return;
	}
	
	debug_shell_println("allocating memory");
	allocPageblock(128);
	allocPageblock(128);
	allocPageblock(128);
	allocPageblock(128); // allocate 2mb
	debug_shell_println("done");
	
	
	/*
	register uint32_t domains asm("r0") = 0;
	asm volatile("mrc p15, 0, r0, c3, c0, 0":"=r" (domains));
	DEBUGPRINTF_3("domains: 0x%x\n",domains); // domain 0 is client, so we can use it for everything, because access permissions are checked
	*/
	
	debug_shell_println("performing virtual memory manager self-test");
	b = virtual_mm_self_test();
	if (! b)
	{
		debug_shell_println_rgb("error in virtual memory manager self-test         aborting",255,0,0);
		keypad_press_release_barrier();
		free_init_pds();
		return;
	}
	debug_shell_println("performing slab allocator self-test");
	b = slab_allocator_self_test();
	if (! b)
	{
		debug_shell_println_rgb("error in slab allocator self-test         aborting",255,0,0);
		keypad_press_release_barrier();
		free_init_pds();
		return;
	}
	
	debug_shell_println("initializing kernel space");
	initializeKernelSpace();
	free_init_pds();
	debug_shell_println("done");
	//asm(".long 0xE1212374"); // bkpt
	
	void* framebuffer_start = (void*) ((uint32_t)framebuffer & 0xfff);
	for (int i = 0;i<37;i++)
	{
		addVirtualKernelPage(framebuffer_start+SMALL_PAGE_SIZE*i,framebuffer_start+SMALL_PAGE_SIZE*i);
	}
	
	
	
	debug_shell_println("running general self-test");
	
	//TODO try to call it with the new stack, the old one gets cut away in the context switch
	
	
	void* page = usePage();
	if (page == NULL)
	{
		debug_shell_println_rgb("no page for general self test         aborting",255,0,0);
		keypad_press_release_barrier();
		return;
	}
	addVirtualKernelPage(page,(void*) 0xe8000000);
	
	b = (bool) call_with_stack((void*)(0xe8000000+SMALL_PAGE_SIZE-8),run_self_test);
	
	setPageUsedBit(page,false);
	
	
	//b = true;
	if (! b)
	{
		debug_shell_println_rgb("error in general self-test         aborting",255,0,0);
		keypad_press_release_barrier();
		return;
	}
	
	
	
	
	
	
	
	
	
	
	
	debug_shell_println_rgb("osext installed",0,255,0);
	debug_shell_println_rgb("press any key to exit",0,255,0);
	// to be able to read the messages
	keypad_press_release_barrier();
}




























