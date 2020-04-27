#ifndef USB_H
#define USB_H


struct usb_state {
	// I don't think the timers are used
	//uint32_t timer0load;
	//uint32_t timer0ctrl;
	//uint32_t timer1oad;
	//uint32_t timer1ctrl;
	uint32_t bus_config;
	uint32_t device_interface;
	uint32_t cmd;
	uint32_t intenable;
	uint32_t frindex;
	uint32_t periodicbase_devaddr;
	uint32_t asynclist_endpointlist;
	uint32_t tt_bufferstatusctrl;
	uint32_t burstsize;
	uint32_t transmit_pre_bufferpacket;
	uint32_t endpointNAKenable;
	uint32_t portctrl;
	uint32_t OTGctrl;
	uint32_t mode;
	uint32_t endpoint0;
	uint32_t endpoint1;
	uint32_t endpoint2;
	uint32_t endpoint3;
	uint32_t endpoint4;
};


void usb_save_state(struct usb_state *state);
void usb_restore_state(struct usb_state *state);



void usb_print_id_registers();





void usb_reset();

void usb_halt();
void usb_halt_block();
void usb_start();


uint32_t usb_get_mode();
void usb_set_mode(uint32_t mode);



































#endif