#include "kernel.h"











// page descriptors have to be in DMA memory, because they are what resolves the virtual addresses
// temporary page descriptors for the remapped kernel, will be freed when a proper page table is set up by the kernel
static uint32_t *init_pds_unaligned = NULL;
static uint32_t *init_pds = NULL;







void free_init_pds()
{
	if (init_pds_unaligned != NULL)
	{
		ti_free(init_pds_unaligned);
		init_pds = NULL;
		init_pds_unaligned = NULL;
	}
}

void* get_init_pds()
{
	return init_pds;
}


// somehow the linker variables get changed by relocation, subtracting &_EXEC_START gives the right values


void relocate_self(void)
{
	// TODO  relocate to a virtual address, by changing the translation table, flushing the tlb and the copying
	DEBUGPRINTF_1("relocating\n")
	uint32_t offset = ((uint32_t) main)-((uint32_t) (&_EXEC_START));
	uint32_t kernel_size = (uint32_t) ((&_EXEC_SIZE)-(&_EXEC_START));
	DEBUGPRINTF_1("kernel size: 0x%x\n",kernel_size,&_EXEC_SIZE-&_EXEC_START)
	
	
	
	DEBUGPRINTF_2("text: 0x%x, data: 0x%x, bss: 0x%x\n",&_TEXT_SIZE-&_EXEC_START,&_DATA_SIZE-&_EXEC_START,&_BSS_SIZE-&_EXEC_START)
	//DEBUGPRINTF_1("main: 0x%x, exec_start: 0x%x\n",main,&_EXEC_START)
	
	DEBUGPRINTF_2("got: 0x%x, gotsize: 0x%x\n",&_GOT_START-&_EXEC_START,&_GOT_SIZE)
	
	
	
	
	
	uint32_t malloced_chunk = (uint32_t) ti_malloc(kernel_size+SMALL_PAGE_SIZE*4); // extra size to align the kernel on a (large) page boundrary
	void *aligned = (void*) makeSmallPageAligned((void*) malloced_chunk);
	
	if (((void*) malloced_chunk) == NULL)
	{
		uart_send_string("Not enough memory!\n");
		return;
	}
	
	
	
	
	uint32_t sections = (kernel_size/SECTION_SIZE)+1;
	
	init_pds_unaligned = ti_calloc(sizeof(uint32_t)*sections*256+16+SMALL_PAGE_SIZE*2);
	if (init_pds_unaligned == NULL)
	{
		ti_free((void*) malloced_chunk);
		uart_send_string("Not enough memory!\n");
		return;
	}
	init_pds = makeSmallPageAligned(init_pds_unaligned);
	DEBUGPRINTF_1("pds: 0x%x\n",init_pds)
	
	
	
	
	
	
	
	
	
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	DEBUGPRINTF_3("tt_base: 0x%x\n",tt_base)
	uint32_t *tt = (uint32_t*) tt_base;
	
	
	
	// TODO map virtual_base_address to the copy of the kernel, by making the invalid tt-entries to coarse page tables and map the large pages to the copy, and the leftover pages let be invalid
	
	
	uint32_t section = ((uint32_t) virtual_base_address);
	
	for (uint32_t i = 0;i<(kernel_size/SMALL_PAGE_SIZE)+1;i++)
	{
		init_pds[i] = newSPD(1,1,0b01010101,(((uint32_t) aligned)+i*SMALL_PAGE_SIZE));
		DEBUGPRINTF_3("page: i: 0x%x, address: 0x%x, descriptor: 0x%x, descaddr: 0x%x\n",i,(((uint32_t) aligned)+i*SMALL_PAGE_SIZE),init_pds[i],init_pds+i)
	}
	
	
	for (uint32_t i = 0;i<sections;i++)
	{
		tt[(section+SECTION_SIZE*i)>>20] = newCPTD(0,(uint32_t) (init_pds+256*i));
		DEBUGPRINTF_3("CPTD: 0x%x, addr: 0x%x, pages: 0x%x\n",tt[(section+SECTION_SIZE*i)>>20],tt+((section+SECTION_SIZE*i)>>20),init_pds+256*i)
	}
	
	clear_caches();
	
	
	invalidate_TLB();
	
	
	
	
	// every variable modified before this point will be copied into the new kernel
	k_memcpy((void*) virtual_base_address,&_EXEC_START,kernel_size);
	
	
	
	int (*new_entry)(int, char**) = (int (*)(int, char**)) (((uint32_t) virtual_base_address)+offset);
	
	DEBUGPRINTF_1("relocated to 0x%x\n",aligned);
	
	DEBUGPRINTF_1("unaligned: 0x%x\n",malloced_chunk);
	
	
	DEBUGPRINTF_1("entering relocated kernel\n");
	
	clear_caches();
	
	new_entry(1,(char**)0x53544c41);
	DEBUGPRINTF_1("return from relocated kernel\n");
	
	
	
	
	return;
}

































