#ifndef SYSTIME_H
#define SYSTIME_H



// if not otherwise specified, use the first timer of the first timer module for the system time
#ifndef SYSTIME_TIMER
	#define SYSTIME_TIMER 1,0
#endif





void systime_init();



// returns the current unix time in second
int64_t systime_unix();






// returns the current unix time with millisecond accuracy
int64_t systime_unix_milis();

int64_t systime_unix_micro();


void systime_set_unix_milis(int64_t unix_time_milis);



void systime_set_unix(int64_t unix_time);





int64_t systime_timestamp_to_unix(uint32_t year,uint32_t month, uint32_t day,uint32_t hour, uint32_t minute, uint32_t second);


void systime_unix_to_timestamp(int64_t unix,uint32_t *year,uint32_t *month,uint32_t *day,uint32_t *hour,uint32_t *minute,uint32_t *second);




void systime_rtc_overflow();
void systime_timer_overflow();







#endif