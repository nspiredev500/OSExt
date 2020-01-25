#include <os.h>

#include <nspireio2.h>


#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include <stdbool.h>
#include "zehn_loader.h"

int main(int argsn, char **argv)
{
	uart_printf("osext loader\n\n");
	if (argsn > 1 && argv[1] != NULL)
	{
		uart_printf("%s\n",argv[1]);
		NUC_FILE *f = nuc_fopen(argv[1],"rb");
		if (f == NULL)
			return 0xDEAD;
		bool hw;
		void *mem;
		int (*entry)(int, char**);
		if (zehn_load(f,&mem,&entry,&hw) == 0)
		{
			nuc_fclose(f);
			entry(0,NULL);
			return 0;
		}
		else
		{
			nuc_fclose(f);
			return 0xDEAD;
		}
	}
	cfg_register_fileext("ose","osextloader");;
	return 0;
}













