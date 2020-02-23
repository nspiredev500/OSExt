#ifndef THREAD_H
#define THREAD_H


#include "LinkedList.h"

struct Thread;
struct Thread {
	uint32_t regs[17];
	uint16_t tid;
	uint16_t status;
};


struct Thread* createThread(uint16_t tid, uint32_t pc);
void destroyThread(struct Thread *t);

void runThread(struct Thread *t);


































#endif