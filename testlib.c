
#include <os.h>

#include "include/osext.h"



void test()
{
	bkpt();
}



int main(int argsn, char *argc)
{
	
	registerLibrary("testlib",test);
	
	nl_set_resident();
	
	return 0;
}














