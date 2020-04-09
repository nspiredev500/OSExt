#include "kernel.h"





static struct img565* image = NULL;


void background_set_image(struct img565* img)
{
	image = img;
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
	for (uint32_t i = 0;i<MENU_LANGUAGE_POPUP;i++)
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
		for (uint32_t x = 0;x<320;x++)
		{
			for (uint32_t y = 0;y<240;y++)
			{
				if (nspire[x*240+y] == black)
				{
					framebuffer_setpixel565(framebuffer,x,y,image->data[x+y*240]);
				}
			}
		}
	}
}





































