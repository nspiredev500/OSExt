#include "kernel.h"







struct img565* create_img565(uint16_t width,uint16_t height)
{
	struct img565 *img = kmalloc(sizeof(struct img565));
	if (img == NULL)
	{
		return NULL;
	}
	img->width = width;
	img->height = height;
	img->data = kmalloc(width*height*2);
	if (img->data == NULL)
	{
		kfree(img);
		return NULL;
	}
	return img;
}


void destroy_img565(struct img565* img)
{
	if (img != NULL)
	{
		kfree(img->data);
		kfree(img);
	}
}










