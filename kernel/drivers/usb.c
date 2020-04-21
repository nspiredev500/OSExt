#include "../kernel.h"


static volatile uint32_t *remapped_usb = (volatile uint32_t*) 0xe9100000;


/*
	id registers:
	remapped_usb[0] = id/version register
	remapped_usb[1] = general hardware parameters
	remapped_usb[2] = host hardware parameters
	remapped_usb[3] = device hardware parameters
	remapped_usb[4] = tx buffer hardware parameters
	remapped_usb[5] = rx buffer hardware parameters
	
	misc:
	remapped_usb[32-5] = timer registers
	remapped_usb[36] = system bus configuration register
	
	
	ehci capabilities:
	remapped_usb[64] = capability length and EHCI version
	remapped_usb[65] = host controller structural parameters
	remapped_usb[66] = host control capability parameters
	
	remapped_usb[72] = device interface version number
	remapped_usb[73] = device control capability parameters
	
	
	
	
	
	operation registers:
	remapped_usb[80] = usb command
	remapped_usb[81] = usb status
	remapped_usb[82] = usb interrupt enable
	remapped_usb[83] = usb frame index
	remapped_usb[85] = frame list base address (host mode)
	remapped_usb[85] = usb device address register (device mode)
	remapped_usb[86] = async list address (host mode)
	remapped_usb[86] = endpoint list address (device mode)
	remapped_usb[87] = embedded tt control (host mode)
	remapped_usb[88] = programmable burst size
	remapped_usb[89] = host transmit pre-buffer packet timing
	remapped_usb[91] = inter-chip control register
	remapped_usb[94] = endpoint NAK register
	remapped_usb[95] = endpoint NAK enable
	remapped_usb[97] = port control 1
	remapped_usb[105] = OTG status and control
	remapped_usb[106] = usb mode
	remapped_usb[107] = endpoint setup status
	remapped_usb[108] = endpoint prime register
	remapped_usb[109] = endpoint flush register
	remapped_usb[110] = endpoint status register
	remapped_usb[111] = endpoint complete register
	remapped_usb[112] = endpoint control 0
	remapped_usb[113] = endpoint control 1
	remapped_usb[114] = endpoint control 2
	remapped_usb[115] = endpoint control 3
	remapped_usb[116] = endpoint control 4
*/

































































