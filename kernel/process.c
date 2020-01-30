#include "kernel.h"




struct Process* createProcess(uint16_t pid)
{
	struct Process *p = requestProcess();
	p->space = createAddressSpace();
	p->status = 0;
	p->priviledges = 0;
	p->pid = pid;
	p->threads = NULL;
	return p;
}
void destroyProcess(struct Process *p)
{
	destroyAddressSpace(p->space);
	uint32_t i = 0;
	LinkedList *cpt = NULL;
	while ((cpt = getLinkedListEntry(&p->threads,i)) != NULL)
	{
		cpt = getLinkedListEntry(&p->threads,i);
		destroyThread(cpt->data);
		i++;
	}
	destroyLinkedList(&p->threads);
}

















































