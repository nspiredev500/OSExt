#include "kernel.h"








uint32_t newCPTD(unsigned char domain,uint32_t base_address)
{
	const uint32_t cptd_template = 0b100010000000000000000000000000;
	uint32_t desc = cptd_template;
	desc = desc | ((domain & 0b1111) << 5) | (base_address & (~ 0b1111111111));
	return desc;
}

uint32_t newLPD(unsigned char c,unsigned char b,unsigned char ap,uint16_t base_address)
{
	const uint32_t lpd_template = 0b10000000000000000000000000000000;
	uint32_t desc = lpd_template;
	desc = desc | ((c & 0b1) << 3) | ((b & 0b1) << 2) | (ap << 4) | (base_address & (~ 0xffffffff));
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
	"mcr p15, 0, r0, c7, c7, 0":::"r0");
}


























