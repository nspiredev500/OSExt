#include "kernel.h"



static inline int wa_syscall(int nr)
{
	register int r0 asm("r0");
  
	asm volatile(
		"swi %[nr]\n"
		: "=r" (r0)
		: [nr] "i" (nr)
		: "memory", "r1", "r2", "r3", "r4", "r12", "lr");
  
	return r0;
}

static inline int wa_syscall1(int nr, int p1)
{
	register int r0 asm("r0") = p1;
  
	asm volatile(
		"swi %[nr]\n"
		: "=r" (r0)
		: [nr] "i" (nr), "r" (r0)
		: "memory", "r1", "r2", "r3", "r4", "r12", "lr");
  
	return r0;
}

static inline int wa_syscall2(int nr, int p1, int p2)
{
	register int r0 asm("r0") = p1;
	register int r1 asm("r1") = p2;
  
	asm volatile(
		"swi %[nr]\n"
		: "=r" (r0)
		: [nr] "i" (nr), "r" (r0), "r" (r1)
		: "memory", "r2", "r3", "r4", "r12", "lr");
  
	return r0;
}

static inline int wa_syscall3(int nr, int p1, int p2, int p3)
{
	register int r0 asm("r0") = p1;
	register int r1 asm("r1") = p2;
	register int r2 asm("r2") = p3;
	
	asm volatile(
		"swi %[nr]\n"
		: "=r" (r0)
		: [nr] "i" (nr), "r" (r0), "r" (r1), "r" (r2)
		: "memory", "r3", "r4", "r12", "lr");
  
	return r0;
}

static inline int wa_syscall4(int nr, int p1, int p2, int p3, int p4)
{
	register int r0 asm("r0") = p1;
	register int r1 asm("r1") = p2;
	register int r2 asm("r2") = p3;
	register int r3 asm("r3") = p4;
 
	asm volatile(
		"swi %[nr]\n"
		: "=r" (r0)
		: [nr] "i" (nr), "r" (r0), "r" (r1), "r" (r2), "r" (r3)
		: "memory", "r4", "r12", "lr");
  
	return r0;
}









int TCT_Local_Control_Interrupts(int mask)
{
	return wa_syscall1(e_TCT_Local_Control_Interrupts,mask);
}

void *ti_malloc(uint32_t size) // not 8-byte-aligned !
{
	return (void*) wa_syscall1(e_malloc,size);
}

void ti_free(void *ptr)
{
	wa_syscall1(e_free,(uint32_t) ptr);
}

void *ti_calloc(uint32_t size) // not 8-byte-aligned !
{
	void* p = ti_malloc(size);
	if (p == NULL)
	{
		return NULL;
	}
	k_memset(p,0,(size_t) size);
	return p;
}

void ascii2utf16(char *string16,char *string, size_t size)
{
	wa_syscall3(e_ascii2utf16, (int) string16,(int) string,size);
}




NUC_FILE* nuc_fopen(char *path,char *mode)
{
	return (NUC_FILE*) wa_syscall2(e_fopen,(int) path,(int) mode);
}


void nuc_fclose(NUC_FILE* f)
{
	wa_syscall1(e_fclose,(int) f);
}


uint32_t nuc_fread(void* buffer,uint32_t elements,uint32_t size,NUC_FILE* f)
{
	return wa_syscall4(e_fread,(int) buffer,elements,size,(int) f);
}


uint32_t nuc_fwrite(void* buffer,uint32_t elements,uint32_t size,NUC_FILE* f)
{
	return wa_syscall4(e_fwrite,(int) buffer,elements,size,(int) f);
}


int32_t nuc_fseek(NUC_FILE* f,int32_t offset, int32_t whence)
{
	return wa_syscall3(e_fseek,(int) f,offset,whence);
}


int32_t nuc_fflush(NUC_FILE* f)
{
	return wa_syscall1(e_fflush,(int) f);
}
















// currently not working
/*
int savedlr_stack[10];
int savedlr_stack_nr = 0;
__attribute__((naked)) int _show_1NumericInput(int p1, const char *p2, const char *p3, const char *p4, int *p5, int p6, int p7, int p8)
{
	asm volatile("push {r4-r6}\n"
				"ldr r4, =savedlr_stack\n"
				"ldr r5, =savedlr_stack_nr\n"
				"ldr r6, [r5]\n"
				"add r6, r6, #1\n"
				"str lr, [r4, r6, lsl #2]\n"
				"str r6, [r5]\n"
				"pop {r4-r6}\n"
				"swi %[nr]\n"
				"push {r4-r6}\n"
				"ldr r4, =savedlr_stack\n"
				"ldr r5, =savedlr_stack_nr\n"
				"ldr r6, [r5]\n"
				"ldr lr, [r4, r6, lsl #2]\n"
				"sub r6, r6, #1\n"
				"str r6, [r5]\n"
				"pop {r4-r6}\n"
				"bx lr\n"
				".ltorg" :: [nr] "i" (e__show_1NumericInput));
}


int32_t show_1_numeric_input(char* title,char* subtitle,char* msg,int32_t *val)
{
	if (k_strlen(title,50) >= 50 || k_strlen(subtitle,50) >= 50 || k_strlen(msg,50) >= 50)
	{
		DEBUGPRINTLN_1("showinput: strings too long")
		return 0;
	}
	static char title16[110];
	static char subtitle16[110];
	static char msg16[110];
	k_memset(title16,'\0',105);
	k_memset(subtitle16,'\0',105);
	k_memset(msg16,'\0',105);
	ascii2utf16(title16,title,sizeof(title16));
	ascii2utf16(subtitle16,subtitle,sizeof(subtitle16));
	ascii2utf16(msg16,msg,sizeof(msg16));
	
	int32_t mask = TCT_Local_Control_Interrupts(0);
	bool irq = isIRQ();
	enableIRQ();
	//asm(".long 0xE1212374"); // bkpt
	int32_t ret = (_show_1NumericInput(0, (const char*) title16, (const char*) subtitle16, (const char*) msg16, (int*) val, 1,INT_MIN, INT_MAX) == 5103);
	if (! irq)
	{
		disableIRQ();
	}
	TCT_Local_Control_Interrupts(mask);
	
	
	return ret;
}
*/













