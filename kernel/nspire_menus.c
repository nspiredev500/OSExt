#include "kernel.h"





bool menu_open(void* nspire_framebuffer_raw,enum nspire_menu menu)
{
	uint16_t *nspire_framebuffer = nspire_framebuffer_raw;
	uint16_t menucolor1 = rgbto565(231,227,231); // about, hints, setup
	uint16_t menucolor2 = rgbto565(200,191,231); // settings
	uint16_t menucolor3 = rgbto565(206,203,206); // explorer bar
	uint16_t menucolor4 = rgbto565(222,28,57); // doc red x
	uint16_t menucolor5 = rgbto565(255,251,140); // battery popup
	uint16_t menucolor6 = rgbto565(173,170,173); // status handheld color
	uint16_t menucolor7 = rgbto565(115,113,115); // bar in login/network screen
	uint16_t menucolor8 = rgbto565(247,247,247); // gray from recent files
	uint16_t menucolor9 = rgbto565(41,150,189); // blue from recent files
	switch (menu)
	{
	case MENU_ABOUT:
		if (nspire_framebuffer[200] == menucolor1)
			return true;
		break;
	case MENU_BATTERY:
		if (nspire_framebuffer[250*240+35] == menucolor5)
			return true;
		if (nspire_framebuffer[174*240+62] == menucolor5)
			return true;
		if (nspire_framebuffer[274*240+64] == menucolor5)
			return true;
		break;
	case MENU_EXPLORER:
		if (nspire_framebuffer[42*240+32] == menucolor3)
			return true;
		break;
	case MENU_DOC_SETTINGS:
		if (nspire_framebuffer[30*240+225] == menucolor2)
			return true;
		break;
	case MENU_DOC:
		if (nspire_framebuffer[310*240+10] == menucolor4)
			return true;
		break;
	case MENU_SETUP:
		if (nspire_framebuffer[11*240+230] == menucolor1)
			return true;
		break;
	case MENU_HINTS:
		if (nspire_framebuffer[200] == menucolor1)
			return true;
		break;
	case MENU_INNOVATOR:
		if (nspire_framebuffer[50*240+133] == menucolor7)
			return true;
		break;
	case MENU_LANGUAGE:
		if (nspire_framebuffer[103*240+130] == menucolor7)
			return true;
		break;
	case MENU_LOGIN:
		if (nspire_framebuffer[87*240+144] == menucolor7)
			return true;
		break;
	case MENU_MAIN:
		return false; // just assume it's the main menu if it's not another one
		break;
	case MENU_NETWORK:
		if (nspire_framebuffer[137*240+122] == menucolor7)
			return true;
		break;
	case MENU_RECENT:
		if (nspire_framebuffer[300*240+106] == menucolor8 || nspire_framebuffer[300*240+106] == menucolor9)
			return true;
		break;
	case MENU_SCRATCHPAD:
		if (nspire_framebuffer[180*240+10] == menucolor1 && nspire_framebuffer[60] == rgbto565(0,0,0))
			return true;
		break;
	case MENU_SETTINGS_POPUP:
		if (nspire_framebuffer[168*240+78] == menucolor8 || nspire_framebuffer[168*240+78] == menucolor9)
			return true;
		break;
	case MENU_STATUS:
		if (nspire_framebuffer[82*240+46] == menucolor6)
			return true;
		break;
	case MENU_LANGUAGE_POPUP:
		if (nspire_framebuffer[267*240+216] == rgbto565(255,255,255))
			return true;
		break;
	case MENU_POPUP_ANY:
		if (nspire_framebuffer[168*240+78] == menucolor8 || nspire_framebuffer[168*240+78] == menucolor9 || nspire_framebuffer[168*240+78] == menucolor1)
			return true;
		break;
	case MENU_ANY:
		if (nspire_framebuffer[160*240+120] != rgbto565(0,0,0) || nspire_framebuffer[120] != rgbto565(0,0,0) || nspire_framebuffer[319*240+120] != rgbto565(0,0,0))
			return true;
		break;
	default:
		return false;
	}
	return false;
}




















