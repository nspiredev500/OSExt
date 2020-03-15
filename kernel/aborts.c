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

static const uint8_t undef_offset = 0x4;
static const uint8_t swi_offset = 0x8;
static const uint8_t prefetch_offset = 0xc;
static const uint8_t data_offset = 0x10;
static const uint8_t irq_offset = 0x18;
static const uint8_t fiq_offset = 0x1c;


static const uint8_t undef_adr_offset = 0x24+8; // +8, because pc in pc-relative access is 2 instructions after
static const uint8_t swi_adr_offset = 0x28+8;
static const uint8_t prefetch_adr_offset = 0x2c+8;
static const uint8_t data_adr_offset = 0x30+8;
static const uint8_t irq_adr_offset = 0x38+8;
static const uint8_t fiq_adr_offset = 0x3c+8;


volatile void *remapped_exception_vectors = (void*) 0xffff0000;


asm(
"undef_jump: ldr pc, [pc, #0x20] \n"
"swi_jump: ldr pc, [pc, #0x20] \n"
"prefetch_jump: ldr pc, [pc, #0x20] \n"
"data_jump: ldr pc, [pc, #0x20] \n"
"irq_jump: ldr pc, [pc, #0x20] \n"
"fiq_jump: ldr pc, [pc, #0x20] \n");




bool install_exception_handlers()
{
	
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
	extern void swi_wrapper();
	extern void data_wrapper();
	extern void irq_wrapper();
	extern void fiq_wrapper();
	
	
	set_exception_vectors(true);
	
	void undef_jump();
	void swi_jump();
	void prefetch_jump();
	void data_jump();
	void irq_jump();
	void fiq_jump();
	
	void *page = usePage();
	if (page == NULL)
	{
		return false;
	}
	addVirtualKernelPage(page,(void*) remapped_exception_vectors);
	
	init_syscall_table();
	
	volatile uint32_t *vector = (remapped_exception_vectors+undef_offset);
	*vector = *(uint32_t*) undef_jump;
	
	
	vector = (remapped_exception_vectors+swi_offset);
	*vector = *(uint32_t*) swi_jump;
	
	vector = (remapped_exception_vectors+prefetch_offset);
	*vector = *(uint32_t*) prefetch_jump;
	
	
	vector = (remapped_exception_vectors+data_offset);
	*vector = *(uint32_t*) data_jump;
	
	
	vector = (remapped_exception_vectors+irq_offset);
	*vector = *(uint32_t*) irq_jump;
	
	vector = (remapped_exception_vectors+fiq_offset);
	*vector = *(uint32_t*) fiq_jump;
	
	
	
	vector = (remapped_exception_vectors+undef_adr_offset);
	*vector = (uint32_t) undef_wrapper;
	
	
	vector = (remapped_exception_vectors+swi_adr_offset);
	*vector = (uint32_t) swi_wrapper;
	
	
	vector = (remapped_exception_vectors+prefetch_adr_offset);
	*vector = (uint32_t) prefetch_wrapper;
	
	
	vector = (remapped_exception_vectors+data_adr_offset);
	*vector = (uint32_t) data_wrapper;
	
	
	vector = (remapped_exception_vectors+irq_adr_offset);
	*vector = (uint32_t) irq_wrapper;
	
	
	vector = (remapped_exception_vectors+fiq_adr_offset);
	*vector = (uint32_t) fiq_wrapper;
	
	
	
	
	
	// TODO but the page with the fiq handler in the lockdown-tlb, by making an offset variable in the linker script
	
	
	
	return true;
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




























