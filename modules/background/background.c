#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>

#define OSEXT_VERSION 0x00000004



#include "../module.h"
#include "background_module.h"


#include "bmp.h"
#include "img565.h"
#include "nspire_menus.h"



static const uint32_t slideshow_time = 15; // the time an image is shown before the transition in seconds
static const uint32_t slideshow_transition = 10;

static uint32_t last_changed = 0;

static bool slideshow = false;
static struct img565* image = NULL;
static uint32_t slideshow_length = 0;
static uint32_t slideshow_index = 0;

static uint32_t next_image()
{
	if (slideshow_index >= slideshow_length-1)
	{
		return 0;
	}
	else
	{
		return slideshow_index + 1;
	}
}

void background_set_image(struct img565* img)
{
	image = img;
}

struct img565* background_get_image()
{
	return image;
}

void background_unset_image()
{
	image = NULL;
}

void background_draw_image(void* nspire_framebuffer,void* framebuffer)
{
	if (image == NULL)
	{
		return;
	}
	uint16_t *nspire = nspire_framebuffer;
	bool not_main = false;
	for (uint32_t i = 0;i<MENU_ANY;i++)
	{
		if (menu_open(nspire_framebuffer,i))
		{
			not_main = true;
			break;
		}
	}
	uint16_t black = rgbto565(0,0,0);
	if (! not_main)
	{
		if (! slideshow)
		{
			for (uint32_t x = 0;x<320;x++)
			{
				for (uint32_t y = 23;y<240;y++)
				{
					if (nspire[x*240+y] == black)
					{
						framebuffer_setpixel565(framebuffer,x,y,image->data[x+y*image->width]);
					}
				}
			}
		}
		else
		{
			if (rtc_get_value()-last_changed > slideshow_time)
			{
				slideshow_index = next_image();
				last_changed = rtc_get_value();
				background_draw_image(nspire_framebuffer,framebuffer);
				return;
			}
			else
			{
				struct img565 img = image[slideshow_index];
				for (uint32_t x = 0;x<320;x++)
				{
					for (uint32_t y = 23;y<240;y++)
					{
						if (nspire[x*240+y] == black)
						{
							framebuffer_setpixel565(framebuffer,x,y,img.data[x+y*img.width]);
						}
					}
				}
			}
		}
	}
}



void background_set_slideshow(struct img565* list, uint32_t length)
{
	slideshow = true;
	image = list;
	slideshow_index = 0;
	last_changed = rtc_get_value();
	slideshow_length = length;
}
struct img565* background_get_slideshow()
{
	if (! slideshow)
	{
		return NULL;
	}
	return image;
}


void background_unset_slideshow()
{
	slideshow = false;
	slideshow_index = 0;
	slideshow_length = 0;
	last_changed = 0;
	image = NULL;
}

bool background_is_slideshow()
{
	return slideshow;
}

uint32_t background_slideshow_length()
{
	return slideshow_length;
}








void background_update()
{
	if (background_is_slideshow())
	{
		struct img565* show = background_get_slideshow();
		for (uint32_t i = 0;i<background_slideshow_length();i++)
		{
			destroy_img565(&show[i]);
		}
		background_unset_slideshow();
		kfree(show);
	}
	else
	{
		struct img565* img = background_get_image();
		background_unset_image();
		destroy_img565(img);
	}
	struct img565* img1 = NULL;
	struct img565* img2 = NULL;
	struct img565* img3 = NULL;
	struct img565* img4 = NULL;
	bool bk = false;
	uint32_t length = 0;
	
	
	
	NUC_FILE* f = nuc_fopen("/documents/background.bmp.tns","rb");
	if (f != NULL)
	{
		img1 = load_bmp_file(f);
		nuc_fclose(f);
		if (img1 == NULL)
		{
			bk = true;
		}
	}
	else
	{
		bk = true;
	}
	f = nuc_fopen("/documents/background2.bmp.tns","rb");
	if (f != NULL && ! bk)
	{
		img2 = load_bmp_file(f);
		nuc_fclose(f);
		if (img2 == NULL)
		{
			bk = true;
			length = 1;
		}
	}
	else
	{
		if (! bk)
			length = 1;
		bk = true;
	}
	f = nuc_fopen("/documents/background3.bmp.tns","rb");
	if (f != NULL && ! bk)
	{
		img3 = load_bmp_file(f);
		nuc_fclose(f);
		if (img3 == NULL)
		{
			bk = true;
			length = 2;
		}
	}
	else
	{
		if (! bk)
			length = 2;
		bk = true;
	}
	f = nuc_fopen("/documents/background4.bmp.tns","rb");
	if (f != NULL && ! bk)
	{
		img4 = load_bmp_file(f);
		nuc_fclose(f);
		if (img4 == NULL)
		{
			bk = true;
			length = 3;
		}
	}
	else
	{
		if (! bk)
			length = 3;
		bk = true;
	}
	if (! bk)
	{
		length = 4;
	}
	if (img2 == NULL)
	{
		background_set_image(img1);
	}
	else
	{
		struct img565* array = kmalloc(sizeof(struct img565)*length);
		switch (length)
		{
		case 4:
			array[3] = *img4;
		case 3:
			array[2] = *img3;
		case 2:
			array[1] = *img2;
		}
		array[0] = *img1;
		background_set_slideshow(array,length);
	}
}






