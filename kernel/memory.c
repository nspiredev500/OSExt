#include "kernel.h"


void* align4Bytes(void* address)
{
	uint32_t adr = (uint32_t) address;
	if ((adr & 0b11) == 0)
	{
		return address;
	}
	return (void*) ((adr & (~ 0b11)) + 0b100);
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
		//uart_printf("copy: source: %d, dest: %d\n",area2,area1);
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

void* (*memset)(void* str,int c,size_t n) = k_memset;
void* (*memcpy)(void* str,const void* str2,size_t n) = k_memcpy;
void* (*memmove)(void* str,const void* str2,size_t n) = k_memcpy;
int (*memcmp)(const void* str,const void* str2,size_t n) = k_memcmp;

