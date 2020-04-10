#include "kernel.h"


static volatile bool recovery = false;

static void* recovery_start = NULL;

static void* recovery_phys = NULL;

uint32_t* recovery_tt = NULL;


static void __attribute__ ((noreturn)) reset(void) {
	*(volatile unsigned*)0x900A0008 = 2;
	__builtin_unreachable();
}

static void map_recovery_kernel()
{
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	uint32_t *tt = (uint32_t*) tt_base;
	
	tt[(uint32_t) recovery_start >> 20] = newCPTD(0,(uint32_t) recovery_tt);
	
	clear_caches();
	invalidate_TLB();
}


static void unmap_recovery_kernel()
{
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	uint32_t *tt = (uint32_t*) tt_base;
	
	tt[(uint32_t) recovery_start >> 20] = 0;
	
	clear_caches();
	invalidate_TLB();
}

bool recovery_init()
{
	
	uint32_t kernel_size = (uint32_t) ((&_EXEC_SIZE)-(&_EXEC_START));
	
	
	recovery_start = makeSmallPageAligned((void*) (((uint32_t) virtual_base_address)+kernel_size+SMALL_PAGE_SIZE*2))+SECTION_SIZE;
	
	uint32_t size_pages = (kernel_size/SMALL_PAGE_SIZE)+2;
	
	recovery_phys = useConsecutivePages(size_pages,0);
	
	if (recovery_phys == NULL)
	{
		DEBUGPRINTLN_1("not enough consecutive pages for the recovery kernel!");
		recovery_start = NULL;
		return false;
	}
	
	recovery_tt = requestCPT();
	k_memset(recovery_tt,0,sizeof(uint32_t)*256);
	for (uint32_t i = 0;i<size_pages;i++)
	{
		recovery_tt[i] = newSPD(1,1,0b01010101,(uint32_t) (recovery_phys+SMALL_PAGE_SIZE*i));
	}
	
	map_recovery_kernel();
	
	
	void *got_offset = (void*) ((&_GOT_START)-(&_EXEC_START));
	
	
	
	recovery = true;
	
	k_memcpy(recovery_start,virtual_base_address,kernel_size);
	
	
	// try to change the GOT-entries like the zehn loader does
	uint32_t *got_entry = (uint32_t*) ((uint32_t)recovery_start+(uint32_t)got_offset);
	
	while (*got_entry != 0xFFFFFFFF)
	{
		*got_entry = *got_entry + (uint32_t) (-(uint32_t)(&_EXEC_START)+(uint32_t)recovery_start);
		got_entry++;
	}
	
	
	
	
	
	recovery = false;
	
	
	unmap_recovery_kernel();
	
	return true;
}




bool recovery_mode()
{
	return recovery;
}



void __attribute__ ((noreturn)) recovery_shell(uint32_t cause,uint32_t *regs)
{
	
	
	
	__builtin_unreachable();
}




void __attribute__ ((noreturn)) recovery_enter(uint32_t cause,uint32_t *regs)
{
	if (recovery_start == NULL)
	{
		reset();
	}
	
	disableIRQ();
	disableFIQ();
	
	map_recovery_kernel();
	
	uint32_t shell_offset = (((uint32_t) recovery_shell)-((uint32_t) (&_EXEC_START)));
	
	void (*rec_shell)(uint32_t,uint32_t*) = (recovery_start+shell_offset);
	rec_shell(cause,regs);
	
	
	__builtin_unreachable();
}
































