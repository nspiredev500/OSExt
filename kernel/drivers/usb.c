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

/*
	the OS doesn't seem to re-initialize the controller after a reset.
	So this can be used to stop usb transactions from or to the calculator.
	
	but unless we can bring it into the state it was, you have to reboot to
	let the OS use it again
	
	or we have to emulate all the navnet services...
	
	
	
	
*/




static const uint32_t USB_HOST = 3;
static const uint32_t USB_DEVICE = 2;




void usb_save_state(struct usb_state *state)
{
	
	power_enable_device(5);
	state->cmd = remapped_usb[80];
	usb_halt_block();
	//state->bus_config = remapped_usb[36] & 0b111;
	//state->device_interface = remapped_usb[72] & 0xffff;
	state->cmd = remapped_usb[80];
	state->intenable = remapped_usb[82];
	//state->frindex = remapped_usb[83];
	state->periodicbase_devaddr = remapped_usb[85];
	state->asynclist_endpointlist = remapped_usb[86];
	//state->tt_bufferstatusctrl = remapped_usb[87];
	//state->burstsize = remapped_usb[88];
	//state->transmit_pre_bufferpacket = remapped_usb[89];
	//state->endpointNAKenable = remapped_usb[95];
	state->portctrl = remapped_usb[97];
	state->OTGctrl = remapped_usb[105];
	state->mode = remapped_usb[106];
	state->endpoint0 = remapped_usb[112];
	state->endpoint1 = remapped_usb[113];
	state->endpoint2 = remapped_usb[114];
	state->endpoint3 = remapped_usb[115];
	//state->endpoint4 = remapped_usb[116];
}


void usb_restore_state(struct usb_state *state)
{
	power_enable_device(5);
	//remapped_usb[36] = state->bus_config & 0b111;
	debug_shell_println("setting mode");
	remapped_usb[106] = state->mode;
	//remapped_usb[72] = state->device_interface & 0xffff;
	debug_shell_println("setting command");
	remapped_usb[80] = state->cmd;
	debug_shell_println("setting intenable");
	remapped_usb[82] = state->intenable;
	//remapped_usb[83] = state->frindex;
	//debug_shell_println("setting periodiclist");
	//remapped_usb[85] = state->periodicbase_devaddr; // freezes here on hardware
	debug_shell_println("setting asynclist");
	remapped_usb[86] = state->asynclist_endpointlist;
	//remapped_usb[87] = state->tt_bufferstatusctrl;
	//remapped_usb[88] = state->burstsize;
	//remapped_usb[89] = state->transmit_pre_bufferpacket;
	//remapped_usb[95] = state->endpointNAKenable;
	//debug_shell_println("setting portctrl");
	//remapped_usb[97] = state->portctrl; // also freezes here
	//debug_shell_println("setting OTGctrl");
	//remapped_usb[105] = state->OTGctrl; // also freezes here
	debug_shell_println("setting endpoint0");
	remapped_usb[112] = state->endpoint0;
	debug_shell_println("setting endpoint1");
	remapped_usb[113] = state->endpoint1;
	debug_shell_println("setting endpoint2");
	remapped_usb[114] = state->endpoint2;
	debug_shell_println("setting endpoint3");
	remapped_usb[115] = state->endpoint3;
	//remapped_usb[116] = state->endpoint4;
}







/*
	reported registers:
	id: 0xe241fa05
	hardware param: 0x15
	host param: 0x10020001
	device param: 0x9
	tx param: 0x80050704
	rx param: 0x504
*/
void usb_print_id_registers()
{
	power_enable_device(5);
	debug_shell_println("usb registers:");
	debug_shell_println("id: 0x%x",remapped_usb[0]);
	debug_shell_println("hardware param: 0x%x",remapped_usb[1]);
	debug_shell_println("host param: 0x%x",remapped_usb[2]);
	debug_shell_println("device param: 0x%x",remapped_usb[3]);
	debug_shell_println("tx param: 0x%x",remapped_usb[4]);
	debug_shell_println("rx param: 0x%x",remapped_usb[5]);
	debug_shell_println("end usb info");
}

// in host mode, halts the controller and resets it
// in device mode, sends a detach event and resets
void usb_reset()
{
	power_enable_device(5);
	usb_halt_block();
	remapped_usb[80] |= 0b1 << 1;
	while (((remapped_usb[80] >> 1) & 0b1) == 1) {}
}

// in host mode, halts the controller
// in device mode, sends a detach event
void usb_halt()
{
	power_enable_device(5);
	remapped_usb[81] &= ~0b1;
}


void usb_halt_block()
{
	power_enable_device(5);
	usb_halt();
	if ((remapped_usb[106] & 0b11) == 3)
	{
		while (((remapped_usb[81] >> 12) & 0b1) == 0b0) {}
	}
}

// in host mode, starts the controller
// in device mode, sends an attach event
void usb_start()
{
	power_enable_device(5);
	remapped_usb[81] |= 0b1;
}



// modes: 0 = idle, 2 = device, 3 = host 
uint32_t usb_get_mode()
{
	power_enable_device(5);
	return remapped_usb[106] & 0b11;
}

void usb_set_mode(uint32_t mode)
{
	power_enable_device(5);
	remapped_usb[106] &= ~0b11;
	remapped_usb[106] |= mode & 0b11;
}


void usb_reset_init_host()
{
	usb_reset();
	usb_set_mode(USB_HOST);
	usb_start();
}




struct usb_qTD* usb_create_transfer(uint8_t data_toggle,uint16_t total_bytes,uint8_t ioc,uint8_t pid, uint16_t first_offset, void* page0, void* page1, void* page2, void* page3, void* page4)
{
	struct usb_qTD* transfer = kmalloc(32); // has to be 32 byte aligned anyways
	if (transfer == NULL)
	{
		return NULL;
	}
	k_memset(transfer,0,32);
	transfer->nextqTD = 1; // invalid pointer
	transfer->altnextqTD = 1; // invalid pointer
	transfer->qTD_token = (data_toggle << 31) | ((total_bytes && 0x7fff) << 16) | ((ioc & 0b1) << 15) | ((pid & 0b11) << 8) | (1 << 7);
	transfer->buffers[0] = ((uint32_t) page0 & (~ 0xfff)) | (first_offset & 0xfff);
	transfer->buffers[1] = ((uint32_t) page1 & (~ 0xfff));
	transfer->buffers[2] = ((uint32_t) page2 & (~ 0xfff));
	transfer->buffers[3] = ((uint32_t) page3 & (~ 0xfff));
	transfer->buffers[4] = ((uint32_t) page4 & (~ 0xfff));
	return transfer;
}



struct usb_QH* usb_create_queue_head(uint8_t nak_reload, uint8_t control_endpoint, uint16_t max_packet_size, uint8_t data_toggle_control, uint8_t endpoint_speed, uint8_t endpoint_number,
									 uint8_t device_address,uint8_t mult, uint8_t port, uint8_t hub, uint8_t split_mask, uint8_t interrupt_mask)
{
	struct usb_QH* queue_head = kmalloc(32);
	if (queue_head == NULL)
	{
		return NULL;
	}
	k_memset(queue_head,0,32);
	queue_head->horizontal_link = 0b1 << 1; // next thing is also a queue head
	queue_head->endpt_cap1 = ((nak_reload & 0xf) << 28) | ((control_endpoint & 0b1) << 27) | ((max_packet_size & 0x7ff) << 16) | ((data_toggle_control & 0b1) << 14) | ((endpoint_speed & 0b11) << 12) | ((endpoint_number & 0xf) << 8) | (device_address & 0x7f);
	queue_head->endpt_cap2 = ((mult & 0b11) << 30) | ((port & 0x7f) << 23) | ((hub & 0x7f) << 16) | (split_mask << 8) | interrupt_mask;
	
	queue_head->overlay.nextqTD = 1; // invalid pointer
	queue_head->overlay.altnextqTD = 1; // invalid pointer
	
	
	return queue_head;
}








void usb_qh_add_transfer(struct usb_QH* queue,struct usb_qTD *transfer)
{
	
	
	
	
}


// also frees it
void usb_qh_remove_transfer(struct usb_QH* queue,struct usb_qTD *transfer)
{
	// only remove if active bit is 0 and the current transfer descriptor pointer of the queue head doesn't point to it
	
	
	
	
}


void usb_add_queue_head(struct usb_QH* queue)
{
	
	
	
	
	
}

// also frees it
void usb_remove_queue_head(struct usb_QH* queue)
{
	
	
	
	
	
}


void usb_control_transfer_blocking(uint8_t device_addr, uint8_t endpt_num,bool in,void* data, uint16_t size)
{
	
	
	
	
}






































