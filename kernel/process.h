#ifndef PROCESS_H
#define PROCESS_H

#include "LinkedList.h"
#include "virtual_mm.h"


struct shm_desc { // descriptor for shared memory
	uint16_t count; //reference counter. only free if counter is 0
};


struct cow_desc {
	
};


const uint32_t MEM_WRITE;
const uint32_t MEM_READ;
const uint32_t MEM_SHARED;
const uint32_t MEM_COW;





struct mem_desc { // additional information for a memory page
	void* address; // the virtual address the descriptor is for
	void* data; // additional data
	
	uint32_t flags; 
};



struct process;
struct process {
	struct address_space *space;
	uint8_t status;
	uint8_t priviledges;
	uint16_t pid;
	uint32_t uid;
	uint32_t gid;
	struct thread *threads;
};




struct process* createProcess(uint16_t pid,uint32_t uid, uint32_t gid);
void destroyProcess(struct process *p);


void process_new_thread();
void process_delete_thread();
































#endif