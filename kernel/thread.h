#ifndef THREAD_H
#define THREAD_H


#include "LinkedList.h"

struct Thread;
struct Thread {
	uint32_t regs[16];
	uint16_t tid;
	uint8_t status;
};







































#endif