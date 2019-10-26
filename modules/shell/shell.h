#ifndef SHELL_H
#define SHELL_H



/*
typedef struct {
	struct Line* last;
	struct Line* next;
	char* text;
	char len;
} Line;

typedef struct {
	struct Line *first;
	struct Line *last;
} Lines;


typedef struct {
	struct Lines *l;
	
	
} Shell;


struct Line* createLine(char* text);
void freeLine();
void setLineText(struct Line* line,char* text);

*/





static bool firststartshell = true;
void shell()
{
	if (firststartshell)
	{
		firststartshell = false;
		initshell();
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
		
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 184, 200, 222));
		nSDL_DrawString(screen, font, 10, 10, "Hello, world!");
		
		SDL_Flip(screen);
		msleep(10);
	}
	SDL_Quit();
	*/
	
	
	
	
}

void submitcmd(char* cmd)
{
	if (strcmp(cmd,"template") == 0)
	{
		
	}
	
	
}




void initshell()
{
	
	
	
	
	
}

/*
struct Line* createLine(char* text)
{
	typedef struct Line Line;
	Line *line = malloc(sizeof(Line));
	if (text == NULL)
	{
		line->len = 0;
		line->text = NULL;
	}
	else
	{
		line->text = text;
		line->len = strlen(text);
	}
	return line;
}

void setLineText(struct Line* line,char* text)
{
	if (text != NULL)
	{
		line->text = text;
		line->len = strlen(text);
	}
}

void freeLine(struct Line* line)
{
	if (line->len != 0)
	{
		free(line->text);
	}
	free(line);
}
*/















#endif