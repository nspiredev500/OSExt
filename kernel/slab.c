#include "kernel.h"


// slab allocator for the kernel


const void *kernel_heap_start = (const void*) 0xe8000000;
void* kernel_heap_next_page = (void*) (0xe8000000);


typedef struct cache_entry cache_entry;
typedef struct cache_entry {
	cache_entry *next;
	void *data;
};



static void addCacheEntry(cache_entry **list, cache_entry *e)
{
	if (*list == NULL)
	{
		*list = e;
		return;
	}
	e->next = *list;
	*list = e;
}

/*
static void addCacheEntry(cache_entry **list, cache_entry *e)
{
	if (*list == NULL)
	{
		*list = e;
		return;
	}
	cache_entry *next = *list;
	while (next->next != NULL)
	{
		next = next->next;
	}
	next->next = e;
}
*/

// return true if the cache entry was found (and removed)
static bool removeCacheEntry(cache_entry **list, cache_entry *e)
{
	if (*list == NULL)
	{
		return false;
	}
	if (*list == e)
	{
		*list = (*list)->next;
		return true;
	}
	cache_entry *next = *list;
	while (next != NULL)
	{
		if (next->next == e)
		{
			if (next->next == NULL)
			{
				next->next = NULL;
				return true;
			}
			next->next = (next->next)->next; // override the next value so it points after the removed entry
			return true;
		}
		next = next->next;
	}
	return false;
}

static cache_entry* removeCacheEntryByData(cache_entry **list, void *data)
{
	if (*list == NULL)
	{
		return NULL;
	}
	cache_entry *next = *list;
	while (next != NULL)
	{
		if (next->data == data)
		{
			if (next->next == NULL)
			{
				next->next = NULL;
				return next;
			}
			next->next = (next->next)->next; // override the next value so it points after the removed entry
			return next;
		}
		next = next->next;
	}
	return NULL;
}


// we need a cache for the cache entries, too
static cache_entry *unused_entries = NULL;
static uint32_t unused_entries_count = 0;

// coarse page table cache
static cache_entry *cpt_cache = NULL;
static cache_entry *cpt_cache_unused = NULL;


// address space struct cache
static cache_entry *address_space_cache = NULL;
static cache_entry *address_space_cache_unused = NULL;

// virtual address space cache
static cache_entry *translation_table_cache = NULL;
static cache_entry *translation_table_cache_unused = NULL;


static cache_entry *lcd_framebuffer_cache = NULL;
static cache_entry *lcd_framebuffer_cache_unused = NULL;


static cache_entry *process_cache = NULL;
static cache_entry *process_cache_unused = NULL;


static cache_entry *thread_cache = NULL;
static cache_entry *thread_cache_unused = NULL;

static cache_entry *linkedlist_cache = NULL;
static cache_entry *linkedlist_cache_unused = NULL;



static void refillCacheEntriesWithPage(void *page,cache_entry **cache,uint32_t size);
static void refillCacheEntries(cache_entry **cache,uint32_t size);
static void refillUnusedCacheEntriesWithPage(void *page);
static void refillUnusedCacheEntries();


// only supports small structures that fir into one page
static void refillCacheEntriesWithPage(void *page,cache_entry **cache,uint32_t size)
{
	if (unused_entries_count <= SMALL_PAGE_SIZE / size)
	{
		// TODO allocate new page and refill the cache entries
		panic("not enough empty cache entries left!\n");
	}
	for (int i = 0;i<SMALL_PAGE_SIZE;i+=size)
	{
		cache_entry *e = unused_entries;
		removeCacheEntry(&unused_entries,e);
		unused_entries_count--;
		e->data = page+i;
		addCacheEntry(cache,e);
	}
}



static void refillCacheEntries(cache_entry **cache,uint32_t size)
{
	if (unused_entries_count <= SMALL_PAGE_SIZE / size)
	{
		if (cache == &cpt_cache_unused)
		{
			panic("not enough unused cache entries and trying to refill cpt entries!");
		}
		refillUnusedCacheEntries();
	}
	
	
	
}

static void refillUnusedCacheEntriesWithPage(void *page)
{
	for (int i = 0;i<SMALL_PAGE_SIZE;i+=sizeof(cache_entry))
	{
		cache_entry *e = (page+i);
		e->data = NULL;
		e->next = NULL;
		addCacheEntry(&unused_entries,e);
		unused_entries_count++;
	}
}

static void refillUnusedCacheEntries()
{
	void *page = usePage();
	if (page == NULL)
	{
		panic("no free page for cache entries!\n");
	}
	addVirtualKernelPage(page,kernel_heap_next_page);
	kernel_heap_next_page += SMALL_PAGE_SIZE;
	refillUnusedCacheEntriesWithPage(page);
}

void initSlabAllocator()
{
	kernel_heap_next_page += SMALL_PAGE_SIZE;
	
	
	uint32_t *tmp_pds_unaligned = ti_malloc(256*4+1024*2);
	uint32_t *tmp_pds = make1KAligned(tmp_pds_unaligned);
	
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	uint32_t *tt = (uint32_t*) tt_base;
	
	
	k_memset(tmp_pds_unaligned,0,256*4+1024*2);
	
	
	
	tt[((uint32_t) kernel_heap_start)>>20] = newCPTD(0,tmp_pds);
	
	void* page = usePage();
	
	if (page == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[0] = newSPD(1,1,0b01010101,page);
	
	
	
	void* page2 = usePage();
	
	if (page2 == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[1] = newSPD(1,1,0b01010101,page2);
	
	
	
	void* page3 = usePage();
	
	if (page3 == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[2] = newSPD(1,1,0b01010101,page3);
	
	void* page4 = usePage();
	
	if (page4 == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[3] = newSPD(1,1,0b01010101,page4);
	
	
	clear_caches();
	
	invalidate_TLB();
	
	DEBUGPRINTF_1("first 4 pages allocated!\n");
	//asm(".long 0xE1212374"); // bkpt
	
	refillUnusedCacheEntriesWithPage(kernel_heap_start);
	refillCacheEntriesWithPage(kernel_heap_start+SMALL_PAGE_SIZE,&cpt_cache_unused,1024);
	refillUnusedCacheEntriesWithPage(kernel_heap_start+SMALL_PAGE_SIZE*2);
	refillCacheEntriesWithPage(kernel_heap_start+SMALL_PAGE_SIZE*3,&linkedlist_cache_unused,sizeof(LinkedList));
	kernel_heap_next_page += SMALL_PAGE_SIZE*4;
	
	
	
	migrateKernelCPT(kernel_heap_start,tmp_pds,4);
	
	
	
	
	ti_free(tmp_pds_unaligned);
}


void ensureFreeCacheEntries(cache_entry **cache)
{
	if (cache == &unused_entries)
	{
		
		return;
	}
	if (cache == &cpt_cache)
	{
		
		return;
	}
	if (cache == &address_space_cache)
	{
		
		return;
	}
	if (cache == &lcd_framebuffer_cache)
	{
		
		return;
	}
	if (cache == &process_cache)
	{
		
		return;
	}
	if (cache == &thread_cache)
	{
		
		return;
	}
}

static void* requestCacheEntry(cache_entry **cache_used,cache_entry **cache_unused)
{
	if (*cache_unused == NULL)
	{
		refillCacheEntries(cache_unused,1024);
	}
	cache_entry *c = *cache_unused;
	removeCacheEntry(cache_unused,c);
	addCacheEntry(cache_used,c);
	return c->data;
}

static void freeCacheEntry(cache_entry **cache_used,cache_entry **cache_unused,void *dat)
{
	cache_entry *e = removeCacheEntryByData(cache_unused,dat);
	if (e == NULL)
	{
		return;
	}
	addCacheEntry(cache_used,e);
}


LinkedList* requestLinkedListEntry()
{
	return (LinkedList*) requestCacheEntry(&linkedlist_cache,&linkedlist_cache_unused);
}

void freeLinkedListEntry(void* list)
{
	freeCacheEntry(&linkedlist_cache,&linkedlist_cache_unused,list);
}

uint32_t* requestCPT()
{
	return (uint32_t*) requestCacheEntry(&cpt_cache,&cpt_cache_unused);
}

void freeCPT(void* cpt)
{
	freeCacheEntry(&cpt_cache,&cpt_cache_unused,cpt);
}



struct address_space* requestAddressSpace()
{
	return (address_space*) requestCacheEntry(&address_space_cache,&address_space_cache_unused);
}


void freeAddressSpace(void *space)
{
	freeCacheEntry(&address_space_cache,&address_space_cache_unused,space);
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












































































