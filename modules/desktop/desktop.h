#ifndef DESKTOP_H
#define DESKTOP_H
#include "../../apis/digits.h"
#include "../../apis/chars.h"
#include "../../apis/graphics.h"
#include "../../apis/rawdraw.h"
#include "../../apis/keys.h"


#ifdef MODULE_SETTINGS
static void settingsscreen();
#endif
static void reloadcolor();



/*
static struct
{
	
	
} Task;

static struct
{
	
	
	
} Taskbar;
*/



Graphics *desktopg;

static int r,g,b;

static bool firststartdekstop = true;
void desktop()
{
	if (firststartdekstop)
	{
		firststartdekstop = false;
		initdesktop();
	}
	
	initGraphics(desktopg);
	while (true)
	{
		if (isKeyPressed(KEY_NSPIRE_ESC))
		{
			break;
		}
		#ifdef MODULE_SETTINGS
		if (isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_S))
		{
			settingsscreen();
		}
		#endif
		setGraphicsColor(desktopg,r,g,b);
		fillRect(desktopg,0,0,320,240);
		
		
		
		setGraphicsColor(desktopg,192,192,192);
		fillRect(desktopg,0,240-20,320,20);
		
		
		
		
		
		
		
		blitGraphicsToScreen(desktopg);
		msleep(20);
	}
	exitGraphics(desktopg);
	
	
}
#ifdef MODULE_SETTINGS
void settingsscreen()
{
	int selected = 0;
	char buffer[32];
	int stringindex = 0;
	bool edit = false;
	int yoff = 0;
	while (true)
	{
		if (edit)
		{
			char key = firstAlphanumericKeyPressed();
			if (key != '\0')
			{
				if (stringindex < 29)
				{
					buffer[stringindex] = key;
					stringindex++;
				}
			}
			if (isKeyPressed(KEY_NSPIRE_DEL))
			{
				if (stringindex > 0)
				{
					buffer[stringindex-1] = '\0';
					stringindex--;
				}
			}
		}
		else
		{
			if (isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_ENTER))
			{
				saveSettings();
				return;
			}
			/*
			if (isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_PI))
			{
				deleteSettings();
				loadSettings(); // makes a memory leak if run more than once after boot
				reloadcolor();
				return;
			}
			*/
			if (isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_PI))
			{
				deleteSettings();
				reloadSettings();
				reloadcolor();
				return;
			}
			if (isKeyPressed(KEY_NSPIRE_B) && ! isKeyPressed(KEY_NSPIRE_SHIFT))
			{
				yoff += 4;
			}
			if (isKeyPressed(KEY_NSPIRE_P) && ! isKeyPressed(KEY_NSPIRE_SHIFT))
			{
				if (yoff > 0)
					yoff -= 4;
			}
			if (isKeyPressed(KEY_NSPIRE_B) && isKeyPressed(KEY_NSPIRE_SHIFT))
			{
				if (selected > 0)
					selected--;
			}
			if (isKeyPressed(KEY_NSPIRE_P) && isKeyPressed(KEY_NSPIRE_SHIFT))
			{
				if (selected < settingssize-1)
					selected++;
			}
		}
		if (isKeyPressed(KEY_NSPIRE_ENTER))
		{
			if (edit)
			{
				edit = false;
				if (settings[selected].type == 0)
				{
					int *d = malloc(sizeof(int));
					int match = sscanf(buffer,"%d",d);
					if (match)
					{
						changeSetting(settings[selected].name,d);
					}
					else
					{
						free(d);
					}
				}
				if (settings[selected].type == 1)
				{
					char *str = calloc(31,sizeof(char));
					memcpy(str,buffer,30);
					changeSetting(settings[selected].name,str);
				}
				if (settings[selected].type == 2)
				{
					double *d = malloc(sizeof(double));
					int match = sscanf(buffer,"%lf",d);
					if (match)
					{
						changeSetting(settings[selected].name,d);
					}
					else
					{
						free(d);
					}
				}
				for (int i = 0;i<30;i++)
				{
					buffer[i] = '\0';
				}
				stringindex = 0;
			}
			else
			{
				edit = true;
			}
		}
		
		
		
		reloadcolor();
		
		setGraphicsColor(desktopg,r,g,b);
		fillRect(desktopg,0,0,320,240);
		
		
		setGraphicsColor(desktopg,0,0,0);
		drawString10p(desktopg,10,yoff,"type");
		drawString10p(desktopg,60,yoff,"name");
		drawString10p(desktopg,160,yoff,"value");
		drawString10p(desktopg,220,yoff,buffer);
		for (int i = 0;i<settingssize;i++)
		{
			setGraphicsColor(desktopg,50,220,50);
			if (edit)
			{
				setGraphicsColor(desktopg,150,220,50);
			}
			if (i == selected)
			{
				fillRect(desktopg,0,10*i+10+yoff,320,10);
			}
			setGraphicsColor(desktopg,0,0,0);
			if (settings[i].type == 0)
			{
				char str[10];
				sprintf(str,"%8d",*((int*)settings[i].data));
				drawString10p(desktopg,10,yoff+10*i+10,"0");
				drawString10p(desktopg,160,yoff+10*i+10,str);
			}
			if (settings[i].type == 1)
			{
				drawString10p(desktopg,10,yoff+10*i+10,"1");
				drawString10p(desktopg,160,yoff+10*i+10,((char*)settings[i].data));
			}
			if (settings[i].type == 2)
			{
				drawString10p(desktopg,10,yoff+10*i+10,"2");
				char str[10];
				sprintf(str,"%8lf",*((double*)settings[i].data));
				drawString10p(desktopg,160,yoff+10*i+10,str);
			}
			drawString10p(desktopg,60,yoff+10*i+10,settings[i].name);
		}
		
		
		
		
		blitGraphicsToScreen(desktopg);
		msleep(60);
	}
	
	
	
}
#endif

static void reloadcolor()
{
	#ifdef MODULE_SETTINGS
		int rr = getSetting("r");
		int gg = getSetting("g");
		int bb = getSetting("b");
		if (rr == -1 || gg == -1 || bb == -1)
		{ // 102,205,170
			r = 102;
			g = 205;
			b = 170;
		}
		else
		{
			r = *((int*)(settings[rr].data));
			g = *((int*)(settings[gg].data));
			b = *((int*)(settings[bb].data));
		}
	#else
		r = 102;
		g = 205;
		b = 170;
	#endif
}





void initdesktop()
{
	reloadcolor();
	desktopg = createGraphics();
}

#endif