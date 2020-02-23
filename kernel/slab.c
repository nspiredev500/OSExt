#include "kernel.h"


// slab allocator for the kernel


void* const kernel_heap_start = (void* const) 0xe4000000;
static void* kernel_heap_next_page = (void*) (0xe4000000);


struct cache_t;
struct slab_desc_t;
struct slab_desc_t {
	uint32_t size; // number of pages
	void *start; // start of the slab in memory
	struct slab_desc_t *next;
	uint8_t *used; // pointer to the array where the used bit for each object is stored
	// on-slab if NULL
}; // sizeof(struct slab) = 16

struct cache_t {
	struct slab_desc_t *full;
	struct slab_desc_t *partial;
	struct slab_desc_t *free;
	uint32_t obj_size;
	uint32_t flags; // bit 0 = essential (has to have one free slab at all time)
	char *name;
	struct cache_t *next;
}; // sizeof(struct cache) = 28


struct cache* caches = NULL;




void* getKernelHeapNextPage()
{
	return kernel_heap_next_page;
}
void setKernelHeapNextPage(void* next)
{
	kernel_heap_next_page = next;
}

void growCache(struct cache_t* cache)
{
	if (cache->obj_size > SMALL_PAGE_SIZE)
	{
		void* page = usePage();
		if (page == NULL)
		{
			panic("no page for a growing cache found!");
		}
		// TODO addVirtualKernelPage uses the slab allocator and if the cpt or linkedlist caches need to grow,
		// it would result in an endless loop
		addVirtualKernelPage();
	}
	else
	{
		
	}
}

void* alloc_object_from_cache(struct cache_t* cache)
{
	if (cache->partial == NULL)
	{
		if (cache->free == NULL)
		{
			growCache(cache);
			return alloc_object_from_cache(cache);
		}
		else
		{
			// remove the slab from the freelist
			struct slab_desc_t *freeslab = cache->free;
			cache->free = freeslab->next;
			// add it to the partial list
			struct slab_desc_t *partialslab = cache->partial;
			freeslab->next = cache->partial;
			cache->partial = freeslab;
			return alloc_object_from_slab(freeslab,cache->obj_size);
		}
	}
	else
	{
		struct slab_desc_t *cslab = cache->partial;
		while (cslab != NULL)
		{
			void *object = alloc_object_from_slab(cslab,cache->obj_size);
			if (object != NULL)
			{
				return object;
			}
			// TODO add the slab to the full list, because there was no object returned
			cslab = cslab->next;
		}
		growCache(cache);
		return alloc_object_from_cache(cache);
	}
}


void* alloc_object_from_slab(struct slab_desc_t* slab,uint32_t obj_size)
{
	if (obj_size < 128)
	{
		// slab descriptor on-slab
		uint32_t objs_raw = (slab->size*SMALL_PAGE_SIZE) / obj_size;
		// don't subtract slab descriptor size to account for more objects if calculations are wrong
		
		uint32_t objs = (slab->size*SMALL_PAGE_SIZE-(objs_raw/8+1))/obj_size;
		void *objects_start = slab->start + sizeof(slab_desc_t) + (objs_raw / obj_size)/8+1;
		// offset = sizeof(slab descriptor) + length of used array
		uint8_t *used = slab->start+sizeof(slab_desc_t);
		uint32_t i = 0;
		for (uint32_t i = 0;i<objs;i++)
		{
			uint8_t bit = i%8;
			if (((used[i/8] >> bit ) & 0b1) == 0b0)
			{
				used[i/8] = used[i/8] | (0b1 << bit);
				return objects_start+i*obj_size;
			}
		}
		return NULL;
	}
	else
	{
		// slab descriptor off-slab
		uint8_t *used = slab->used;
		uint32_t objs = (slab->size*SMALL_PAGE_SIZE) / obj_size;
		for (uint32_t i = 0;i<objs;i++)
		{
			uint8_t bit = i%8;
			if (((used[i/8] >> bit ) & 0b1) == 0b0)
			{
				used[i/8] = used[i/8] | (0b1 << bit);
				return slab->start+i*obj_size;
			}
		}
		return NULL;
	}
}

// allocates an object from a cache, using a larger one if no perfectly fitting one is found and returning NULL if
// no cache with enough object size was found 
void* kmalloc(uint32_t size)
{
	struct cache_t *best_fit_cache = NULL;
	struct cache_t *ccache = caches;
	while (ccache != NULL)
	{
		if (ccache->obj_size == size)
		{
			best_fit_cache = ccache;
			break;
		}
		if (ccache->obj_size > size)
		{
			best_fit_cache = ccache;
		}
		ccache = ccache->next;
	}
	if (ccache == NULL)
	{
		return NULL;
	}
	else
	{
		return alloc_object_from_cache(best_fit_cache);
	}
}

void createCache(uint32_t obj_size,uint32_t flags, char* name)
{
	
	
	
	
}

void initSlabAllocator()
{
	void* page = usePage();
	if (page == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		return;
	}
	
	
	uint32_t *tmp_pds_unaligned = ti_malloc(256*4+1024*2);
	uint32_t *tmp_pds = make1KAligned(tmp_pds_unaligned);
	k_memset(tmp_pds,0,1024);
	
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	uint32_t *tt = (uint32_t*) tt_base;
	
	tt[kernel_heap_start >> 20] = newCPTD(0,(uint32_t) tmp_pds);
	
	tmp_pds[0] = newSPD(1,1,0b01010101,(uint32_t) page);
	
	
	page = usePage();
	if (page == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[1] = newSPD(1,1,0b01010101,(uint32_t) page);
	
	
	page = usePage();
	if (page == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[2] = newSPD(1,1,0b01010101,(uint32_t) page);
	
	page = usePage();
	if (page == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[3] = newSPD(1,1,0b01010101,(uint32_t) page);
	invalidateTLB();
	
	
	
	struct slab_desc_t *cache_slab = kernel_heap_start;
	kernel_heap_next_page += SMALL_PAGE_SIZE;
	
	cache_slab->size = 1;
	cache_slab->start = cache_slab;
	cache_slab->next = NULL;
	cache_slab->used = NULL;
	
	
	struct cache_t *cache_cache = alloc_object_from_slab(cache_slab,sizeof(struct cache_t));
	if (cache_cache == NULL)
	{
		DEBUGPRINTF_1("no free entry in created cache!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	
	cache_cache->full = NULL;
	cache_cache->partial = NULL;
	cache_cache->free = cache_slab;
	cache_cache->obj_size = sizeof(struct cache_t);
	cache_cache->flags = 0;
	cache_cache->name = "cache_cache";
	cache_cache->next = NULL;
	
	
	struct slab_desc_t *slab_slab = kernel_heap_next_page;
	kernel_heap_next_page += SMALL_PAGE_SIZE;
	
	slab_slab->size = 1;
	slab_slab->start = slab_slab;
	slab_slab->next = NULL;
	slab_slab->used = NULL;
	
	struct cache_t *slab_cache = alloc_object_from_slab(cache_slab,sizeof(struct cache_t));
	if (slab_cache == NULL)
	{
		DEBUGPRINTF_1("no free entry in created cache!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	
	slab_cache->full = NULL;
	slab_cache->partial NULL;
	slab_cache->free = slab_slab;
	slab_cache->obj_size = sizeof(struct slab_desc_t);
	slab_cache->flags = 0;
	slab_cache->name = "slab_cache";
	slab_cache->next = NULL;
	
	
	struct slab_desc_t *linkedlist_slab = kernel_heap_next_page;
	kernel_heap_next_page += SMALL_PAGE_SIZE;
	
	linkedlist_slab->size = 1;
	linkedlist_slab->start = linkedlist_slab;
	linkedlist_slab->next = NULL;
	linkedlist_slab->used = NULL;
	
	struct cache_t *linkedlist_cache = alloc_object_from_slab(cache_slab,sizeof(struct cache_t));
	if (linkedlist_cache == NULL)
	{
		DEBUGPRINTF_1("no free entry in created cache!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	
	linkedlist_cache->full = NULL;
	linkedlist_cache->partial NULL;
	linkedlist_cache->free = linkedlist_slab;
	linkedlist_cache->obj_size = sizeof(LinkedList);
	linkedlist_cache->flags = 0;
	linkedlist_cache->name = "linkedlist_cache";
	linkedlist_cache->next = NULL;
	
	
	
	struct slab_desc_t *cpt_slab = alloc_object_from_slab(slab_slab,sizeof(struct slab_desc_t));
	if (cpt_slab == NULL)
	{
		DEBUGPRINTF_1("no free entry in created cache!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	
	cpt_slab->size = 1;
	cpt_slab->start = kernel_heap_next_page;
	kernel_heap_next_page += SMALL_PAGE_SIZE;
	cpt_slab->next = NULL;
	
	// SMALL_PAGE_SIZE/1024 = 4, so only one byte needed, but the size caches aren't created jet,
	// so just use the linkedlist cache, as it is the smallest (8 bytes)
	cpt_slab->used = alloc_object_from_slab(linkedlist_slab,sizeof(LinkedList));
	if (cpt_slab->used == NULL)
	{
		DEBUGPRINTF_1("no free entry in created cache!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	
	
	
	struct cache_t *cpt_cache = alloc_object_from_slab(cache_slab,sizeof(struct cache_t));
	if (cpt_cache == NULL)
	{
		DEBUGPRINTF_1("no free entry in created cache!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	
	cpt_cache->full = NULL;
	cpt_cache->partial NULL;
	cpt_cache->free = cpt_slab;
	cpt_cache->obj_size = 1024;
	cpt_cache->flags = 0;
	cpt_cache->name = "cpt_cache";
	cpt_cache->next = NULL;
	
	// initialize the cache linkedlist
	caches = cache_cache;
	cache_cache->next = slab_cache;
	slab_cache->next = linkedlist_cache;
	linkedlist_cache->next = cpt_cache;
	cpt_cache->next = NULL;
	
	
	
	// migrate the mapping to the virtual memory manager, so the following code can usePage
	// addVirtualKernelPage, as it is used in the cache creation
	migrateKernelCPT((uint32_t) kernel_heap_start,tmp_pds,4);
	
	
	
	
	
	
	
	
	
	
	
	
	asm(".long 0xE1212374"); // bkpt
}



bool slab_allocator_self_test()
{
	
}




void slab_maintenance(uint32_t milis) // maybe specify a point in the future instead, so it can exit when that specific point is reached
{
	/* TODO
		- check if partial slabs are empty and put them in the freelist
		- make LinkedList of precached objects for faster allocation
		- free some free slabs in a cache of there are many
		- while doing that, check the timer and put the slab allocator in a usable state and exit if the time has been reached
	*/
	
	
	// disable fiq and irq (irq should always be disabled in the kernel, because it relays to the os, which isn't always mapped) while cleaning
	// to keep the allocator in a usable state
	// maybe the interrupt handlers need objects
	
	
	
}

void ensureCPTCapacity()
{
	
}
void ensureLinkedListCapacity()
{
	
}






LinkedList* requestLinkedListEntry()
{
	
}
void freeLinkedListEntry(void* list)
{
	
}

uint32_t* requestCPT()
{
	
}
void freeCPT(void* cpt)
{
	
}


struct address_space* requestAddressSpace()
{
	
}
void freeAddressSpace(void *space)
{
	
}


uint32_t* requestTranslationTable()
{
	
}
void freeTranslationTable(uint32_t *tt)
{
	
}


void* requestLCDFramebuffer()
{
	
}
void freeLCDFramebuffer(void* buff)
{
	
}


struct Process* requestProcess()
{
	
}
void freeProcess(void* proc)
{
	
}


struct Thread* requestThread()
{
	
}
void freeThread(void* thread)
{
	
}


uint32_t* request4Bytes()
{
	
}
void free4Bytes(uint32_t* b)
{
	
}












































































