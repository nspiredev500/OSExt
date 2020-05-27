#include "../kernel.h"

static const char* charset_path = "/documents/charset.tns";


static uint8_t charset_ascii[1600]; // (128*10*10)/8 bytes for the charset


void charset_load()
{
	NUC_FILE *f = nuc_fopen(charset_path,"rb");
	if (f == NULL)
	{
		DEBUGPRINTLN_1("charset not found!");
		return;
	}
	k_memset(charset_ascii,0,sizeof(charset_ascii));
	
	nuc_fread(charset_ascii,1,sizeof(charset_ascii),f);
	
	nuc_fclose(f);
	DEBUGPRINTLN_1("charset loaded: size: %d!",sizeof(charset_ascii));
}

bool isDigitPixel(uint32_t x,uint32_t y,uint32_t digit)
{
	return isAsciiPixel(x,y,digit+48); // ascii offset for digits
}
bool isCharPixel(uint32_t x,uint32_t y, uint8_t character)
{
	return isAsciiPixel(x,y,character+97); // ascii offset for small characters
}


bool isAsciiPixel(uint32_t x,uint32_t y,uint32_t index)
{
	uint32_t bitoffset = index*10*10+10*y+x;
	uint32_t byteoffset = bitoffset/8;
	bitoffset = bitoffset % 8;
	
	//DEBUGPRINTLN_1("character: %d",index);
	//DEBUGPRINTLN_1("byteoffset: %d, bitoffset: %d",byteoffset,bitoffset);
	//DEBUGPRINTLN_1("byte: %d", (uint32_t) charset_ascii[byteoffset]);
	
	
	//BREAKPOINT
	
	
	if (byteoffset < sizeof(charset_ascii))
	{
		if (((charset_ascii[byteoffset] >> bitoffset) & 0b1) == 0b1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}






























