

#ifdef MODULE_CLOCK
	void settime();
	void drawclock();
	void hook_minicklock();
#endif

#ifdef MODULE_SETTINGS
	void changeSetting(char * name,void *data);
	int getSetting(char *name);
	void loadSettings();
	void saveSettings();
	void defaultSettings();
#endif

#ifdef MODULE_DESKTOP
	void desktop();
	void initdesktop();
#endif

#ifdef MODULE_DISABLENAVNET
	void disablenavnet();
	void disableusb();
	void enableusb();
#endif


#ifdef MODULE_SHELL
	void shell();
	void initshell();
#endif

#ifdef MODULE_USBTEST
	void ums_register(void)
#endif

#ifdef MODULE_SECURITY
	void loginScreen();
	void initlogin();
	void lockScreen();
	void testScreentime();
	void ptt_pressed();
#endif


#ifdef MODULE_DYNLINKER
	typedef struct Library Library;
	struct Library
	{
		char *name;
		void *functiontable;
		Library *next;
	};
	struct Library *libs;
	typedef struct
	{
		char *name;
		char *translation;
	} TranslationEntry;
	
	bool s_addLibrary();
	void* s_requestLibrary();
	bool loadLibrary(char *name);
	Library* searchLibrary(char * name);
	void initDynlinker();
	void addLibraryEntry(char *name, void *functiontable);
	
	
	
#endif


#ifndef MODULE_NOFLICKER
	uint32_t *real_lcdc;
	bool noflicker;
	void enableNoflicker();
	void disableNoflicker();
	
	
	
	
#endif








#ifdef MODULE_ADDSYSCALLS
	void setSyscall(int syscall_number,unsigned int address);
	void extendSWIHandler();
	
	
#endif


#ifdef MODULE_CLOCK
	#include "clock/miniclock.h"
#endif

#ifdef MODULE_SETTINGS
	#include "settings/settings.h"
#endif

#ifdef MODULE_DESKTOP
	#include "desktop/desktop.h"
#endif

#ifdef MODULE_DISABLENAVNET
	#include "disablenavnet/disablenavnet.h"
#endif


#ifdef MODULE_SHELL
	#include "shell/shell.h"
#endif

#ifdef MODULE_USBTEST
	#include "usb/usbtest.h"
#endif

#ifdef MODULE_SECURITY
	#include "security/security.h"
#endif

#ifdef MODULE_DYNLINKER
	#include "dynlinker/dynlinker.h"
#endif



#ifdef MODULE_ADDSYSCALLS
	#include "addsyscalls/addsyscalls.h"
#endif

#ifndef MODULE_NOFLICKER
#include "noflicker/noflicker.h"
#endif


































