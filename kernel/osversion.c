#include "kernel.h"



static int32_t ut_os_version_index = -1;



void read_osversion()
{
	if (ut_os_version_index != -1)
	{
		// adapted from ndless/src/resources/utils.c
		switch (*(unsigned*)(0x10000020)) {
		case 0x102F0FA0: // 3.1.0 non-CAS
			ut_os_version_index = 0;
			break;
		case 0x102F16D0: // 3.1.0 CAS
			ut_os_version_index = 1;
			break;
		case 0x102F0A10: // 3.1.0 non-CAS CX
			ut_os_version_index = 2;
			break;
		case 0x102F11A0: // 3.1.0 CAS CX
			ut_os_version_index = 3;
			break;
		case 0x102DBF20: // 3.1.0 CM-C
			ut_os_version_index = 4;
			break;
		case 0x102DC6B0: // 3.1.0 CAS CM-C
			ut_os_version_index = 5;
			break;
		case 0x10375BB0: // 3.6.0 non-CAS
			ut_os_version_index = 6;
			break;
		case 0x103765F0: // 3.6.0 CAS
			ut_os_version_index = 7;
			break;
		case 0x10375620: // 3.6.0 non-CAS CX
			ut_os_version_index = 8;
			break;
		case 0x10376090: // 3.6.0 CAS CX
			ut_os_version_index = 9;
			break;
		case 0x1037CDE0: // 3.9.0 non-CAS
			ut_os_version_index = 10;
			break;
		case 0x1037D320: // 3.9.0 CAS
			ut_os_version_index = 11;
			break;
		case 0x1037C760: // 3.9.0 non-CAS CX
			ut_os_version_index = 12;
			break;
		case 0x1037CCC0: // 3.9.0 CAS CX
			ut_os_version_index = 13;
			break;
		case 0x1037D160: // 3.9.1 non-CAS CX
			ut_os_version_index = 16;
			break;
		case 0x1037D6C0: // 3.9.1 CAS CX
			ut_os_version_index = 17;
			break;
		case 0x10386E70: // 4.0.0.235 non-CAS CX
			ut_os_version_index = 18;
			break;
		case 0x103873B0: // 4.0.0.235 CAS CX
			ut_os_version_index = 19;
			break;
		case 0x1038C290: // 4.0.3.93 non-CAS CX
			ut_os_version_index = 20;
			break;
		case 0x1038C7D0: // 4.0.3.93 CAS CX
			ut_os_version_index = 21;
			break;
		case 0x1039C7A0: // 4.2.0.532 non-CAS CX
			ut_os_version_index = 22;
			break;
		case 0x1039CD20: // 4.2.0.532 CAS CX
			ut_os_version_index = 23;
			break;
		case 0x103A3100: // 4.3.0.702 non-CAS CX
			ut_os_version_index = 24;
			break;
		case 0x103A3690: // 4.3.0.702 CAS CX
			ut_os_version_index = 25;
			break;
		case 0x103A9F50: // 4.4.0.532 non-CAS CX
			ut_os_version_index = 26;
			break;
		case 0x103AA4E0: // 4.4.0.532 CAS CX
			ut_os_version_index = 27;
			break;
		case 0x103B1860: // 4.5.0.1180 non-CAS CX
			ut_os_version_index = 28;
			break;
		case 0x103B1E60: // 4.5.0.1180 CAS CX
			ut_os_version_index = 29;
			break;
		case 0x103B27D0: // 4.5.1.12 non-CAS CX
			ut_os_version_index = 30;
			break;
		case 0x103B2EF0: // 4.5.1.12 CAS CX
			ut_os_version_index = 31;
			break;
		default:
			panic("unknown os version!");
		}
	}
}

uint32_t os_value(const uint32_t* values,int32_t size)
{
	if (ut_os_version_index >= size)
		return 0;
	return values[ut_os_version_index];
}


uint32_t get_osversion()
{
	if (ut_os_version_index == -1)
	{
		read_osversion();
	}
	return ut_os_version_index;
}









