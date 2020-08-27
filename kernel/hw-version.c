#include "kernel.h"


static bool hwversion_checked = false;


static uint16_t hwversion = 0;
static uint32_t hwsubversion = 0;


void hwversion_check()
{
	/*
	DEBUGPRINTLN_1("checking hardware version")
	if (! hwversion_checked)
	{
		intoKernelSpaceSaveAddressSpace();
		
		NUC_FILE* f = nuc_fopen("phoenix/manuf.dat","rb");
		DEBUGPRINTLN_1("file opened")
		if (f == NULL)
		{
			hwversion_checked = true;
			restoreAddressSpace();
			DEBUGPRINTLN_1("could not check hardware version")
			return;
		}
		nuc_fseek(f,0x81C,SEEK_SET);
		
		nuc_fread(&hwsubversion,1,sizeof(uint32_t),f);
		DEBUGPRINTLN_1("subversion read")
		
		
		nuc_fseek(f,0x804,SEEK_SET);
		
		nuc_fread(&hwversion,1,sizeof(uint16_t),f);
		nuc_fclose(f);
		
		hwversion_checked = true;
		DEBUGPRINTLN_1("hardware version checked")
		restoreAddressSpace();
	}
	*/
	hwversion_checked = true;
}



/*
	0: not able to read
	1: Nspire CAS
	2: Nspire
	3: Nspire CX CAS
	4: Nspire CX
	5: Nspire CM
	6: Nspire CM CAS
*/
uint32_t hwversion_main()
{
	hwversion_check();
	switch (hwversion)
	{
	case 0xC:
		return 1;
		break;
	case 0xE:
		return 2;
		break;
	case 0xF:
		return 3;
		break;
	case 0x10:
		return 4;
		break;
	case 0x11:
		return 6;
		break;
	case 0x12:
		return 5;
		break;
	}
	return 0;
}

/*
	0: not able to read
	1: CM / CX Napoca
	2: CX CR / HW-J+
	3: CX CR4 / HW-W+
*/
uint32_t hwversion_sub()
{
	hwversion_check();
	switch (hwsubversion)
	{
	case 0x5:
		return 1;
		break;
	case 0x85:
		return 2;
		break;
	case 0x185:
		return 3;
		break;
	}
	return 0;
}




