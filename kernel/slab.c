#include "kernel.h"


// slab allocator for the kernel


void* const kernel_heap_start = (void* const) 0xe4000000;
static void* kernel_heap_next_page = (void*) (0xe4000000);



void* getKernelHeapNextPage()
{
	return kernel_heap_next_page;
}
void setKernelHeapNextPage(void* next)
{
	kernel_heap_next_page = next;
}


typedef struct cache_entry cache_entry;
typedef struct cache_entry {
	cache_entry *next;
	void *data;
} cache_entry;



static void addCacheEntry(cache_entry **list, cache_entry *e)
{
	if (*list == NULL)
	{
		e->next = NULL;
		*list = e;
		return;
	}
	//asm(".long 0xE1212374"); // bkpt
	e->next = *list;
	
	*list = e;
}


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

static cache_entry *uint32_cache = NULL;
static cache_entry *uint32_cache_unused = NULL;




static void refillCacheEntriesWithPage(void *page,cache_entry **cache,uint32_t size);
static void refillCacheEntries(cache_entry **cache,uint32_t size);
static void refillUnusedCacheEntriesWithPage(void *page);
static void refillUnusedCacheEntries();


// only supports small structures that fir into one page
static void refillCacheEntriesWithPage(void *page,cache_entry **cache,uint32_t size)
{
	DEBUGPRINTF_3("page used for refilling: 0x%x\n",page);
	if (unused_entries_count <= SMALL_PAGE_SIZE / size)
	{
		refillUnusedCacheEntries();
		//panic("not enough empty cache entries left!\n");
	}
	for (uint32_t i = 0;i+size<SMALL_PAGE_SIZE;i+=size) // - SMALL_PAGE_SIZE % size to make it fit into the page and not accidentally overflow
	{
		cache_entry *e = unused_entries;
		if (e == NULL)
		{
			refillUnusedCacheEntries();
			i -= size;
			continue;
			//panic("no empty cache entries left!\n");
		}
		removeCacheEntry(&unused_entries,e);
		unused_entries_count--;
		e->data = page+i;
		addCacheEntry(cache,e);
	}
}
void ensureCPTCapacity()
{
	refillCacheEntries(&cpt_cache_unused,1024);
}

void ensureLinkedListCapacity()
{
	refillCacheEntries(&linkedlist_cache_unused,sizeof(LinkedList));
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
	// filter out all the things that need more than one page
	if (size > SMALL_PAGE_SIZE)
	{
		uint32_t alignment = 0;
		if (cache == &translation_table_cache_unused)
		{
			alignment = 1024*16;
		}
		refillUnusedCacheEntries();
		refillUnusedCacheEntries();
		refillCacheEntries(&cpt_cache_unused,1024);
		refillCacheEntries(&linkedlist_cache_unused,sizeof(LinkedList));
		refillUnusedCacheEntries();
		refillUnusedCacheEntries();
		refillCacheEntries(&cpt_cache_unused,1024);
		refillCacheEntries(&linkedlist_cache_unused,sizeof(LinkedList));
		// caches can't be refilled while remapping the block
		
		void *data = useConsecutivePages(size/SMALL_PAGE_SIZE,alignment);
		DEBUGPRINTF_3("page base for entry: 0x%x\n",data);
		DEBUGPRINTF_1("cache entry size: 0x%x\n",size)
		if (data == NULL)
		{
			panic("not enough consecutive pages for cache entry found!\n");
		}
		
		
		
		cache_entry *e = unused_entries;
		removeCacheEntry(&unused_entries,e);
		unused_entries_count--;
		
		e->data = kernel_heap_next_page;
		
		
		// map the pages to the heap
		void* pagecounter = data;
		void* heap_pagecounter = kernel_heap_next_page;
		
		kernel_heap_next_page += size;
		
		DEBUGPRINTF_1("remapping pages for cache entry\n")
		for (uint32_t i = 0;i<size/SMALL_PAGE_SIZE;i++)
		{
			addVirtualKernelPage(pagecounter,heap_pagecounter);
			heap_pagecounter += SMALL_PAGE_SIZE;
			pagecounter += SMALL_PAGE_SIZE;
		}
		DEBUGPRINTF_1("finished remapping pages\n")
		
		
		
		addCacheEntry(cache,e);
		return;
	}
	void *page = usePage();
	if (page == NULL)
	{
		panic("no free page for cache entries!\n");
	}
	addVirtualKernelPage(page,kernel_heap_next_page);
	kernel_heap_next_page += SMALL_PAGE_SIZE;
	refillCacheEntriesWithPage(kernel_heap_next_page-SMALL_PAGE_SIZE,cache,size);
}

static void refillUnusedCacheEntriesWithPage(void *page)
{
	DEBUGPRINTF_3("page used for refilling: 0x%x\n",page);
	k_memset(page,0,SMALL_PAGE_SIZE);
	for (uint32_t i = 0;i+sizeof(cache_entry)<SMALL_PAGE_SIZE;i+=sizeof(cache_entry))
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
	refillUnusedCacheEntriesWithPage(kernel_heap_next_page-SMALL_PAGE_SIZE);
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
	
	
	
	tt[(uint32_t) kernel_heap_start >> 20] = newCPTD(0,(uint32_t) tmp_pds);
	
	void* page = usePage();
	
	if (page == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[0] = newSPD(1,1,0b01010101,(uint32_t) page);
	
	
	
	void* page2 = usePage();
	
	if (page2 == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[1] = newSPD(1,1,0b01010101,(uint32_t) page2);
	
	
	
	void* page3 = usePage();
	
	if (page3 == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[2] = newSPD(1,1,0b01010101,(uint32_t) page3);
	
	void* page4 = usePage();
	
	if (page4 == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[3] = newSPD(1,1,0b01010101,(uint32_t) page4);
	
	
	clear_caches();
	
	invalidate_TLB();
	
	DEBUGPRINTF_1("first 4 pages allocated!\n");
	//asm(".long 0xE1212374"); // bkpt
	
	refillUnusedCacheEntriesWithPage(kernel_heap_start);
	refillCacheEntriesWithPage(kernel_heap_start+SMALL_PAGE_SIZE,&cpt_cache_unused,1024);
	refillUnusedCacheEntriesWithPage(kernel_heap_start+SMALL_PAGE_SIZE*2);
	refillCacheEntriesWithPage(kernel_heap_start+SMALL_PAGE_SIZE*3,&linkedlist_cache_unused,sizeof(LinkedList));
	kernel_heap_next_page += SMALL_PAGE_SIZE*4;
	
	
	migrateKernelCPT((uint32_t) kernel_heap_start,tmp_pds,4);
	
	//debug_shell_println_rgb("kernel heap next1: 0x%x",255,0,0,kernel_heap_next_page);
	
	
	ti_free(tmp_pds_unaligned);
}



static void* requestCacheEntry(cache_entry **cache_used,cache_entry **cache_unused,uint32_t size)
{
	if (*cache_unused == NULL)
	{
		refillCacheEntries(cache_unused,size);
	}
	// refill the cpt cache even before it is empty, because you need cpts to refill the unused entries and could get into a loop
	if (cache_unused == &cpt_cache_unused && (cpt_cache_unused->next == NULL || cpt_cache_unused->next->next == NULL))
	{
		refillCacheEntries(cache_unused,size);
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







bool slab_allocator_self_test()
{
	
	
	
	
	
	
	return true;
}




























LinkedList* requestLinkedListEntry()
{
	return (LinkedList*) requestCacheEntry(&linkedlist_cache,&linkedlist_cache_unused,sizeof(LinkedList));
}

void freeLinkedListEntry(void* list)
{
	freeCacheEntry(&linkedlist_cache,&linkedlist_cache_unused,list);
}


uint32_t* requestCPT()
{
	return (uint32_t*) requestCacheEntry(&cpt_cache,&cpt_cache_unused,1024);
}

void freeCPT(void* cpt)
{
	freeCacheEntry(&cpt_cache,&cpt_cache_unused,cpt);
}


struct address_space* requestAddressSpace()
{
	return (struct address_space*) requestCacheEntry(&address_space_cache,&address_space_cache_unused,sizeof(struct address_space));
}

void freeAddressSpace(void *space)
{
	freeCacheEntry(&address_space_cache,&address_space_cache_unused,space);
}


uint32_t* requestTranslationTable()
{
	return (uint32_t*) requestCacheEntry(&translation_table_cache,&translation_table_cache_unused,1024*16);
}

void freeTranslationTable(uint32_t *tt)
{
	return freeCacheEntry(&translation_table_cache,&translation_table_cache_unused,tt);
}


void* requestLCDFramebuffer()
{
	return requestCacheEntry(&lcd_framebuffer_cache,&lcd_framebuffer_cache_unused,SMALL_PAGE_SIZE*37); // 320*240*2
}

void freeLCDFramebuffer(void* buff)
{
	freeCacheEntry(&lcd_framebuffer_cache,&lcd_framebuffer_cache_unused,buff);
}


struct Process* requestProcess()
{
	return (struct Process*) requestCacheEntry(&process_cache,&process_cache_unused,sizeof(struct Process));
}

void freeProcess(void* proc)
{
	freeCacheEntry(&process_cache,&process_cache_unused,proc);
}


struct Thread* requestThread()
{
	return (struct Thread*) requestCacheEntry(&thread_cache,&thread_cache_unused,sizeof(struct Thread));
}

void freeThread(void* thread)
{
	freeCacheEntry(&thread_cache,&thread_cache_unused,thread);
}


uint32_t* request4Bytes()
{
	return (uint32_t*) requestCacheEntry(&uint32_cache,&uint32_cache_unused,sizeof(uint32_t));
}

void free4Bytes(uint32_t* b)
{
	freeCacheEntry(&uint32_cache,&uint32_cache_unused,b);
}











