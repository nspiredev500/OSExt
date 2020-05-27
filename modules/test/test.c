#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>

void module_end();

void* (*search)(char*) = NULL;
void* module_start(void* search_func)
{
	search = (void* (*)(char*)) search_func;
	void (*uart_printf)(const char*,...) = search("uart_printf");
	if (uart_printf != NULL)
	{
		uart_printf("\nHello from test module!\n\n");
	}
	
	
	
	
	return &module_end;
}




void module_end()
{
	
	
	
	
	
}









