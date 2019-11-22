#ifndef CURSOR_H
#define CURSOR_H

#include "../../apis/graphics.h"

int tpadw = -1,tpadh = -1;;


int cursorxsetting = -2,cursorysetting = -2;

int cursorx = 160;
int cursory = 120;
//static bool pressed = false;
int notpressedfor = 0;


int cursorlastpressedtime = 10;

static void checkbounds()
{
	if (cursorx<0)
		cursorx = 0;
	if (cursorx>320)
		cursorx = 320;
	if (cursory<0)
		cursory = 0;
	if (cursory>240)
		cursory = 240;
}
int lastpsetting = -2;
static int lastx = -1,lasty = -1;
void checkPad(int deltams)
{
	touchpad_report_t report;
	touchpad_scan(&report);
	if (tpadw == -1 && tpadh == -1)
	{
		touchpad_info_t *inf = touchpad_getinfo();
		tpadw = inf->width;
		tpadh = inf->height;
	}
	if (! report.contact)
	{
		if (lastpsetting == -2)
		{
			#ifdef MODULE_SETTINGS
				lastpsetting = getSetting("lastpress");
			#else
				lastpsetting = -3;
			#endif
		}
		if (lastpsetting > -1)
		{
			cursorlastpressedtime = IntSetting(lastpsetting);
		}
		if (notpressedfor>cursorlastpressedtime)
		{
			lastx = -1;
			lasty = -1;
		}
		else
		{
			notpressedfor += deltams;
		}
		return;
	}
	notpressedfor = 0;
	if (lastx == -1 && lasty == -1)
	{
		lastx = report.x;
		lasty = report.y;
		return;
	}
	if (cursorxsetting == -2)
	{
		#ifndef MODULE_SETTINGS
			cursorxsetting = -3;
		#else
			cursorxsetting = getSetting("cursorx");
			cursorysetting = getSetting("cursory");
		#endif
	}
	int cursorxmul = 25,cursorymul = 25;
	if (cursorxsetting > -1 && cursorysetting > -1)
	{
		cursorxmul = IntSetting(cursorxsetting);
		cursorymul = IntSetting(cursorysetting);
	}
	cursorx += (int) ((((double)(report.x-lastx)/tpadw)*15)*cursorxmul);
	cursory += (int) ((((double)(lasty-report.y)/tpadh)*15)*cursorymul);
	lastx = report.x;
	lasty = report.y;
	checkbounds();
}

void drawCursor(Graphics *g)
{
	setGraphicsColor(g,0,0,0);
	drawLine(g,cursorx,cursory,cursorx+5,cursory);
	drawLine(g,cursorx,cursory,cursorx,cursory+5);
	drawLine(g,cursorx,cursory,cursorx+10,cursory+10);
}















#endif