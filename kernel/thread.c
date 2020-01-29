#include "kernel.h"



struct Thread* createThread(uint16_t tid)
{
	struct Thread *t = requestThread();
	for (int i = 0;i<16;i++)
	{
		t->regs[i] = 0;
	}
	t->tid = tid;
	t->status = 0;
	return t;
}



void destroyThread(struct Thread *t)
{
	freeThread(t);
}


















































