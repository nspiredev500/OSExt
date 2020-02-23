#ifndef SLAB_H
#define SLAB_H



void* const kernel_heap_start;

void* getKernelHeapNextPage();
void setKernelHeapNextPage(void* next);

void initSlabAllocator();

void* alloc_object_from_slab(struct slab_desc_t* slab,uint32_t obj_size);
void* alloc_object_from_cache(struct cache_t* cache);
void growCache(struct cache_t* cache);

void createCache(uint32_t obj_size,uint32_t flags, char* name);


void* kmalloc(uint32_t size);
void kfree(void* obj);


// this one performs better, because it only searches the caches that are equal or bigger than the object size
void kfree(void* obj,uint32_t size);



// check if partial slabs are actually free, update pre-cached objects..., for milis miliseconds at most
void slab_maintenance(uint32_t milis);


bool slab_allocator_self_test();


void ensureCPTCapacity();
void ensureLinkedListCapacity();







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