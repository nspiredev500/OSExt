#ifndef PROCESS_H
#define PROCESS_H

#include "../LinkedList.h"
#include "../memory/virtual_mm.h"


const uint32_t MEM_WRITE;
const uint32_t MEM_READ;
const uint32_t MEMT_SHARED;
const uint32_t MEMT_COW;
const uint32_t MEMT_PRIVATE;


struct mem_desc;


// cow_desc is a list of what other virtual pages are mapped as the same page as copy-on-write
// when a write occurs, the mapping changes to private and the mem_desc is removed from the list
// if there are only 2 cow_desc in the list and a write occurs, both are changed to private and
// the cow-mapping is completely removed
/// TODO one of the 2 remaining processes can use the cow-page after a write, no need to copy to 2 pages and
/// free the old one when the data is already there
struct cow_desc;
struct cow_desc {
	struct mem_desc* owner;
	struct cow_desc* next;
};





struct mem_desc {
	void* address; // virtual address
	void* page; // physical address of the page
	uint16_t type;
	/*
		0 = private memory
		1 = shared memory
		2 = copy on write memory, data is cow_desc
	*/
	void* data;
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
};




struct process* createProcess(uint16_t pid,uint32_t uid, uint32_t gid);
void destroyProcess(struct process *p);


void process_new_thread();
void process_delete_thread();
































#endif