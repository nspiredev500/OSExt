#ifndef KEYPAD_H
#define KEYPAD_H



void keypad_press_release_barrier();
bool any_key_pressed();

// define the keycodes
enum keycode {KEY_RET = 0, KEY_ENTER, KEY_MATH_MINUS, KEY_SPACE, KEY_Z, KEY_Y, KEY_ZERO, KEY_QUESTION, KEY_ON,
			  KEY_X, KEY_W, KEY_V, KEY_THREE, KEY_U, KEY_T, KEY_S, KEY_ONE, KEY_PI, KEY_TRIG, KEY_TEN_POW,
			  KEY_R, KEY_Q, KEY_P, KEY_SIX, KEY_O, KEY_N, KEY_M, KEY_4, KEY_EE, KEY_SQUARED,
			  KEY_L, KEY_K, KEY_J, KEY_NINE, KEY_I, KEY_H, KEY_G, KEY_SEVEN, KEY_SLASH, KEY_E_POW,
			  KEY_F, KEY_E, KEY_D, KEY_C, KEY_B, KEY_A, KEY_EQUALS, KEY_STAR, KEY_CIRCUMFLEX,
			  KEY_VAR, KEY_MINUS, KEY_CLOSING_BRACKET, KEY_DOT, KEY_OPENING_BRACKET, KEY_FIVE, KEY_CAT, KEY_FRAC, KEY_DEL, KEY_SCRATCH,
			  KEY_FLAG, KEY_PLUS, KEY_DOC, KEY_TWO, KEY_MENU, KEY_EIGHT, KEY_ESCAPE, KEY_TAB,
			  KEY_SHIFT, KEY_CTRL, KEY_COMMA};
//

char getAlphanumericKeyPressed();
bool isKeyPressed(enum keycode key);

void keypad_no_key_pressed_barrier(); // waits until no key is pressed



#endif