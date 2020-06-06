#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>

#define OSEXT_VERSION 0x00000004



#include "../module.h"

#include "bmp.h"
#include "img565.h"
#include "nspire_menus.h"
#include "background_module.h"






struct img565* create_img565(uint16_t width,uint16_t height)
{
	struct img565 *img = kmalloc(sizeof(struct img565));
	if (img == NULL)
	{
		return NULL;
	}
	img->width = width;
	img->height = height;
	if (width*height*2 > 1024)
		img->data = (uint16_t*) (useConsecutivePages(((width*height*2)/SMALL_PAGE_SIZE)+1,0)-old_RAM+remapped_RAM);
	else
		img->data = kmalloc(width*height*2);
	if (img->data == NULL)
	{
		
		return NULL;
	}
	return img;
}


void destroy_img565(struct img565* img)
{
	if (img != NULL)
	{
		if (img->width*img->height*2 > 1024)
			freeConsecutivePages((void*) ((void*) img->data-remapped_RAM+old_RAM),((img->width*img->height*2)/SMALL_PAGE_SIZE)+1);
		else
			kfree(img->data);
		kfree(img);
	}
}










