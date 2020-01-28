#ifndef SLAB_H
#define SLAB_H




void initSlabAllocator();

LinkedList* requestLinkedListEntry();
void freeLinkedListEntry(void* list);

uint32_t* requestCPT();
void freeCPT(void* cpt);

























#endif