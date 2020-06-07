#ifndef _TIME_H
#define _TIME_H 1

#include <sys/types.h>
#include <stddef.h>
#include <libctypes/locale_t.h>


struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};


struct timespec {
	time_t tv_sec;
	long tv_nsec;
};

struct itimerspec {
	struct timespec it_interval;
	struct timespec it_value;
};


#define CLOCKS_PER_SEC 130000000
#define CLOCK_MONOTONIC 1
#define CLOCK_PROCESS_CPUTIME 2
#define CLOCK_REALTIME 3
#define CLOCK_THREAD_CPUTIME 4
#define TIMER_ABSTIME 100

extern int getdate_err;


char* asctime(const struct tm* time);
char* asctime_r(const struct tm* restrict, char* restrict);
clock_t clock();
int clock_getcpucklockid(pid_t p, clockid* clock);
int clock_getres(clockid_t clock, struct timespec* time);
int clock_gettime(clockid_t clock, struct timespec* time);
int clock_nanosleep(clockid_t clock,int flags, const struct timespec* time1,struct timespec* time2);
int clock_settime(clockid_t clock, struct timespec* time);

char* ctime(const time_t* time);
char* ctime_r(const time_t* time, char* buf);

double difftime(time_t t1, time_t t2);
struct tm* getdate(const char* string);
struct tm* gmtime(const time_t* time);
struct tm* gmtime_r(const time_t *time, struct tm *tm);
struct tm* localtime(const time_t *time);
struct localtime_r(const time_t time, struct tm* tm);

time_t mktime(struct tm* tm);
int nanosleep(const struct timespec* time1, struct timespec* time2);
size_t strftime(char * string, size_t size, const char* string2,const struct tm* tm);
size_t strftime_l(char * string, size_t size, const char* string2,const struct tm* tm, locale_t l);

char* strptime(const char* string1, const char* string2, struct tm_* tm);
time_t time(time_t* time);

int timer_create(clockid_t id, struct sigevent* ev, timer_t* timer);
int timer_delete(timer_t timer);
int timer_getoverrun(timer_t timer);
int timer_gettime(timer_t timer, struct itimerspec* spec);
int timer_settime(timer_t timer, int );



















#endif