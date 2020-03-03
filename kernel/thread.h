#ifndef THREAD_H
#define THREAD_H


#include "LinkedList.h"

struct Thread;
struct Thread {
	uint32_t regs[17];
	uint16_t tid;
	uint16_t status;
};

struct Thread *running_thread;

struct Thread* createThread(uint16_t tid, uint32_t pc);
void destroyThread(struct Thread *t);




// 0 means successfully run until the timeslice completed
// 
uint32_t runThread(struct Thread *t);


































#endif