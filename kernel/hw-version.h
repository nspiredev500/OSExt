#ifndef HW_VERSION_H
#define HW_VERSION_H


// doesn't work on hw<W in firebird
void hwversion_check();

/*
	0: not able to read
	1: Nspire CAS
	2: Nspire
	3: Nspire CX CAS
	4: Nspire CX
	5: Nspire CM
	6: Nspire CM CAS
*/
uint32_t hwversion_main();

/*
	0: not able to read
	1: CM / CX Napoca
	2: CX CR / HW-J+
	3: CX CR4 / HW-W+
*/
uint32_t hwversion_sub();













#endif