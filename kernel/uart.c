#include "kernel.h"




static volatile uint32_t *data = (volatile uint32_t*) 0x90020000;


//TODO check for full FIFO buffer


void uart_printf(char *str,...)
{
	va_list va;
	va_start(va,str);
	char *c = str;
	while (true)
	{
		if (*c == '\0')
			break;
		if (*c == '%' && *(c+1) != '\0')
		{
			if (*(c+1) == 's')
			{
				char *str2 = va_arg(va,char*);
				uart_send_string(str2);
				c += 2;
				continue;
			}
			if (*(c+1) == 'd')
			{
				uart_send_uint32(va_arg(va,uint32_t));
				c += 2;
				continue;
			}
			if (*(c+1) == 'x')
			{
				uart_send_uint32_base(va_arg(va,uint32_t),16);
				c += 2;
				continue;
			}
			if (*(c+1) == 'l' && *(c+2) == 'l' && *(c+3) == 'x')
			{
				uart_send_uint32_base(va_arg(va,uint64_t),16);
				c += 4;
				continue;
			}
			if (*(c+1) == 'l' && *(c+2) == 'l' && *(c+3) == 'd')
			{
				uart_send_uint32_base(va_arg(va,uint64_t),10);
				c += 4;
				continue;
			}
		}
		uart_send(*c);
		c++;
	}
	va_end(va);
}

void uart_println(char *str,...)
{
	va_list va;
	va_start(va,str);
	char *c = str;
	while (true)
	{
		if (*c == '\0')
			break;
		if (*c == '%' && *(c+1) != '\0')
		{
			if (*(c+1) == 's')
			{
				char *str2 = va_arg(va,char*);
				uart_send_string(str2);
				c += 2;
				continue;
			}
			if (*(c+1) == 'd')
			{
				uart_send_uint32(va_arg(va,uint32_t));
				c += 2;
				continue;
			}
			if (*(c+1) == 'x')
			{
				uart_send_uint32_base(va_arg(va,uint32_t),16);
				c += 2;
				continue;
			}
			if (*(c+1) == 'l' && *(c+2) == 'l' && *(c+3) == 'x')
			{
				uart_send_uint32_base(va_arg(va,uint64_t),16);
				c += 4;
				continue;
			}
			if (*(c+1) == 'l' && *(c+2) == 'l' && *(c+3) == 'd')
			{
				uart_send_uint32_base(va_arg(va,uint64_t),10);
				c += 4;
				continue;
			}
		}
		uart_send(*c);
		c++;
	}
	uart_send('\n');
	va_end(va);
}

void uart_send_uint64_base(uint64_t a,uint32_t base)
{
	//max uint32_t: 4 294 967 296
	
	char buff[68];
	k_memset(buff,'\0',66);
	
	
	uint32_t digit = 0;
	while (true)
	{
		uint64_t tmp = (a % base);
		if (tmp > 15)
			break;	// something has gone wrong
		char c = (tmp<10) ? 48+tmp:97+tmp-10;
		buff[digit] = c;
		a = a / base;
		if (a == 0)
			break;
		digit++;
	}
	for (int i = digit;i>=0;i--)
	{
		uart_send(buff[i]);
	}
}


void uart_send_uint32_base(uint32_t a,uint32_t base)
{
	//max uint32_t: 4 294 967 296
	
	char buff[36];
	k_memset(buff,'\0',34);
	
	
	int digit = 0;
	while (true)
	{
		uint32_t tmp = (a % base);
		if (tmp > 15)
			break;	// something has gone wrong
		char c = (tmp<10) ? 48+tmp:97+tmp-10;
		buff[digit] = c;
		a = a / base;
		if (a == 0)
			break;
		digit++;
	}
	for (int i = digit;i>=0;i--)
	{
		uart_send(buff[i]);
	}
}


void uart_send_uint32(uint32_t a)
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
		a = a / 10;
		if (a == 0)
			break;
		digit++;
	}
	for (int i = digit;i>=0;i--)
	{
		uart_send(buff[i]);
	}
}



void uart_send_string(char *string)
{
	char *c = string;
	while (true)
	{
		if (*c == '\0')
			break;
		uart_send(*c);
		c++;
	}
}

void uart_send_string_length(char *string,int length)
{
	for (int i = 0;i<length;i++)
	{
		uart_send(string[i]);
	}
}


void uart_send(char c)
{
	*data = c;
}






char uart_rec()
{
	
	
	
	
	
	return 0;
}






















