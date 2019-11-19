
#include <libndls.h>
#include "include/osext.h"







void test()
{
	bkpt();
}




int main(int argsn, char *argc)
{
	assertOSExt();
	initRecursiveDynlinker();
	
	
	registerLibrary_r("testlib",test);
	
	nl_set_resident();
	
	return 0;
}














