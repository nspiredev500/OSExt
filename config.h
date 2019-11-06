/* 
uncomment the modules you want, comment the ones you don't
"//" at the start of a line for comments
*/
#ifndef MODULES_CHOSEN_BY_MAKE
// use this to enable experimental on all modules features. not stable.
// may not start at all or do undocumented stuff, change files, delete them, everything bugs can do. not recommended
// for development purposes
//#define EXPERIMENTAL

#define MODULE_CLOCK
//#define MODULE_SETTINGS
//#define MODULE_SHELL // currently not implemented
//#define MODULE_DESKTOP
//#define MODULE_DISABLENAVNET
//#define MODULE_SECURITY
//#define MODULE_USBTEST // currently not working





// DEPENDENCIES - DO NOT REMOVE

#else
#pragma message ("Make defined modules")
#endif

//dependencies: some modules need others and those will be selected even if you disabled them above
#ifdef MODULE_SECURITY
	#ifndef MODULE_SETTINGS
		#define MODULE_SETTINGS //security needs settings, so you can set a password, and without a password it's pointless
	#endif
#endif






