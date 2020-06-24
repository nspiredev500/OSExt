#include "../kernel.h"







// for drivers that need it, remap the io
void init_drivers_remapped_io()
{
	remappLCD((void*) (0xe9000000+2*SECTION_SIZE));
	remappUART((void*) (0xe9000000+0x00020000));
}



// runs init methods of drivers that need it
void init_drivers()
{
	initLCDDriver();
	//systime_init();
	
	
	
	
	
	
	
	
	
	
	
	
}


























