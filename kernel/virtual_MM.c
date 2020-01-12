#include "kernel.h"





struct coarse_ptd newCPD(unsigned char domain,uint32_t base_address)
{
	struct coarse_ptd c;
	c.typ = 0b01;
	c.zeros = 0b00;
	c.one = 0b1;
	c.domain = domain;
	c.zero = 0b0;
	c.base_address = base_address;
	return c;
}
struct large_pd newLPD(unsigned char c,unsigned char b,unsigned char ap,uint16_t base_address)
{
	struct large_pd d;
	d.typ = 0b01;
	d.b = c;
	d.c = c;
	d.ap0 = ap & (0b11);
	d.ap1 = ap & (0b11 >> 2);
	d.ap2 = ap & (0b11 >> 4);
	d.ap3 = ap & (0b11 >> 6);
	d.zeros = 0b0000;
	d.base_address = base_address;
	return d;
}



/*
uint32_t newCPTD(unsigned char domain,uint32_t base_address)
{
	
	
	
	
	
}

uint32_t newLPD(unsigned char c,unsigned char b,unsigned char ap,uint16_t base_address)
{
	
	
	
	
	
	
}
*/




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


























