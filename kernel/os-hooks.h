#ifndef OS_HOOKS
#define OS_HOOKS



void install_hooks();
void uninstall_hooks();
uint32_t get_os_draw_address();
void set_drawhook(bool enable);



int maxDayInMonth(unsigned month, int leap);
int isLeapYear(unsigned year);
unsigned long time2timestamp(int hr, int min, int sec);
unsigned long date2timestamp(int year, int month, int day, int hr, int min, int sec);
unsigned long timestamp2time(unsigned long t, int * hr, int * min, int * sec);
void timestamp2day(unsigned long t, int * year, int * month, int * day);
void timestamp2date(unsigned long t, int * year, int * month, int * day, int * hr, int * min, int * sec);




#endif