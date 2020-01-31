#ifndef SLAB_H
#define SLAB_H




void initSlabAllocator();







void slab_allocator_self_test();









LinkedList* requestLinkedListEntry();
void freeLinkedListEntry(void* list);

uint32_t* requestCPT();
void freeCPT(void* cpt);


struct address_space* requestAddressSpace();
void freeAddressSpace(void *space);


uint32_t* requestTranslationTable();
void freeTranslationTable(uint32_t *tt);


void* requestLCDFramebuffer();
void freeLCDFramebuffer(void* buff);


struct Process* requestProcess();
void freeProcess(void* proc);


struct Thread* requestThread();	
void freeThread(void* thread);


uint32_t* request4Bytes();
void free4Bytes(uint32_t* b);






#endif