#ifndef TOUCHPAD_H
#define TOUCHPAD_H



enum toucpad_arrow {ARROW_UP = 0, ARROW_RIGHT, ARROW_DOWN, ARROW_LEFT};


bool touchpad_contact();
uint8_t touchpad_proximity();
uint16_t touchpad_x_abs();
uint16_t touchpad_y_abs();
uint8_t touchpad_x_rel();
uint8_t touchpad_y_rel();
bool touchpad_pressed();
uint8_t touchpad_status();
uint32_t touchpad_firmware();

uint16_t touchpad_max_x();
uint16_t touchpad_max_y();


bool touchpad_is_arrow(enum toucpad_arrow arrow);










#endif