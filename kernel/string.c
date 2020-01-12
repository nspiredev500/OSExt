#include "kernel.h"








uint32_t uint_to_ascii(uint32_t a,char *str) // returns the number of digits
{
	//max uint32_t: 4 294 967 296
	
	char buff[30];
	k_memset(buff,'\0',28);
	
	
	int digit = 0;
	while (true)
	{
		uint32_t tmp = (a % 10);
		if (tmp > 9)
			break;	// something has gone wrong
		buff[digit] = 48+tmp;
		//uart_send(48+tmp);
		a = a / 10;
		if (a == 0)
			break;
		digit++;
	}
	//uart_send('\n');
	uint32_t offset = 0;
	for (int i = digit;i>=0;i--)
	{
		str[offset] = buff[i];
		offset++;
	}
	return offset+1;
}








































