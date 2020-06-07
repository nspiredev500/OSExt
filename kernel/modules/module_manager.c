#include "../kernel.h"










void module_manager_show()
{
	uint32_t selected = 0;
	uint32_t yoffset = 0;
	char buffer[100];
	k_memset(buffer,'\0',100);
	
	while (! isKeyPressed(KEY_ESCAPE))
	{
		void *framebuffer = get_back_framebuffer_address();
		framebuffer_fillrect(framebuffer,0,0,320,240,255,255,255);
		
		struct touchpad_report rep;
		touchpad_get_report(&rep);
		
		uint32_t modules_num = 0;
		const struct module *modules = module_get_modules();
		while (modules != NULL)
		{
			modules_num++;
			modules = modules->next;
		}
		
		if (touchpad_is_arrow(ARROW_UP,&rep))
		{
			if (selected != 0)
			{
				selected--;
			}
		}
		if (touchpad_is_arrow(ARROW_DOWN,&rep))
		{
			if (selected < modules_num)
			{
				selected++;
			}
		}
		
		if (isKeyPressed(KEY_ENTER) && selected == 0)
		{
			
			
			
			
			
			while (isKeyPressed(KEY_ENTER)) {}; // wait until the key has been released
		}
		
		if (isKeyPressed(KEY_PI) && selected != 0)
		{
			modules = module_get_modules();
			uint32_t selcnt = 1;
			while (modules != NULL)
			{
				if (selected == selcnt)
				{
					module_uninstall(modules->name);
					break;
				}
				modules = modules->next;
				selcnt++;
			}
			selected = 0;
			while (isKeyPressed(KEY_PI)) {}; // wait until the key has been released
		}
		
		modules = module_get_modules();
		framebuffer_write10pstring_ascii("Module Manager",framebuffer,320/2-70,yoffset,0,0,0);
		framebuffer_write10pstring_ascii("Memory",framebuffer,0,10+yoffset,0,0,0);
		framebuffer_write10pstring_ascii("Used",framebuffer,10,20+yoffset,0,0,0);
		framebuffer_write10pstring_ascii("Name",framebuffer,320/2-20,10+yoffset,0,0,0);
		
		uint32_t width = k_strlen(buffer,100)*10;
		if (width == 0)
		{
			width = 1;
		}
		if (selected == 0)
		{
			framebuffer_drawrect(framebuffer,320/2-width/2,30+yoffset,width,10,0,255,0);
		}
		uint32_t selcnt = 1;
		while (modules != NULL)
		{
			width = k_strlen(modules->name,100)*10;
			framebuffer_write10pstring_ascii(modules->name,framebuffer,320/2-width/2,30+selcnt*10+yoffset,0,0,0);
			if (selected == selcnt)
			{
				framebuffer_drawrect(framebuffer,320/2-width/2,30+selcnt*10+yoffset,width,10,0,255,0);
			}
			
			
			char buffer2[20];
			k_memset(buffer2,'\0',20);
			sprintf_safe(buffer2,"%dK",10,modules->pages*4);
			framebuffer_write10pstring_ascii(buffer2,framebuffer,0,30+selcnt*10+yoffset,0,0,0);
			
			
			
			
			selcnt++;
			modules = modules->next;
		}
		
		
		
		
		blitLCDBuffer();
		msleep(70);
	}
	keypad_press_release_barrier();
}







