#include "kernel.h"











// page descriptors have to be in DMA memory, because they are what resolves the virtual addresses
// temporary page descriptors for the remapped kernel, will be freed when a proper page table is set up by the kernel
static uint32_t *init_pds_unaligned = NULL;
static uint32_t *init_pds = NULL;

static const void *virtual_base_address = 0xf9c00000;



static const uint32_t PAGESIZE = 1024*64;
static const uint32_t SECTIONSIZE = 1024*1024;

void free_init_pds()
{
	if (init_pds_unaligned != NULL)
	{
		ti_free(init_pds_unaligned);
		init_pds = NULL;
		init_pds_unaligned = NULL;
	}
}

void relocate_self(void)
{
	// TODO  relocate to a virtual address, by changing the translation table, flushing the tlb and the copying
	uart_send_string("relocating\n");
	uint32_t offset = ((uint32_t) main)-((uint32_t) _EXEC_START);
	uint32_t kernel_size = ((uint32_t) _EXEC_END)-((uint32_t) _EXEC_START);
	
	
	
	
	uint32_t malloced_chunk = (uint32_t) ti_malloc(kernel_size+PAGESIZE*4); // extra size to align the kernel on a (large) page boundrary
	void *aligned = (void*) ((malloced_chunk & (~ 0xFFFF))+0x10000);
	
	if (malloced_chunk == NULL)
	{
		uart_send_string("Not enough memory!\n");
		return;
	}
	
	
	
	
	uint32_t sections = (kernel_size/SECTIONSIZE)+1;
	
	init_pds_unaligned = ti_malloc(sizeof(uint32_t)*sections*256+16+1024*2);
	if (init_pds_unaligned == NULL)
	{
		free(malloced_chunk);
		uart_send_string("Not enough memory!\n");
		return;
	}
	k_memset(init_pds_unaligned,0,sizeof(uint32_t)*sections*256+16+1024*2);
	if ((((uint32_t) init_pds_unaligned) & 0b1111111111) != 0) // align to 1k boundrary
	{
		init_pds = (((uint32_t) init_pds_unaligned) & (~ 0b1111111111))+0b10000000000;
	}
	else
	{
		init_pds = init_pds_unaligned;
	}
	uart_printf("pds: %d\n",init_pds);
	
	
	
	
	
	
	
	
	
	register uint32_t tt_base asm("r0");
	asm("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	uart_printf("tt_base: %d\n",tt_base);
	uint32_t *tt = tt_base;
	
	
	
	// TODO map virtual_base_address to the copy of the kernel, by making the invalid tt-entries to coarse page tables and map the large pages to the copy, and the leftover pages let be invalid
	
	
	uint32_t section = ((uint32_t) virtual_base_address);
	
	for (uint32_t i = 0;i<(kernel_size/PAGESIZE)+1;i++)
	{
		for (int a = 0;a<16;a++)
		{
			init_pds[a+i*16] = newLPD(1,1,0b01010101,(((uint32_t) aligned)+i*PAGESIZE));
		}
		uart_printf("page: i: %d, address: %d, descriptor: %d, descaddr: %d\n",i,(((uint32_t) aligned)+i*PAGESIZE),init_pds[i],init_pds+i);
	}
	
	
	for (int i = 0;i<sections;i++)
	{
		tt[(section+SECTIONSIZE*i)>>20] = newCPTD(0,init_pds+256*i);
		uart_printf("CPTD: %d, addr: %d, pages: %d\n",tt[(section+SECTIONSIZE*i)>>20],tt+((section+SECTIONSIZE*i)>>20),init_pds+256*i);
		
		
		/*
		tt[(section+SECTIONSIZE*i)>>20] = newCPD(0,(init_pds+256*i));
		uart_printf("descriptor: %d\n",(*((uint32_t*)&tt[(section+SECTIONSIZE*i)>>20]))&(~ 0b1111111111));
		uart_printf("section: i: %d, index: %d, address: %d\n",i,(section+SECTIONSIZE*i)>>20,init_pds+256*i);
		*/
	}
	
	clear_caches();
	
	
	invalidate_TLB();
	
	
	
	
	// every variable modified before this point will be copied into the new kernel
	k_memcpy(virtual_base_address,_EXEC_START,kernel_size);
	
	
	
	int (*new_entry)(int, char**) = (void (*)(void)) (((uint32_t) virtual_base_address)+offset);
	
	uart_printf("relocated to %d\n",aligned);
	
	uart_printf("unaligned: %d\n",malloced_chunk);
	
	
	uart_send_string("entering relocated kernel\n");
	
	clear_caches();
	
	new_entry(1,(char**)0x53544c41);
	uart_send_string("return from relocated kernel\n");
	
	
	
	
	return;
}

































