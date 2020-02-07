#include "kernel.h"

static volatile uint32_t *keypad_control = (volatile uint32_t*) 0x900E0000;
static volatile uint32_t *keypad_status = (volatile uint32_t*) 0x900E0008;
static volatile uint16_t *keypad_data = (volatile uint16_t*) 0x900E0010;


// a key on the keypad has to be pressed and released for this function to complete
void keypad_press_release_barrier()
{
	DEBUGPRINTLN_1("starting scanning")
	while (! any_key_pressed())
	{
		
	}
	DEBUGPRINTLN_1("button pressed")
	while (any_key_pressed())
	{
		
	}
	DEBUGPRINTLN_1("end scanning")
}

bool any_key_pressed()
{
	
	for (volatile uint32_t* data = (volatile uint32_t*) keypad_data;data < (volatile uint32_t*) 0x900E0030;data++)
	{
		if (*data != 0)
			return true;
	}
	
	return false;
}













