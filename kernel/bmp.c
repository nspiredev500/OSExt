#include "kernel.h"

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
		DEBUGPRINTLN_1("wrong bmp format: 0x%x!",bmp_h.identifier)
		return NULL;
	}
	DEBUGPRINTLN_1("pixel offset: %d",bmp_h.pixel_offset)
	bmp_h.pixel_offset -= 14; // subtract the size of the bmp header here, because we use the offset on the rest of the file
	
	DEBUGPRINTLN_1("file size: %d",bmp_h.size)
	
	void* rest_file = kmalloc(bmp_h.size-14);
	if (rest_file == NULL)
	{
		DEBUGPRINTLN_1("could not allocate memory for the file!")
		return NULL;
	}
	nuc_fread(rest_file,bmp_h.size-14,1,f);
	
	struct dib_header_win dib;
	k_memcpy(&dib,rest_file,sizeof(struct dib_header_win));
	
	
	if (dib.color_planes != 1)
	{
		kfree(rest_file);
		DEBUGPRINTLN_1("color planes != 1! : %d",dib.color_planes)
		return NULL;
	}
	
	DEBUGPRINTLN_1("bpp: %d",dib.bpp)
	if (dib.compression != CMP_NONE)
	{
		kfree(rest_file);
		DEBUGPRINTLN_1("bmp uses compression!")
		return NULL;
	}
	
	
	uint32_t *palette = rest_file+sizeof(struct dib_header_win);
	
	char* pixel_data = rest_file+bmp_h.pixel_offset;
	
	uint32_t width = dib.width;
	uint32_t height = signedtounsigned32(dib.height); // height can be minus if pixels stored from the top down
	struct img565* img = create_img565(width,height);
	
	
	if (img == NULL)
	{
		kfree(rest_file);
		DEBUGPRINTLN_1("could not allocate an img565")
		return NULL;
	}
	
	
	uint16_t *img_data = img->data;
	
	DEBUGPRINTLN_1("width: %d, height: %d",dib.width,dib.height)
	
	
	uint32_t rowsize = (dib.bpp*width)/8;
	if (rowsize % 4 != 0)
	{
		rowsize = rowsize + (4 - rowsize % 4); // round to 4 bytes
	}
	
	DEBUGPRINTLN_1("rowsize: %d",rowsize)
	/*
	int32_t direction;
	int32_t start;
	int32_t compare;
	if (dib.width == 0 || dib.height == 0)
	{
		DEBUGPRINTLN_1("zero width or height image!")
		kfree(rest_file);
		destroy_img565(img);
		return NULL;
	}
	if (dib.height > 0)
	{
		// bottom up
		direction = -1;
		start = height-1;
		compare = -1;
	}
	else
	{
		// top down
		direction = 1;
		start = 0;
		compare = height;
	}
	*/
	
	switch (dib.bpp)
	{
		
	case 24: // blue byte, green byte, red byte
		/*
		for (int32_t y = start;y != compare;y+=direction)
		{
			//DEBUGPRINTLN_1("row: %d",y)
			for (uint32_t x = 0; x<width;x++)
			{
				//DEBUGPRINTLN_1("col: %d",x)
				char *pixel = pixel_data+rowsize*y+x*3;
				DEBUGPRINTLN_1("x: %d, y: %d  r: %d, g:%d, b:%d",x,y,pixel[2],pixel[1],pixel[0])
				if (direction == -1)
					img_data[x+(height-y)*height] = rgbto565(pixel[2],pixel[1],pixel[0]);
				else
					img_data[x+y*height] = rgbto565(pixel[2],pixel[1],pixel[0]);
			}
			
		}
		*/
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
				DEBUGPRINTLN_1("x: %d, y: %d  r: %d, g:%d, b:%d",x,y,pixel[2],pixel[1],pixel[0])
				if (dib.height > 0)
					img_data[x+((height-1)-y)*height] = rgbto565(pixel[2],pixel[1],pixel[0]);
				else
					img_data[x+y*height] = rgbto565(pixel[2],pixel[1],pixel[0]);
			}
		}
		break;
	default:
		DEBUGPRINTLN_1("unsupported bit-per-pixel-value")
		kfree(rest_file);
		destroy_img565(img);
		return NULL;
	}
	
	
	
	
	
	
	
	
	
	
	kfree(rest_file);
	
	
	
	
	
	
	
	return img;
}






















