#ifndef DESKTOP_H
#define DESKTOP_H
/*
static struct
{
	
	
} Task;

static struct
{
	
	
	
} Taskbar;
*/




static bool firststartdekstop = true;
void desktop()
{
	if (firststartdekstop)
	{
		firststartdekstop = false;
		initdesktop();
	}
	
	
	
	
	
	
	// SDL is somehow not working in hardware
	/*
	SDL_Surface *screen;
	nSDL_Font *font;
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(320, 240, has_colors ? 16 : 8, SDL_SWSURFACE);
	font = nSDL_LoadFont(NSDL_FONT_TINYTYPE, 29, 43, 61);
	while (true)
	{
		if (isKeyPressed(KEY_NSPIRE_ESC))
		{
			break;
		}
		
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 200, 222));
		nSDL_DrawString(screen, font, 10, 10, "Hello, world!");
		
		SDL_Flip(screen);
		msleep(10);
	}
	SDL_Quit();
	*/
	
	
}











void initdesktop()
{
	
	
	
	
	
	
}

#endif