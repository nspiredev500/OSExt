#include "kernel.h"



static const void *ram_base_address = (void*) 0x10000000;
/*
static const unsigned int RAMSIZE = 1024*1024*64;
static const unsigned int PAGESIZE = 1024*4;
static const unsigned int SECTIONSIZE = 1024*1024;
*/
#define RAMSIZE (1024*1024*64)
#define PAGESIZE (1024*64)
#define SECTIONSIZE (1024*1024)
static unsigned char freepages[RAMSIZE/PAGESIZE]; // aka all data zero
static unsigned char osextpages[RAMSIZE/PAGESIZE]; // pages used by osext






bool isPageFree(unsigned int page)
{
	if (page > (RAMSIZE/PAGESIZE))
		return false;
	unsigned int byte = page / 8;
	unsigned int bit = page % 8;
	unsigned char entry = freepages[byte] >> bit;
	if ((entry & 0b1) == 0b1)
	{
		return true;
	}
}
void setPageFree(unsigned int page, bool free)
{
	if (page > (RAMSIZE/PAGESIZE))
		return;
	unsigned int byte = page / 8;
	unsigned int bit = page % 8;
	unsigned char entry = freepages[byte];
	if (free)
	{
		entry = entry | (0b1 << bit);
	}
	else
	{
		entry = entry & (~ (0b1 << bit));
	}
	freepages[byte] = entry;
}


bool isPageOsext(unsigned int page)
{
	if (page > (RAMSIZE/PAGESIZE))
		return false;
	unsigned int byte = page / 8;
	unsigned int bit = page % 8;
	unsigned char entry = osextpages[byte] >> bit;
	if ((entry & 0b1) == 0b1)
	{
		return true;
	}
}
void setPageOsext(unsigned int page, bool free)
{
	if (page > (RAMSIZE/PAGESIZE))
		return;
	unsigned int byte = page / 8;
	unsigned int bit = page % 8;
	unsigned char entry = osextpages[byte];
	if (free)
	{
		entry = entry | (0b1 << bit);
	}
	else
	{
		entry = entry & (~ (0b1 << bit));
	}
	osextpages[byte] = entry;
}








//uint32_t ti_malloc = 5;
//uint32_t ti_free = 6;

void scanPages()
{
	int numpages = RAMSIZE / PAGESIZE;
	int numsections = RAMSIZE / SECTIONSIZE;
	int numfree = 0;
	for (int i = 0;i<numpages;i++)
	{
		bool free = true;
		volatile uint32_t *mem = (volatile uint32_t*) (((uint32_t) ram_base_address)+i*PAGESIZE);
		if (mem >= 0x13ea0000 && mem <= 0x13eaf000) // pages not mapped?
			continue;
		for (int a = 0;a<PAGESIZE/4;a++)
		{
			if (*mem != 0)
			{
				free = false;
				break;
			}
			mem++;
		}
		if (free)
			numfree++;
		setPageFree(i,free);
	}
	register unsigned char percent asm("r0") = (int) ((((double)numfree)/((double)numpages))*100);
	//register unsigned int percent asm("r0") = numfree;
	//asm("bkpt"::"r" (percent));
	uart_send(percent);
	uart_send('\n');
}































