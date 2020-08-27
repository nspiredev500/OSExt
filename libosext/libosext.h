#ifndef LIBOSEXT_H
#define LIBOSEXT_H
#include <stdint.h>
#include <stdbool.h>


typedef void osext_thread;


// a value other than 0 indicates an error
// set by all functions
extern int osext_errno;


bool osext_installed();


// returns the osext version, or 0 if not installed
// the first byte is the mayor version
// the second byte is the minor version
// the third byte is the patch version
uint32_t osext_version();




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


// this has to be called before any of the thread functions work
void osext_init_threads();


// this has to be called before your program exits, if you called osext_init_threads
// it can only be called from your main thread
// it immediately stops all other threads
void osext_exit_threads();



// creates a thread with the specified stack, stack size and entry point.
// the entry point cannot get arguments
// the specified stack region has to be freed by the program
osext_thread* osext_create_thread(void* stack,uint32_t stacksize,void (*entry)(void));


// destroys the thread t, waking up any other threads waiting for it.
// the original thread cannot be destroyed
// the stack still has to be freed by the program
void osext_destroy_thread(osext_thread* t);




// lets the current thread sleep for the specified amount of milliseconds.
// the accuracy is dependent on the timeslice length, and gets rounded down to the nearest timeslice (currently 2 milliseconds)
void osext_thread_wait_milli(uint32_t milli);

// lets the current thread sleep until the specified thread has finished
void osext_thread_wait_for_thread(osext_thread* t);






#endif