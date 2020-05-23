

#if _CLOCK == true
//Contents of this file are from levak's ncklock
/* Optimized LeapYear Detection (with optimized modulos) */
int maxDayInMonth(unsigned month, int leap) {
    if(month == 4 || month == 6 || month == 9 || month == 11)
        return 30;
    else
        if(month == 2)
            return (leap) ? 29 : 28;
        else
            return 31;
}
int isLeapYear(unsigned year) {
    if(!(year&3)) { // mod 4 = 0
        int r2= (year>>2)/25;	// century-1
		int r3= (r2>>1)/5;		// millennium-1
        r2= year - ((r2*25)	 << 2);	// 2-digits year
		r3= year - ((r3*125) << 3);	// 3-digits year
        if(!r3 || (!(r2&3) && r2))
            return 1;
        else
            return 0;
    }
    else
        return 0;
}

static const double nb_days_in_year = 5113/14.;

unsigned long time2timestamp(int hr, int min, int sec) {
    return sec + min*60 + hr*3600;
}

unsigned long date2timestamp(int year, int month, int day, int hr, int min, int sec) {
	int cyear;
    sec = time2timestamp(hr, min, sec);
    int ly = isLeapYear(year);
    for(--month; month > 0; --month)
		day += maxDayInMonth(month, ly);
	day--;
	day+=365*(year-1970);
	for(cyear=1972;cyear<year;cyear+=4)
		day+=isLeapYear(cyear);
    return sec + day*86400;
}

unsigned long timestamp2time(unsigned long t, int * hr, int * min, int * sec) {
    unsigned long tt;
    /* compute hh:mm:ss with shift optimisations (divisions & modulos are expensive) */
    tt = (t >> 2) / 15;
    *sec = t - ((tt * 15) << 2);
    t = tt;
    tt = (tt >> 2) / 15;
    *min = t - ((tt * 15) << 2);
    t = tt;
    tt = (tt >> 3) / 3;
    *hr = t - ((tt * 3) << 3);
    return tt;
}

void timestamp2day(unsigned long t, int * year, int * month, int * day) {
	*year=1970;
	int leap_year=0;
	while(t>=365+ (unsigned) leap_year) { // (unsigned) added
		(*year)++;
		t-=365+leap_year;
		leap_year=isLeapYear(*year);
	}
    *month = 1;
    int max_day = maxDayInMonth(*month, leap_year);
    while(t >= (unsigned) max_day) { // (unsigned) added
        t -= max_day;
        ++*month;
        max_day = maxDayInMonth(*month, leap_year);
    }
	*day=t+1;
}

void timestamp2date(unsigned long t, int * year, int * month, int * day, int * hr, int * min, int * sec) {
    t = timestamp2time(t, hr, min, sec);
    timestamp2day(t, year, month, day);
}
#endif