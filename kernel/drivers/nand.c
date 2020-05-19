#include "../kernel.h"

/* 
	it seems the NAND-controller is at 0x8FFF1000
	and the nand itself is at 0x81000000
	
	
	
	
	
	
	
*/


static volatile uint32_t *remapped_nand_ctl = (uint32_t*)  (0xe95f1000);
static volatile uint32_t *remapped_nand = (uint32_t*)  (0xe9600000);













































