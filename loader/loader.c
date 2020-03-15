#include <os.h>

#include <nspireio2.h>


#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include <stdbool.h>
#include "zehn_loader.h"

int main(int argsn, char **argv)
{
	uart_printf("osext loader\n\n");
	if (argsn > 1 && argv[1] != NULL)
	{
		uart_printf("%s\n",argv[1]);
		NUC_FILE *f = nuc_fopen(argv[1],"rb");
		if (f == NULL)
			return 0xDEAD;
		bool hw;
		void *mem;
		int (*entry)(int, char**);
		if (zehn_load(f,&mem,&entry,&hw) == 0)
		{
			nuc_fclose(f);
			//entry(1,(char**) 0x53544c41);
			entry(0,NULL);
			return 0;
		}
		else
		{
			nuc_fclose(f);
			return 0xDEAD;
		}
	}
	cfg_register_fileext("ose","osextloader");;
	return 0;
}




void* makeSmallPageAligned(void *ptr)
{
	if ((((uint32_t) ptr) & (~ 0xFFF)) == (uint32_t) ptr)
		return ptr;
	return (void*) ((((uint32_t) ptr) & (~ 0xFFF))+0x1000);
}


uint32_t newCPTD(unsigned char domain,uint32_t base_address)
{
	const uint32_t cptd_template = 0b000000000000000000000000010001;
	uint32_t desc = cptd_template;
	desc = desc | ((domain & 0b1111) << 5) | (base_address & (~ 0b1111111111));
	return desc;
}

uint32_t newSD(unsigned char c,unsigned char b,unsigned char domain,unsigned char ap,uint32_t base_address)
{
	const uint32_t sd_template = 0b00000000000000000000000000010010;
	uint32_t desc = sd_template;
	desc = desc | ((c & 0b1) << 3) | ((b & 0b1) << 2) | ((domain & 0b1111) << 5) | ((ap & 0b11) << 10) | (base_address & (~ 0b11111111111111111111));
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





