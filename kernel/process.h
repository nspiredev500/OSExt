#ifndef PROCESS_H
#define PROCESS_H

#include "thread.h"



struct Process;



struct Process {
	struct Thread main;
	unsigned char status;
	unsigned short pid;
	unsigned int nthreads; // additional threads
	struct Thread *threads;
};









































#endif