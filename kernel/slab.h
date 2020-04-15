#ifndef SLAB_H
#define SLAB_H

struct cache_t;
struct slab_desc_t;
struct slab_desc_t {
	uint32_t size; // number of pages
	void *start; // start of the slab in memory
	struct slab_desc_t *next;
	// pointer to the array where the used bit for each object is stored
	// on-slab if NULL
	uint8_t *used;
}; // sizeof(struct slab) = 16

struct cache_t {
	struct slab_desc_t *full;
	struct slab_desc_t *partial;
	struct slab_desc_t *free;
	uint32_t obj_size; // objects that use (u)int32 or (u)int16 have to be 2/4 byte aligned
	uint16_t alignment; // alignment for the pages
	uint16_t flags;
	/*	Flags:
		bit 0: no_kmalloc
		bit 1: no_shrink
		bit 2: no_autogrow
		
		
		
		bit 14: poison: initialize objects with a pattern and check it before allocation
				and restore it after free
		bit 15: red zone: padd objects before and after with a pattern and 
				check it every now and then to see a buffer overflow
	*/
	char *name;
	struct cache_t *next;
}; // sizeof(struct cache) = 28

void* const kernel_heap_start;

void* getKernelHeapNextPage();
void setKernelHeapNextPage(void* next);

void addKernelHeapPage(void* page);


void initSlabAllocator();

void* alloc_object_from_slab(struct slab_desc_t* slab,uint32_t obj_size);
bool free_object_from_slab(struct slab_desc_t* slab,uint32_t obj_size,void *obj);
bool isSlabFree(struct slab_desc_t* slab,uint32_t obj_size);

void* alloc_object_from_cache(struct cache_t *cache);
bool free_object_from_cache(struct cache_t *cache,void* obj);

void print_cacheinfo();


void growCache(struct cache_t* cache);

struct cache_t* createCache(uint32_t obj_size,uint16_t alignment,uint16_t flags, char* name);


void* kmalloc(uint32_t size);
void kfree(void* obj);


// this one performs better, because it only searches the caches that are equal or bigger than the object size
void kfree_hint(void* obj,uint32_t size);



// check if partial slabs are actually free, update pre-cached objects..., for milis miliseconds at most
void slab_maintenance(uint32_t milis);


bool slab_allocator_self_test_pre_initialization();
bool slab_allocator_self_test_post_initialization();

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


struct process* requestProcess();
void freeProcess(void* proc);


struct thread* requestThread();	
void freeThread(void* thread);


uint32_t* request4Bytes();
void free4Bytes(uint32_t* b);






#endif