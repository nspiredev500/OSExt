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
	return p;
}
void destroyProcess(struct process *p)
{
	destroyAddressSpace(p->space);
	uint32_t i = 0;
	LinkedList *cpt = NULL;
	// TODO free the threads in the list
}

















































