#include "kernel.h"















static const uint32_t PAGESIZE = 1024*64;


void relocate_self(void)
{
	// TODO  relocate to a virtual address, by changing the translation table, flushing the tlb and the copying
	uart_send_string("relocating\n");
	uint32_t offset = ((uint32_t) main)-((uint32_t) _EXEC_START);
	uint32_t kernel_size = ((uint32_t) _EXEC_END)-((uint32_t) _EXEC_START);
	uint32_t malloced_chunk = (uint32_t) ti_malloc(kernel_size+PAGESIZE*3); // extra size to align the kernel on a (large) page boundrary
	if (malloced_chunk == NULL)
	{
		uart_send_string("Not enough memory!\n");
		return;
	}
	void *aligned = (void*) ((malloced_chunk & (~ 0xFFFF))+0x10000);
	
	
	
	
	k_memcpy(aligned,_EXEC_START,kernel_size);
	
	
	int (*new_entry)(int, char**) = (void (*)(void)) (((uint32_t) aligned)+offset);
	
	uart_printf("relocated to %d\n",aligned);
	
	uart_printf("unaligned: %d\n",malloced_chunk);
	
	
	uart_send_string("entering relocated kernel\n");
	
	// TODO clear the cache
	
	new_entry(1,(char**)0x53544c41);
	uart_send_string("return from relocated kernel\n");
	
	
	
	
	return;
}

































