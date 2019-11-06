

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
	void initializeDynlinker();
	void DynlinkerSelftest();
	
	
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


























