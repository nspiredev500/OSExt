#ifndef THREAD_H
#define THREAD_H
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>

struct Thread;



struct Thread {
	uint32_t regs[16];
	unsigned short tid;
	unsigned char status;
};







































#endif