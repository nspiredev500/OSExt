#ifndef USB_H
#define USB_H


static const uint32_t USB_HOST;
static const uint32_t USB_DEVICE;

// should be 32 byte aligned
struct usb_qTD {
	volatile uint32_t nextqTD; // also the terminate bit
	volatile uint32_t altnextqTD; // alternate next pointer
	volatile uint32_t qTD_token; // total bytes to transfer, data toggle, interrupt on complete, current page, error counter,
	// packet id code, status,
	volatile uint32_t buffers[5];
};

// should be 32 byte aligned
struct usb_QH {
	volatile uint32_t horizontal_link; // + type of item (always type QH in this case) + terminate bit
	volatile uint32_t endpt_cap1;
	volatile uint32_t endpt_cap2;
	volatile uint32_t current_qTD_pointer;
	volatile struct usb_qTD overlay;
};


struct usb_device_descriptor {
	volatile uint8_t lenght; // 18 bytes
	volatile uint8_t type; // 0x01 for device descriptor
	volatile uint16_t bcd_usb; // usb version in binary coded decimal
	volatile uint8_t device_class;
	volatile uint8_t device_subclass;
	volatile uint8_t device_protocol;
	volatile uint8_t max_packsize;
	volatile uint16_t idvendor;
	volatile uint16_t idproduct;
	volatile uint16_t bcd_device; // device release number in binary coded decimal
	volatile uint8_t imanufacturer;
	volatile uint8_t iproduct;
	volatile uint8_t iserial_number;
	volatile uint8_t num_configurations;
};




struct usb_configuration_descriptor {
	volatile uint8_t lenght;
	volatile uint8_t type; // 0x02 for configuration descriptor
	volatile uint16_t total_length;
	volatile uint8_t num_interfaces;
	volatile uint8_t configuration_value;
	volatile uint8_t iconfiguration;
	volatile uint8_t attributes;
	volatile uint8_t maxpower; // in 2mA
};



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


struct usb_device;
struct usb_device {
	struct usb_device *next;
	volatile struct usb_device_descriptor desc;
	
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


void usb_reset_init_host();



struct usb_qTD* usb_create_transfer(uint8_t data_toggle,uint16_t total_bytes,uint8_t ioc,uint8_t pid, uint16_t first_offset, void* page0, void* page1, void* page2, void* page3, void* page4);
struct usb_QH* usb_create_queue_head(uint8_t nak_reload, uint8_t control_endpoint, uint16_t max_packet_size, uint8_t data_toggle_control, uint8_t endpoint_speed, uint8_t endpoint_number,
									 uint8_t device_address,uint8_t mult, uint8_t port, uint8_t hub, uint8_t split_mask, uint8_t interrupt_mask);
//




void usb_qh_add_transfer(struct usb_QH*,struct usb_qTD *transfer);

void usb_qh_remove_transfer(struct usb_QH*,struct usb_qTD *transfer); // also frees it


void usb_add_queue_head(struct usb_QH*);
void usb_remove_queue_head(struct usb_QH*); // also frees it


void usb_control_transfer_blocking(uint8_t device_addr, uint8_t endpt_num,bool in,void* data, uint16_t size);























#endif