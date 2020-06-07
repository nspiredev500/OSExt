#ifndef IMG565_H
#define IMG565_H



struct img565 {
	uint16_t width;
	uint16_t height;
	uint16_t* data; // pointer to the actual image data
}; // sizeof(img565) = 8 bytes

struct img565* create_img565(uint16_t width,uint16_t height);
void destroy_img565(struct img565* img);









#endif