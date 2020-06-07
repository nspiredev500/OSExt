#ifndef PROCESS_H
#define PROCESS_H

#include "../LinkedList.h"
#include "../memory/virtual_mm.h"


const uint32_t MEM_WRITE;
const uint32_t MEM_READ;
const uint32_t MEMT_SHARED;
const uint32_t MEMT_COW;
const uint32_t MEMT_PRIVATE;

struct shm_desc { // descriptor for shared memory
	uint16_t count; //reference counter. only free if counter is 0
	
};

struct cow_desc;
struct cow_desc {
	struct cow_desc *other; // pointer to the other copy-on-write page in the other process, so it can be changed to normal after this one is copied
};





struct mem_desc { // additional information for a memory page
	void* address; // the virtual address the descriptor is for
	void* data; // additional data
	/*
		0 = private memory, data = NULL
		1 = cow, data = struct cow_desc*
		2 = shared memory, data = struct shm_desc*
	*/
	uint32_t type;
	uint32_t flags; // bit 0 = read, bit 1 = write
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
	struct mem_desc* mem;
};




struct process* createProcess(uint16_t pid,uint32_t uid, uint32_t gid);
void destroyProcess(struct process *p);


void process_new_thread();
void process_delete_thread();
































#endif