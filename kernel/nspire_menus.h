#ifndef NSPIRE_MENUS_H
#define NSPIRE_MENUS_H


enum nspire_menu {MENU_ABOUT = 0, MENU_BATTERY, MENU_EXPLORER, MENU_DOC_SETTINGS,
				  MENU_DOC, MENU_SETUP, MENU_HINTS, MENU_INNOVATOR, MENU_LANGUAGE, MENU_LOGIN,
				  MENU_MAIN, MENU_NETWORK, MENU_RECENT, MENU_SCRATCHPAD, MENU_SETTINGS_POPUP,
				  MENU_STATUS,MENU_LANGUAGE_POPUP};


bool menu_open(void* nspire_framebuffer,enum nspire_menu menu); // probes the framebuffer for open menus



















#endif