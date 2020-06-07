
#include <sys/syscall-templates.h>



void _exit(int code)
{
	syscall1(0,code);
	__builtin_unreachable();
}



void _Exit(int code)
{
	_exit(code);
}


void exit(int code)
{
	// TODO call functions registered by atexit
	
	
	// flush all open streams
	
	
	
	_exit(code);
}


