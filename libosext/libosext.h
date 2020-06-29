#ifndef LIBOSEXT_H
#define LIBOSEXT_H





// returns the current unix time (as set by the user), in seconds
int64_t osext_unix_time();


// returns the current unix time (as set by the user), in milliseconds
int64_t osext_unix_time_milli();



// returns the current unix time (as set by the user), in microseconds
int64_t osext_unix_time_micro();


// sets the unix time from a value of seconds
void osext_set_unix_time(int64_t unix);


// sets the unix time from a value of milliseconds
void osext_set_unix_time_milli(int64_t unix);








#endif