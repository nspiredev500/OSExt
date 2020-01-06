#include "kernel.h"


static inline int wa_syscall(int nr, int p1)
{
	register int r0 asm("r0") = p1;
  
	asm volatile(
		"swi %[nr]\n"
		: "=r" (r0)
		: [nr] "i" (nr), "r" (r0)
		: "memory", "r1", "r2", "r3", "r4", "r12", "lr");
  
	return r0;
}

void *ti_malloc(uint32_t size) // not 8-byte-aligned !
{
	return wa_syscall(e_malloc,size);
}

void ti_free(void *ptr)
{
	return wa_syscall(e_free,ptr);
}



void *k_memset(void *str,int c, size_t n)
{
	unsigned char cc = (unsigned char) c;
	unsigned char *area = (unsigned char*) str;
	for (size_t i = 0;i<n;i++)
	{
		*area = cc;
		area++;
	}
	return str;
}


void *k_memcpy(void *str1,const void *str2, size_t n)
{
	char *area1 = (char*) str1;
	char *area2 = (char*) str2;
	for (size_t i = 0;i<n;i++)
	{
		*area1 = *area2;
		area1++;
		area2++;
	}
	return str1;
}


// is this right?
int k_memcmp(const void *str1, const void *str2, size_t n)
{
	unsigned char *area1 = (unsigned char*) str1;
	unsigned char *area2 = (unsigned char*) str2;
	for (size_t i = 0;i<n;i++)
	{
		if (*area1 != *area2)
		{
			if (*area1 < *area2)
				return -1;
			else
				return 1;
		}
		area1++;
		area2++;
	}
	return 0;
}







