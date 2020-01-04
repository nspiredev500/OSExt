#include "kernel.h"




static volatile uint32_t *data = (volatile uint32_t*) 0x90020000;


//TODO check for full FIFO buffer


void uart_send_string(char *string)
{
	char *c = string;
	while (true)
	{
		if (*c == '\0')
			break;
		uart_send(*c);
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






















