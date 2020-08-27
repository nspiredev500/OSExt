#include "../libosext/libosext.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <libndls.h>

static volatile uint32_t *data = (volatile uint32_t*) 0x90020000;
void uart_send(char c);
void uart_send_string(const char *string)
{
	const char *c = string;
	while (true)
	{
		if (*c == '\0')
			break;
		uart_send(*c);
		c++;
	}
}

void uart_send(char c)
{
	*data = c;
}

void output_thread()
{
	uart_send_string("starting second thread\n");
	//bkpt();
	for (int i = 0;i<60;i++)
	{
		//bkpt();
		uart_send_string("Hello from the second thread!\n");
		//osext_thread_wait_milli(100);
		//bkpt();
		osext_thread_wait_milli(2);
	}
}


#define stacksize (1024*64)

int main(void)
{
	uart_send_string("Hello from main thread\n");
	void* stack = calloc(stacksize,1);
	if (stack == NULL)
	{
		uart_send_string("Out of memory!\n");
		return -1;
	}
	//bkpt();
	osext_init_threads();
	if (osext_errno == -1)
	{
		uart_send_string("cannot initialize threads!\n");
		printf("version: 0x%lx\n",osext_version());
		return -1;
	}
	//bkpt();
	osext_thread* t = osext_create_thread(stack,stacksize,output_thread);
	//bkpt();
	//osext_thread_wait_for_thread(t);
	//bkpt();
	
	for (int i = 0;i<60;i++)
	{
		if (i == 10)
		{
			FILE* f = fopen("/documents/test.tns","w");
			if (f != NULL)
			{
				fwrite("test file.",1,strlen("test file."),f);
				fclose(f);
			}
		}
		uart_send_string("Hello from the first thread!\n");
		osext_thread_wait_milli(2);
	}
	
	uart_send_string("Goodbye from main thread!\n");
	osext_exit_threads();
	free(stack);
	return 0;
}



