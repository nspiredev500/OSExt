#ifndef PROCESS_H
#define PROCESS_H

#include "../LinkedList.h"
#include "../memory/virtual_mm.h"


extern const uint32_t MEM_WRITE;
extern const uint32_t MEM_READ;
extern const uint32_t MEMT_SHARED;
extern const uint32_t MEMT_COW;
extern const uint32_t MEMT_PRIVATE;


struct mem_desc;




struct file_buffer {
	
};


struct mem_desc {
	void* address; // virtual address
	void* page; // physical address of the page
	uint16_t type;
	/*
		0 = private memory
		1 = shared memory
		2 = copy on write memory
	*/
	void* data; /// TODO always free the data when the mem_desc is freed, if the data is used
	uint16_t permissions; // permissions are needed for when a page is swapped and unmapped from the page table, and needs to be put there again with the same permissions
	// 0b1 = read permission
	// 0b2 = write permission
	
	
	
	struct mem_desc* next;
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
	// memory descriptors are added when a page is swapped out and as such the information in the page table is lost.
	// The memory descriptor is there so we can reconstruct what type of memory it is.
	// It is also used for copy-on-write mapping
	
	
};




struct process* createProcess(uint16_t pid,uint32_t uid, uint32_t gid);
void destroyProcess(struct process *p);


void process_new_thread();
void process_delete_thread();
































#endif