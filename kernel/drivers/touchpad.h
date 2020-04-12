#ifndef TOUCHPAD_H
#define TOUCHPAD_H



enum touchpad_arrow {ARROW_UP = 0, ARROW_RIGHT, ARROW_DOWN, ARROW_LEFT};




struct touchpad_report {
	bool contact;
	uint8_t proximity;
	uint16_t x;
	uint16_t y;
	uint8_t xrel;
	uint8_t yrel;
	bool pressed;
	//uint8_t status;
};


void touchpad_get_report(struct touchpad_report *rep);

/*
bool touchpad_contact();
uint8_t touchpad_proximity();
uint16_t touchpad_x_abs();
uint16_t touchpad_y_abs();
uint8_t touchpad_x_rel();
uint8_t touchpad_y_rel();
bool touchpad_pressed();
*/
uint8_t touchpad_status();
uint32_t touchpad_firmware();

uint16_t touchpad_max_x();
uint16_t touchpad_max_y();


bool touchpad_is_arrow(enum touchpad_arrow arrow,struct touchpad_report *rep);







#endif