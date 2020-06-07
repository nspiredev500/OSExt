#ifndef CHARSET_H
#define CHARSET_H


void charset_load();

bool isDigitPixel(uint32_t x,uint32_t y,uint32_t digit);
bool isCharPixel(uint32_t x,uint32_t y, uint8_t character);


bool isAsciiPixel(uint32_t x,uint32_t y,uint32_t index);









#endif