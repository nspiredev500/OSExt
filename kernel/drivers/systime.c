#include "../kernel.h"




/*
	since the timers are all 32 bit, they can overflow
	the rtc will overflow in the year ~2038
	the milisecond timers after ~20 days
	so just store the number of overflows to reconstruct the original number
*/
static volatile uint32_t timer_milis_overflows = 0;
static volatile uint32_t timer_seconds_overflows = 0;


void systime_init()
{
	timer_disable(SYSTIME_TIMER);
	timer_set_clockselect(SYSTIME_TIMER,0xa);
	timer_set_prescaler(SYSTIME_TIMER,0);
	timer_set_mode(SYSTIME_TIMER,0b0);
	timer_set_oneshot(SYSTIME_TIMER,false);
	timer_set_size(SYSTIME_TIMER,1);
	timer_set_load(SYSTIME_TIMER,0xffffffff);
	timer_enable(SYSTIME_TIMER);
	
	
	rtc_claim();
	rtc_set_alarm(0);
	
	//debug_shell_println("initializing systime: timer %d,%d",SYSTIME_TIMER);
	
	systime_set_unix(systime_unix()); // to set the millisecond time to the second time, as the RTC not always resets after a reboot
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
	return timer_seconds_overflows*0x100000000+rtc_get_value();
}


int64_t systime_unix_micro()
{
	return (int64_t) ((double) (timer_milis_overflows*0x100000000)*31.25 + (double) (0xffffffff-timer_value(SYSTIME_TIMER))*31.25);
}


// returns the current unix time with millisecond accuracy
int64_t systime_unix_milli()
{
	return  (timer_milis_overflows*0x100000000)/32+(0xffffffff-timer_value(SYSTIME_TIMER))/32;
}



void systime_set_unix_milli(int64_t unix_time_milis)
{
	if (unix_time_milis <= 0) // we use the RTC as a second clock, and it can't deal with negative numbers
	{
		return;
	}
	int64_t seconds = unix_time_milis/1000;
	int64_t milis_remaining = unix_time_milis-(seconds*1000);
	
	timer_set_load(SYSTIME_TIMER,0xffffffff-milis_remaining*32);
	
	uint64_t unix_time = unix_time_milis/1000;
	timer_seconds_overflows = unix_time/0x100000000;
	rtc_set_value(unix_time%0x100000000);
}



void systime_set_unix(int64_t unix_time)
{
	if (unix_time <= 0) // we use the RTC as a second clock, and it can't deal with negative numbers
	{
		return;
	}
	timer_seconds_overflows = unix_time/0x100000000;
	rtc_set_value(unix_time%0x100000000);
	
	timer_milis_overflows = (unix_time*1000)/(0x100000000/32);
	uint32_t milis_remaining = (unix_time*1000)%(0x100000000/32);
	
	timer_set_load(SYSTIME_TIMER,0xffffffff-milis_remaining*32);
}





static uint32_t days_in_month(uint32_t year,uint32_t month)
{
	switch (month)
	{
	case 1:
		return 31;
		break;
	case 2:
		if (year % 4 == 0)
		{
			if (year % 100 == 0)
			{
				if (year % 400 == 0)
				{
					return 29;
				}
				else
				{
					return 28;
				}
			}
			else
			{
				return 29;
			}
		}
		else
		{
			return 28;
		}
		break;
	case 3:
		return 31;
		break;
	case 4:
		return 30;
		break;
	case 5:
		return 31;
		break;
	case 6:
		return 30;
		break;
	case 7:
		return 31;
		break;
	case 8:
		return 31;
		break;
	case 9:
		return 30;
		break;
	case 10:
		return 31;
		break;
	case 11:
		return 30;
		break;
	case 12:
		return 31;
		break;
	}
	return 0;
}

static uint32_t days_in_year(uint32_t year)
{
	if (year % 4 == 0)
	{
		if (year % 100 == 0)
		{
			if (year % 400 == 0)
			{
				return 366;
			}
			else
			{
				return 365;
			}
		}
		else
		{
			return 366;
		}
	}
	else
	{
		return 365;
	}
}

int64_t systime_timestamp_to_unix(uint32_t year,uint32_t month, uint32_t day,uint32_t hour, uint32_t minute, uint32_t second)
{
	int64_t unix = second;
	unix += minute*60;
	unix += hour*60*60;
	unix += day*24*60*60;
	//DEBUGPRINTLN_1("timestamp to: y: %d, m: %d, d: %d, h: %d, min: %d, s: %d",year,month,day,hour,minute,second)
	for (uint32_t m = month;m>0;m--)
	{
		unix += days_in_month(year,month)*24*60*60;
	}
	if (year == 1970)
	{
		return unix;
	}
	if (year < 1970)
	{
		for (uint32_t y = year;y<1970;y++)
		{
			unix -= days_in_year(y)*24*60*60;
		}
	}
	else
	{
		for (uint32_t y = year;y>1970;y--)
		{
			unix += days_in_year(y)*24*60*60;
		}
	}
	return unix;
}


void systime_unix_to_timestamp(int64_t unix,uint32_t *year,uint32_t *month,uint32_t *day,uint32_t *hour,uint32_t *minute,uint32_t *second)
{
	if (unix <= 0)
	{
		return;
	}
	uint32_t y = 1970;
	while (unix > 0)
	{
		unix -= days_in_year(y)*24*60*60;
		y++;
	}
	y--;
	unix += days_in_year(y)*24*60*60;
	if (year != NULL)
	{
		*year = y;
	}
	
	uint32_t m = 1;
	while (unix > 0)
	{
		unix -= days_in_month(y,m)*24*60*60;
		m++;
	}
	m--;
	unix += days_in_month(y,m)*24*60*60;
	if (month != NULL)
	{
		*month = m;
	}
	
	
	uint32_t d = 1;
	while (unix > 0)
	{
		unix -= 24*60*60;
		d++;
	}
	d--;
	unix += 24*60*60;
	if (day != NULL)
	{
		*day = d;
	}
	
	uint32_t h = 0;
	while (unix > 0)
	{
		unix -= 60*60;
		h++;
	}
	h--;
	unix += 60*60;
	if (hour != NULL)
	{
		*hour = h;
	}
	
	uint32_t min = 0;
	while (unix > 0)
	{
		unix -= 60;
		min++;
	}
	min--;
	unix += 60;
	if (minute != NULL)
	{
		*minute = min;
	}
	if (second != NULL)
	{
		*second = (uint32_t) unix;
	}
	//DEBUGPRINTLN_1("timestamp from unix: y: %d, m: %d, d: %d, h: %d, min: %d, s: %d",y,m,d,h,min,(uint32_t) unix)
}



































