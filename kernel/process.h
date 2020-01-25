#ifndef PROCESS_H
#define PROCESS_H

#include "thread.h"



struct Process;



struct Process {
	uint8_t status;
	uint16_t pid;
	struct Thread *threads;
	struct Process *next;
};









































#endif