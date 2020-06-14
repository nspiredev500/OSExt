#include "../kernel.h"


const uint32_t MEM_WRITE = 0b1;
const uint32_t MEM_READ = 0b10;
const uint32_t MEM_SHARED = 0b100;
const uint32_t MEM_COW = 0b1000;







struct process* createProcess(uint16_t pid,uint32_t uid, uint32_t gid)
{
	struct process *p = requestProcess();
	p->space = createAddressSpace();
	p->status = 0;
	p->priviledges = 0;
	p->pid = pid;
	p->uid = uid;
	p->gid = gid;
	p->threads = NULL;
	p->mem = NULL;
	return p;
}



void destroyProcess(struct process *p)
{
	destroyAddressSpace(p->space);
	struct thread *t = p->threads;
	while (t != NULL)
	{
		struct thread *next = t->next;
		destroyThread(t);
		t = next;
	}
	struct mem_desc* mem = p->mem;
	while (mem != NULL)
	{
		/// TODO destroy mem, and the data if the type uses the data field
		
		
		mem = mem->next;
	}
	freeProcess(p);
}

















































