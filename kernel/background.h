#ifndef BACKGROUND_H
#define BACKGROUND_H



void background_set_image(struct img565* img);
struct img565* background_get_image();
void background_unset_image();

void background_draw_image(void* nspire_framebuffer,void* framebuffer);



void background_set_slideshow(struct img565* list, uint32_t length);
struct img565* background_get_slideshow();
void background_unset_slideshow();
uint32_t background_slideshow_length();

bool background_is_slideshow();



void background_update();



#endif