#include "../kernel.h"

static volatile uint32_t *keypad_control = (volatile uint32_t*) 0x900E0000;
static volatile uint32_t *keypad_status = (volatile uint32_t*) 0x900E0008;
static volatile uint16_t *keypad_data = (volatile uint16_t*) 0x900E0010;



// ascii chars for the keycodes in the keycode enum; '\n' means there is no such char in ascii
char keycode_ascii[] = {
	'\0', '\n', '\0', ' ', 'z', 'y', '0', '?', '\0',
	'x', 'w', 'v', '3', 'u', 't', 's', '1', '\0','\0','\0',
	'r', 'q', 'p', '6', 'o', 'n', 'm', '4', '\0','\0',
	'l', 'k', 'j', '9', 'i', 'h', 'g', '7', '/', '\0',
	'f', 'e', 'd', 'c', 'b', 'a', '=', '*', '^',
	'\0', '-', ')', '.', '(', '5', '\0', '\0', 127, '\0',
	'\0', '+', '\0', '2', '\0', '8', 27,'\0',
	'\0', '\0', ','
};


// maps the keycodes to bits
uint8_t keymap_cas[] = {
0,1,3,4,5,6,7,8,9,
16,17,18,19,20,21,22,23,24,25,26,
32,33,34,35,36,37,38,39,40,41,
48,49,50,51,52,53,54,55,56,57,
64,65,67,68,69,70,71,72,73,
81,82,83,84,85,86,87,88,89,90,
96,98,99,100,101,102,103,105,
120,121,122};


// doesn't return special characters like return ans the special calculator buttons
char getAlphanumericKeyPressed()
{
	for (uint32_t i = 0;i<sizeof(keycode_ascii);i++)
	{
		enum keycode key = i;
		if (key != KEY_RET && key != KEY_ENTER && key != KEY_MATH_MINUS && key != KEY_ON && key != KEY_PI && key != KEY_TRIG && key != KEY_TEN_POW &&
			key != KEY_EE && key != KEY_SQUARED && key != KEY_E_POW && key != KEY_VAR && key != KEY_CAT && key != KEY_FRAC && key != KEY_DEL &&
			key != KEY_SCRATCH && key != KEY_FLAG && key != KEY_DOC && key != KEY_MENU && key != KEY_ESCAPE && key != KEY_TAB && key != KEY_SHIFT && key != KEY_CTRL)
		{
			if (isKeyPressed(key))
			{
				return keycode_ascii[i];
			}
		}
	}
	return '\0';
}

bool isKeyPressed(enum keycode key)
{
	uint32_t bit = keymap_cas[key];
	//DEBUGPRINTLN_1("keycode: %d\nbit: %d",key,keymap_cas[key]);
	uint32_t uint32 = bit / 16;
	bit = bit % 16;
	//DEBUGPRINTLN_1("uint32: %d",keymap_cas[key] / 16);
	if (uint32 <= 0x1f)
	{
		if ((*(keypad_data+(uint32)) & (0b1 << bit)) != 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}


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













