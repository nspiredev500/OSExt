#include "../kernel.h"






static uint32_t timer_milis_overflows = 0;
uint32_t timer_seconds_overflows = 0;


void systime_init()
{
	timer_enable(SYSTIME_TIMER);
	timer_set_mode(SYSTIME_TIMER,0);
	timer_set_oneshot(SYSTIME_TIMER,false);
	
	rtc_claim();
	
	rtc_set_alarm(0);
	
	
}

void systime_rtc_overflow()
{
	timer_seconds_overflows++;
}
void systime_timer_overflow()
{
	timer_milis_overflows++;
}


// returns the current unix time in second
int64_t systime_unix()
{
	
	
	
	
	
	
	return 0;
}


// returns the current unix time, modified by the current time zone and the user-supplied leap seconds
// so it should be the users clock-time
int64_t systime_calendar()
{
	
	
	
	
	
	
	return 0;
}



// returns the current unix time with millisecond accuracy
int64_t systime_unix_milis()
{
	
	
	
	
	
	
	return 0;
}



void systime_set_unix_milis(int64_t unix_time_milis)
{
	if (unix_time_milis <= 0) // we use the RTC as a second clock, and it can't deal with negative numbers
	{
		return;
	}
	int64_t seconds = unix_time_milis/1000;
	int64_t milis_remaining = unix_time_milis-(seconds*1000);
	systime_set_unix(seconds);
	timer_set_load(SYSTIME_TIMER,0xffffffff-milis_remaining);
	
	
	
	
	
	
}



void systime_set_unix(int64_t unix_time)
{
	if (unix_time <= 0) // we use the RTC as a second clock, and it can't deal with negative numbers
	{
		return;
	}
	
	
	
	
	
	
	
}



void systime_set_leap_seconds(uint32_t leapseconds)
{
	
	
	
	
	
	
	
}


// specifies the current timezone with UTC+/- x
// daylight saving time should be entered as a different timezone
void systime_set_timezone(int32_t timezone)
{
	
	
	
	
	
	
	
}



int64_t systime_timestamp_to_unix(uint32_t year,uint32_t month, uint32_t day,uint32_t hour, uint32_t minute, uint32_t second)
{
	int64_t unix = second;
	unix += minute*second;
	unix += hour*60*60;
	unix += day*24*60*60;
	unix += ;
	
	
	return 0;
}


void systime_unix_to_timestamp(int64_t unix,uint32_t *year,uint32_t *month,uint32_t *day,uint32_t *hour,uint32_t *minut,uint32_t *second)
{
	
	
	
	
	
	
	
}



































