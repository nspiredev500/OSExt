#include "kernel.h"


// slab allocator for the kernel


const void *kernel_heap_start = (const void*) 0xe8000000;

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
	if (*list == e)
	{
		*list = (*list)->next;
		return true;
	}
	cache_entry *next = *list;
	while (next->next != NULL)
	{
		if (next->next == e)
		{
			next->next = (next->next)->next; // override the next value so it points after the removed entry
			return true;
		}
		next = next->next;
	}
	return false;
}

// we need a cache for the cache entries, too
static cache_entry *unused_entries = NULL;
static uint32_t unused_entries_count = 0;

// coarse page table cache
static cache_entry *cpt_cache = NULL;
static cache_entry *cpt_cache_unused = NULL;


// virtual address space cache
static cache_entry *address_space_cache = NULL;
static cache_entry *address_space_cache_unused = NULL;


static cache_entry *lcd_framebuffer_cache = NULL;
static cache_entry *lcd_framebuffer_cache_unused = NULL;


static cache_entry *process_cache = NULL;
static cache_entry *process_cache_unused = NULL;


static cache_entry *thread_cache = NULL;
static cache_entry *thread_cache_unused = NULL;



static void refillCacheEntries(void *page,cache_entry **cache,uint32_t size)
{
	if (unused_entries_count <= SMALL_PAGE_SIZE / size)
	{
		// TODO allocate new page and refill the cache entries
		
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

static void refillUnusedCacheEntries(void *page)
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


void initSlabAllocator()
{
	uint32_t *tmp_pds_unaligned = ti_malloc(256*4+16);
	uint32_t *tmp_pds = (uint32_t*) ((((uint32_t)tmp_pds_unaligned) & (~ 0b11))+ 0b100);
	
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	uint32_t *tt = (uint32_t*) tt_base;
	
	
	k_memset(tmp_pds_unaligned,0,256*4+16);
	
	
	
	tt[((uint32_t) kernel_heap_start)>>20] = newCPTD(0,tmp_pds);
	
	void* page = usePage();
	
	if (page == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[0] = newSPD(1,1,0b01010101,page);
	
	
	
	page = usePage();
	
	if (page == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[1] = newSPD(1,1,0b01010101,page);
	
	invalidate_TLB();
	
	
	
	refillUnusedCacheEntries(kernel_heap_start);
	refillCacheEntries(kernel_heap_start+SMALL_PAGE_SIZE,&cpt_cache_unused,1024);
	
	cache_entry *e = cpt_cache_unused;
	if (e == NULL)
	{
		DEBUGPRINTF_1("no coarse page table available for allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	removeCacheEntry(&cpt_cache_unused,e);
	addCacheEntry(&cpt_cache,e);
	
	k_memcpy(e->data,tmp_pds,sizeof(uint32_t)*2);
	tt[((uint32_t) kernel_heap_start)>>20] = newCPTD(0,e->data);
	
	invalidate_TLB();
	
	
	
	
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


uint32_t* requestCPT()
{
	
	
	
}

void freeCPT(void* cpt)
{
	
	
	
	
}



uint32_t* requestAddressSpace()
{
	
	
	
	
}


void freeAddressSpace(void *space)
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












































































