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
	
	
	
	
	
	
	
	return &module_end;
}




void module_end()
{
	
	
	
	
	
}









