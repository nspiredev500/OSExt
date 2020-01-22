#include "kernel.h"



const void *virtual_base_address = (const void*) 0xf9c00000;


// store the coarse page descriptors as a table to reuse them
// in every virtual address space, to save memory and have the kernel mapped at all times
static uint32_t kernel_cpdts_size = 0;
static uint32_t **kernel_cpdts = NULL;


struct address_space kernel_space;
static bool kernel_space_initialized = false;

void initializeKernelSpace()
{
	if (kernel_space_initialized)
	{
		return;
	}
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	kernel_space.tt = (uint32_t*) tt_base;
	
	
	
	
	
}


void addVirtualKernelPage(void* page, void* virtual_address)
{
	
	
	
	
	
}



static void expandKernelCPDTs()
{
	
	
	
	
}


// DMA pages should not be removed, as you cannot be sure if some device still tries to access them.
// As they are mapped to their physical address, they can only be accessed if you context-switch to kernel space.
static void addKernelDMAPage()
{
	
	
	
	
	
	
}



uint32_t newCPTD(unsigned char domain,uint32_t base_address)
{
	const uint32_t cptd_template = 0b000000000000000000000000010001;
	uint32_t desc = cptd_template;
	desc = desc | ((domain & 0b1111) << 5) | (base_address & (~ 0b1111111111));
	return desc;
}

uint32_t newLPD(unsigned char c,unsigned char b,unsigned char ap,uint32_t base_address)
{
	const uint32_t lpd_template = 0b00000000000000000000000000000001;
	uint32_t desc = lpd_template;
	desc = desc | ((c & 0b1) << 3) | ((b & 0b1) << 2) | (ap << 4) | ((base_address & 0xffff0000));
	return desc;
}

uint32_t newSPD(unsigned char c,unsigned char b,unsigned char ap,uint32_t base_address)
{
	const uint32_t lpd_template = 0b00000000000000000000000000000010;
	uint32_t desc = lpd_template;
	desc = desc | ((c & 0b1) << 3) | ((b & 0b1) << 2) | (ap << 4) | ((base_address & 0xfffff000));
	return desc;
}













void invalidate_TLB()
{
	asm volatile("mcr p15, 0, r0, c8, c7, 0":::"r0");
}






void clear_caches()
{
	asm volatile(
	"loop: mrc p15, 0, r15, c7, c10, 3 \n" // test and clean d-cache
	"bne loop \n"
	"mcr p15, 0, r0, c7, c7, 0\n" // invalidate cache
	:::"r0");
}


























