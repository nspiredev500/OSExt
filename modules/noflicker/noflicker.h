#ifndef NOFLICKER_H
#define NOFLICKER_H

// arm nop: 0xe1a00000

uint32_t __attribute__ ((aligned (8))) lcd_mirror[320*240];

uint32_t __attribute__ ((aligned (8))) lcd_framebuffer[320*240];


uint32_t volatile *real_lcdc = (uint32_t*) 0xe0000000;


uint32_t *buffupreal = NULL;



char *memory_section = NULL;


char *section = NULL;


bool noflicker = false;

//sadly, this also doesn't seem to work on hardware


void enableNoflicker()
{
	if (noflicker)
		return;
	
	if (memory_section == NULL)
	{
		memory_section = calloc(sizeof(char),1024*3);
		if (memory_section == NULL)
			return;
	}
	
	
	
	
	
	uart_printf("memory_section: 0x%x, end: 0x%x\n",memory_section,memory_section+1024*1024*3);
	section = (((uint32_t)memory_section) & (~ 0xfffff))+0x100000;
	uart_printf("section: 0x%x\n",section);
	
	
	// copy lcdc contents
	{
		uint32_t *csect = section;
		uint32_t *clcd = 0xc0000000;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		
		csect += 1;
		clcd += 1;
		// somehow gives "bad_read_word"'s
		//*csect = *clcd;
		csect += 1;
		clcd += 1;
		//*csect = *clcd;
		csect += 1;
		clcd += 1;
		memcpy(section+0x200,0xc0000000+0x200,0x200);
		
		
		// somehow gives "bad_read_word"'s
		//memcpy(section+0x800,0xc0000000+0x800,0x400);
		
		
		/*
		for (int i = 0;i<256;i++)
		{
			*csect = *clcd;
			csect += 1;
			clcd += 1;
		}
		*/
		
		
		csect = (section+0xc00);
		clcd = (0xc0000000+0xc00);
		
		
		// somehow gives "bad_read_word"'s
		//*csect = *clcd;
		csect += 1;
		clcd += 1;
		//*csect = *clcd;
		csect += 1;
		clcd += 1;
		
		//*csect = *clcd;
		csect += 1;
		clcd += 1;
		//*csect = *clcd;
		csect += 1;
		clcd += 1;
		//*csect = *clcd;
		csect += 1;
		clcd += 1;
		//*csect = *clcd;
		csect += 1;
		clcd += 1;
		
		
		csect = (section+0xc20);
		clcd = (0xc0000000+0xc20);
		
		//*csect = *clcd;
		csect += 1;
		clcd += 1;
		
		csect += 1;
		clcd += 1;
		
		//*csect = *clcd;
		csect += 1;
		clcd += 1;
		
		//*csect = *clcd;
		csect += 1;
		clcd += 1;
		
		
		csect = (section+0xfe0);
		clcd = (0xc0000000+0xfe0);
		
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
		*csect = *clcd;
		csect += 1;
		clcd += 1;
	}
	
	
	
	
	
	uint32_t *tt_base;
	asm volatile("mrc p15, 0, %[tt_base], c2, c0, 0" : [tt_base] "=r" (tt_base));
	uart_printf("tt_base: 0x%x\n",tt_base);
	
	
	
	// Map LCDC from 0xC0000000 to real_lcdc
	tt_base[(uint32_t) real_lcdc >> 20] = tt_base[0xC00];
	tt_base[0xC00] = (((uint32_t)section >> 20) << 20) | 0xc12;
	
	//tt_base[0xC00] = tt_base[((uint32_t)section) >> 20];
	
	//tt_base[section >> 20] = ;
	
	
	
	// Flush TLB for 0xC0000000 and real_lcdc
	asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (0xC0000000));
	asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (real_lcdc));
	
	clear_cache();
	
	
	volatile uint32_t *buffup = 0xe0000010;
	buffupreal = *buffup;
	memcpy(lcd_framebuffer,*buffup,320*240*4);
	*buffup = lcd_framebuffer;
	
	
	
	
	noflicker = true;
	
	
	bkpt();
	
}



void blitMirrorToScreen()
{
	
	
	if (noflicker)
	{
		memcpy(lcd_framebuffer,lcd_mirror,320*240*2);
	}
	
	
	
	
}






void disableNoflicker()
{
	if (! noflicker)
		return;
	//
	
	
	// currently not working
	//return;
	
	
	uint32_t *tt_base;
	asm volatile("mrc p15, 0, %[tt_base], c2, c0, 0" : [tt_base] "=r" (tt_base));
	// Map real lcdc at 0xC0000000
	tt_base[0xC00] = 0xC0000C12;
	//tt_base[(uint32_t) real_lcdc >> 20] = 0xe0000C12;
	
	
	
	// Flush TLB for 0xC0000000
	asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (0xC0000000));
	//asm volatile("mcr p15, 0, %[base], c8, c7, 1" :: [base] "r" (real_lcdc));
	
	
	
	
	uart_printf("noflicker disabled\n");
	
	
	noflicker = false;
}











































#endif