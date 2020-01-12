#include "kernel.h"





static uint32_t test = 20;

static uint32_t *test2 = &test;

static uint32_t *test3 = NULL;



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
	
	
	//uart_printf("size: %d, %d, %d\n",sizeof(struct section_descriptor),sizeof(struct coarse_page_table_descriptor),sizeof(struct fine_page_table_descriptor));
	
	
	
	//uart_printf("size: %d\n",sizeof(struct large_page_descriptor));
	
	
	//uart_printf("test: %d\n",test);
	//uart_printf("test2: %d\n",test2);
	//uart_printf("test3: %d\n",test3);
	
	
	if (argsn == 1 && ((unsigned int) argc) == 0x53544c41) //STandaloneLAunch
	{
		uart_send_string("relocated\n");
		// relocation finished or already done by loader
		initialize();
	}
	else
	{
		//test3 = ti_malloc(8);
		//uart_printf("test3: %d\n",test3);
		relocate_self();
	}
	return 0;
}




void initialize()
{
	uart_send_string("initializing\n");
	
	
	
	
	
	
	
	
}




























