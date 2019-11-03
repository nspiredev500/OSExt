#ifndef SECURITY_H
#define SECURITY_H
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../../apis/digits.h"
#include "../../apis/chars.h"
#include "../../apis/graphics.h"
#include "../../apis/rawdraw.h"
#include "../../apis/keys.h"


//volatile unsigned int *contrast = (unsigned int*) 0x900F0020;

static time_t lastkeypressed;
static char password[32];
static char inputbuffer[31];
static bool pttunlocked = false;
static bool loginnext = false;
static int locktime = 120;
static Graphics *loging;
void initlogin()
{
	lastkeypressed = time(NULL);
	int pwdsn = getSetting("password");
	if (pwdsn != -1)
	{
		char *pwd = StringSetting(pwdsn);
		password[31] = '\0';
		memcpy(password,pwd,31);
		printf("password: %s",password);
	}
	else
	{
		printf("password setting not found\n");
		password[0] = '\0';
	}
	int locktsn = getSetting("locktime");
	if (locktsn != -1 && IntSetting(locktsn) >= 30)
	{
		locktime = IntSetting(locktsn);
	}
	
	
	loging = createGraphics();
}
void testScreentime()
{
	touchpad_report_t rep;
	touchpad_scan(&rep);
	time_t ct = time(NULL);
	if (any_key_pressed() || rep.pressed || rep.contact)
	{
		if (loginnext)
		{
			loginScreen();
			loginnext = false;
			ct = time(NULL);
		}
		lastkeypressed = ct;
	}
	if (! loginnext && abs(ct-lastkeypressed) > locktime)
	{
		lockScreen();
	}
}
void lockScreen()
{
	loginnext = true;
	pttunlocked = false;
}
void ptt_pressed()
{
	if (! pttunlocked)
	{
		loginScreen();
		lastkeypressed = time(NULL);
		pttunlocked = true;
		loginnext = false;
	}
}
void loginScreen()
{
	if (strlen(password) == 0)
	{
		return;
	}
	//unsigned int lastcontrast = *contrast;
	//*contrast = 0x174;
	memset(inputbuffer,'\0',31);
	initGraphics();
	while (true)
	{
		setGraphicsColor(loging,255,255,255);
		fillRect(loging,0,0,320,240);
		
		int length = strlen(inputbuffer);
		if (length < 30)
		{
			char in = firstAlphanumericKeyPressed();
			if (in != '\0')
			{
				inputbuffer[length] = in;
				length = strlen(inputbuffer);
			}
		}
		if (isKeyPressed(KEY_NSPIRE_DEL))
		{
			if (length>0)
			{
				inputbuffer[length-1] = '\0';
			}
		}
		
		setGraphicsColor(loging,0,0,0);
		drawString10p(loging,160-5*length,120,inputbuffer);
		
		if (strcmp(inputbuffer,password) == 0)
		{
			break;
		}
		
		blitGraphicsToScreen(loging);
		
		msleep(100);
	}
	//*contrast = lastcontrast;
	exitGraphics();
}











#endif