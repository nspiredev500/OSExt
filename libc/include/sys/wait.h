#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H 1


#include <sys/types.h>


// TODO the macros and function implementations






#if __STDC_HOSTED__ != 0
	pid_t wait(int*);
	int waitid(idtype_t, id_t, siginfo_t*,int);
	pid_t waitpid(pid_t, int*,int);
#endif


#endif