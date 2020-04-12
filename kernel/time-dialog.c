#include "kernel.h"







void set_time_dialog()
{
	
	uint32_t selected = 0;
	//int year = 2020, month = 4,day = 10,hour = 0,minute = 0,second = 0;
	int numbers[6];
	numbers[0] = 2020;
	numbers[1] = 1;
	numbers[2] = 1;
	numbers[3] = 0;
	numbers[4] = 0;
	numbers[5] = 0;
	while (! isKeyPressed(KEY_ESCAPE))
	{
		struct touchpad_report rep;
		touchpad_get_report(&rep);
		void *framebuffer = get_back_framebuffer_address();
		framebuffer_fillrect(framebuffer,0,0,320,240,255,255,255);
		if (touchpad_is_arrow(ARROW_UP,&rep))
		{
			switch (selected)
			{
			case 0:
				if (numbers[selected] < 2040)
					numbers[selected]++;
				break;
			case 1:
				if (numbers[selected] < 12)
					numbers[selected]++;
				break;
			case 2:
				if (numbers[selected] < 31)
					numbers[selected]++;
				break;
			case 3:
				if (numbers[selected] < 23)
					numbers[selected]++;
				break;
			case 4:
			case 5:
				if (numbers[selected] < 60)
					numbers[selected]++;
				break;
			}
		}
		if (touchpad_is_arrow(ARROW_DOWN,&rep))
		{
			switch (selected)
			{
			case 0:
				if (numbers[selected] > 1970)
					numbers[selected]--;
				break;
			case 1:
			case 2:
				if (numbers[selected] > 1)
					numbers[selected]--;
				break;
			case 3:
			case 4:
			case 5:
				if (numbers[selected] > 0)
					numbers[selected]--;
				break;
			}
		}
		if (touchpad_is_arrow(ARROW_LEFT,&rep))
		{
			if (selected > 0)
				selected--;
		}
		if (touchpad_is_arrow(ARROW_RIGHT,&rep))
		{
			if (selected < 5)
				selected++;
		}
		char buffer[100];
		k_memset(buffer,'\0',90);
		sprintf_safe(buffer,"y: %d mon: %d d: %d",80,numbers[0],numbers[1],numbers[2]);
		framebuffer_write10pstring_ascii(buffer,framebuffer,320/2-k_strlen(buffer,80)*5,240/2-5,0,0,0,ascii10p);
		
		
		k_memset(buffer,'\0',90);
		sprintf_safe(buffer,"h: %d min: %d s: %d",80,numbers[3],numbers[4],numbers[5]);
		framebuffer_write10pstring_ascii(buffer,framebuffer,320/2-k_strlen(buffer,80)*5,240/2-5+10,0,0,0,ascii10p);
		
		
		switch (selected)
		{
		case 0:
			framebuffer_write10pstring_ascii("y",framebuffer,320/2,0,0,200,0,ascii10p);
			break;
		case 1:
			framebuffer_write10pstring_ascii("mon",framebuffer,320/2,0,0,200,0,ascii10p);
			break;
		case 2:
			framebuffer_write10pstring_ascii("d",framebuffer,320/2,0,0,200,0,ascii10p);
			break;
		case 3:
			framebuffer_write10pstring_ascii("h",framebuffer,320/2,0,0,200,0,ascii10p);
			break;
		case 4:
			framebuffer_write10pstring_ascii("min",framebuffer,320/2,0,0,200,0,ascii10p);
			break;
		case 5:
			framebuffer_write10pstring_ascii("s",framebuffer,320/2,0,0,200,0,ascii10p);
			break;
		}
		
		
		k_memset(buffer,'\0',35);
		sprintf_safe(buffer,"x: %d, y: %d",30,(uint32_t) rep.x,(uint32_t) rep.y);
		framebuffer_write10pstring_ascii(buffer,framebuffer,0,10,255,0,0,ascii10p);
		
		
		
		
		
		
		blitLCDBuffer();
		msleep(70);
	}
	setRTCValue(date2timestamp(numbers[0],numbers[1],numbers[2],numbers[3],numbers[4],numbers[5]));
	keypad_press_release_barrier();
}





