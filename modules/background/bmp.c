#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>


#define OSEXT_VERSION 0x00000004


#include "../module.h"
#include "background_module.h"
#include "img565.h"

enum bmp_type {BMP_WIN = 0x4D42};

struct bmp_header {
	uint16_t identifier; // 0x42 0x4D for windows
	uint32_t size; // size of the file in bytes
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t pixel_offset; // offset from the start of the file to the pixel data
};

enum win_compression {CMP_NONE = 0};

struct dib_header_win {
	uint32_t size; // size of the header in bytes
	int32_t width;
	int32_t height;
	uint16_t color_planes; // must be 1
	uint16_t bpp; // bits per pixel
	uint32_t compression;
	uint32_t img_size; // size of the raw bitmap data
	uint32_t horiz_resolution; // ?
	uint32_t vert_resolution; // ?
	uint32_t colors; // number of colors in the palette
	uint32_t improtant_colors; // generally ignored
}; // this all follows the alignment rules for half-word and word specified by arm
// so it should be in memory without any padding and we can just memcpy it



struct img565* load_bmp_file(NUC_FILE* f)
{
	
	
	struct bmp_header bmp_h;
	//nuc_fread(&bmp_h.,sizeof(),1,f);
	nuc_fread(&bmp_h.identifier,2,1,f);
	nuc_fread(&bmp_h.size,4,1,f);
	nuc_fread(&bmp_h.reserved1,2,1,f);
	nuc_fread(&bmp_h.reserved2,2,1,f);
	nuc_fread(&bmp_h.pixel_offset,4,1,f);
	
	if (bmp_h.identifier != BMP_WIN)
	{
		return NULL;
	}
	bmp_h.pixel_offset -= 14; // subtract the size of the bmp header here, because we use the offset on the rest of the file
	
	
	bool ti = false;
	void* ti_raw = NULL;
	
	uint32_t pages = (bmp_h.size/SMALL_PAGE_SIZE)+1;
	void* rest_file = useConsecutivePages(pages,0);
	if (rest_file == NULL)
	{
		return NULL;
	}
	nuc_fread(rest_file,bmp_h.size-14,1,f);
	
	struct dib_header_win dib;
	k_memcpy(&dib,rest_file,sizeof(struct dib_header_win));
	
	
	if (dib.color_planes != 1)
	{
		freeConsecutivePages(rest_file,pages);
		return NULL;
	}
	
	if (dib.compression != CMP_NONE)
	{
		freeConsecutivePages(rest_file,pages);
		return NULL;
	}
	
	
	uint32_t *palette = rest_file+sizeof(struct dib_header_win);
	
	char* pixel_data = rest_file+bmp_h.pixel_offset;
	
	uint32_t width = dib.width;
	uint32_t height = signedtounsigned32(dib.height); // height can be minus if pixels stored from the top down
	struct img565* img = create_img565(width,height);
	
	
	if (img == NULL)
	{
		freeConsecutivePages(rest_file,pages);
		return NULL;
	}
	
	
	uint16_t *img_data = img->data;
	
	
	
	uint32_t rowsize = (dib.bpp*width)/8;
	if (rowsize % 4 != 0)
	{
		rowsize = rowsize + (4 - rowsize % 4); // round to 4 bytes
	}
	
	
	switch (dib.bpp)
	{
		
	case 24: // blue byte, green byte, red byte
		for (uint32_t y = 0;y<height;y++)
		{
			/*
				blue byte: pixel_data[rowsize*y+x*3]
				green byte: pixel_data[rowsize*y+x*3+1]
				red byte: pixel_data[rowsize*y+x*3+2]
			*/
			for (uint32_t x = 0;x<width;x++)
			{
				char *pixel = pixel_data+rowsize*y+x*3;
				//DEBUGPRINTLN_1("x: %d, y: %d  r: %d, g:%d, b:%d",x,y,pixel[2],pixel[1],pixel[0])
				if (dib.height > 0)
					img_data[x+((height-1)-y)*width] = rgbto565(pixel[2],pixel[1],pixel[0]);
				else
					img_data[x+y*width] = rgbto565(pixel[2],pixel[1],pixel[0]);
			}
		}
		break;
	default:
		freeConsecutivePages(rest_file,pages);
		destroy_img565(img);
		return NULL;
	}
	
	
	
	
	freeConsecutivePages(rest_file,pages);
	
	
	return img;
}






















