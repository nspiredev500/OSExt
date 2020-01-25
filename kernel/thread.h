#ifndef THREAD_H
#define THREAD_H
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>

struct Thread;



struct Thread {
	uint32_t regs[16];
	uint16_t tid;
	uint8_t status;
	struct Thread *next;
};







































#endif