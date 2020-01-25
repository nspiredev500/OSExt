#include "kernel.h"






uint32_t k_pow(uint32_t a,uint32_t b)
{
	if (b == 0)
	{
		return 1;
	}
	if (b == 1)
	{
		return a;
	}
	uint32_t sum = a;
	for (uint32_t i = 0;i<b-1;i++)
	{
		sum *= a;
	}
	return sum;
}

uint32_t pow_of_2(uint32_t exp)
{
	return 0b1 << exp;
}

uint32_t set_bits(uint32_t b)
{
	return 0xffffffff >> (32-b);
}





