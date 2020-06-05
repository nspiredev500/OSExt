#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>


#define OSEXT_VERSION 0x00000004

void module_end();

void* (*search)(char*) = NULL;
void* module_start(void* search_func, uint32_t osext_version, bool standalone)
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









