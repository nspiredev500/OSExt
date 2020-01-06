#include "kernel.h"





//static uint32_t test = 20;



int main(int argsn,char **argc)
{
	
	
	//test to see whether the pointer gets updated by relocation
	/*
	char buff[40];
	uint32_t len = uint_to_ascii(&test,buff);
	uart_send_string_length(buff,len);
	uart_send('\n');
	*/
	
	/*
	char buff[40];
	uint32_t len = uint_to_ascii(_EXEC_START,buff);
	uart_send_string_length(buff,len);
	uart_send('\n');
	*/
	
	
	
	
	
	//uart_printf("&test: %d\n",&test);
	
	if (argsn == 1 && ((unsigned int) argc) == 0x53544c41) //STandaloneLAunch
	{
		uart_send_string("relocated\n");
		// relocation finished or already done by loader
		initialize();
	}
	else
	{
		relocate_self();
	}
	return 0;
}




void initialize()
{
	uart_send_string("initializing\n");
	
	
	
	
	
	
	
	
}




























