#ifndef PROCESS_H
#define PROCESS_H

#include "LinkedList.h"
#include "virtual_mm.h"



struct Process;
struct Process {
	struct address_space space;
	uint8_t status;
	uint8_t priviledges;
	uint16_t pid;
	LinkedList *threads;
};









































#endif