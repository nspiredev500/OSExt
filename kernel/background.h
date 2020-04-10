#ifndef BACKGROUND_H
#define BACKGROUND_H



void background_set_image(struct img565* img);
void background_unset_image();

void background_draw_image(void* nspire_framebuffer,void* framebuffer);
















#endif