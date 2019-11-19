#ifndef CURSOR_H
#define CURSOR_H

#include "../../apis/graphics.h"

int tpadw = -1,tpadh = -1;;


int cursorx = 160;
int cursory = 120;
static bool pressed = false;;

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
static int lastx = -1,lasty = -1;
void checkPad()
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
		lastx = -1;
		lasty = -1;
		return;
	}
	if (lastx == -1 && lasty == -1)
	{
		lastx = report.x;
		lasty = report.y;
		return;
	}
	cursorx += (int) (((double)(report.x-lastx)/tpadw)*300);
	cursory += (int) (((double)(lasty-report.y)/tpadh)*300);
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