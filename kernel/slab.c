#include "kernel.h"


// slab allocator for the kernel


void* const kernel_heap_start = (void* const) 0xe4000000;
static void* kernel_heap_next_page = (void*) (0xe4000000);

/*
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
	uint32_t obj_size;
	uint16_t alignment; // alignment for the pages
	uint16_t flags;
	char *name;
	struct cache_t *next;
}; // sizeof(struct cache) = 28
*/

struct cache_t *caches = NULL;


// keep a pointer to each essential cache for faster access
struct cache_t *slab_cache = NULL;
struct cache_t *cache_cache = NULL;
struct cache_t *linkedlist_cache = NULL;
struct cache_t *cpt_cache = NULL;
struct cache_t *tt_cache = NULL;
struct cache_t *address_space_cache = NULL;
struct cache_t *process_cache = NULL;
struct cache_t *thread_cache = NULL;
struct cache_t *framebuffer_cache = NULL;



static LinkedList* big_objects = NULL; // list of big object allocated with kmalloc
static LinkedList* big_objects_size = NULL; // list of big object allocated with kmalloc

static struct slab_desc_t* requestSlabDesc()
{
	return alloc_object_from_cache(slab_cache);
}

static void destroySlabDesc(struct slab_desc_t* slab)
{
	free_object_from_cache(slab_cache,slab);
}

void addKernelHeapPage(void* page)
{
	addVirtualKernelPage(page,kernel_heap_next_page);
	kernel_heap_next_page += SMALL_PAGE_SIZE;
}


// adds a SPD for the page in the CPT, that is not tracked by the virtual memory manager
// allocates a CPT temporarily if no one is there

static void addUnlistedKernelPage(void* page, void* virtual_address,uint32_t** cpt,uint32_t** cpt_unaligned)
{
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	uint32_t *tt = (uint32_t*) tt_base;
	
	
	
	
	
	uint32_t* kernel_cpt = getKernelCPT(virtual_address);
	
	uint32_t section = (uint32_t) virtual_address & (~ 0xfffff);
	if (kernel_cpt != NULL)
	{
		kernel_cpt[((uint32_t) virtual_address - section)/SMALL_PAGE_SIZE] = newSPD(1,1,0b01010101,(uint32_t) page);
	}
	else
	{
		
		
		register uint32_t tt_base asm("r0");
		asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
		
		tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
		uint32_t *tt = (uint32_t*) (tt_base+ (uint32_t) remapped_RAM);
		
		if ((tt[section] & 0b11) == 0b01) // already an coarse page table
		{
			uint32_t *cpt_base = (uint32_t*) ((tt[section] & (~ 0b1111111111)) + remapped_RAM);
			
			cpt_base[(uint32_t) ((uint32_t) virtual_address-section)/SMALL_PAGE_SIZE] = newSPD(1,1,0b01010101,(uint32_t) page);
			
			return;
		}
		intoKernelSpaceSaveAddressSpace();
		uint32_t *tmp_pds_unaligned = ti_malloc(256*4+1024*2);
		if (tmp_pds_unaligned == NULL)
		{
			panic("no memory for tmp_pds!\n");
		}
		uint32_t *tmp_pds = make1KAligned(tmp_pds_unaligned);
		k_memset(tmp_pds,0,1024);
		*cpt = tmp_pds;
		*cpt_unaligned = tmp_pds_unaligned;
		tt[section] = newCPTD(0,(uint32_t) tmp_pds);
		
		tmp_pds[(uint32_t) ((uint32_t) virtual_address-section)/SMALL_PAGE_SIZE] = newSPD(1,1,0b01010101,(uint32_t) page);
		
		
		restoreAddressSpace();
	}
	
	
	
	
}





/*
static void addUnlistedKernelPage(void* page, void* virtual_address,uint32_t** cpt)
{
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	uint32_t *tt = (uint32_t*) tt_base;
	if (tt[(uint32_t)virtual_address>>20] && 0b11 == 0)
	{
		uint32_t *tmp_pds_unaligned = ti_malloc(256*4+1024*2);
		if (tmp_pds_unaligned == NULL)
		{
			panic("no memory for tmp_pds!\n");
		}
		uint32_t *tmp_pds = make1KAligned(tmp_pds_unaligned);
		k_memset(tmp_pds,0,1024);
		*cpt = tmp_pds_unaligned;
		uint32_t index = ((((uint32_t) virtual_address) & 0b11111111000000000000) >> 10)/4;
		uint32_t* cpt_real = tmp_pds;
		cpt_real[index] = newSPD(1,1,0b01010101,(uint32_t) page);;
	}
	else
	{
		uint32_t descriptor = tt[(uint32_t)virtual_address>>20];
		if ((descriptor  & 0b11) != 0b01)
		{
			panic("addUnlistedKernelPage: not a coarse page table descriptor!\n");
		}
		uint32_t *cpt_base = (uint32_t*) (descriptor & (~ 0b1111111111));
		uint32_t index = ((((uint32_t) virtual_address) & 0b11111111000000000000) >> 10)/4;
		cpt_base[index] = newSPD(1,1,0b01010101,(uint32_t) page);
	}
}
*/


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
	DEBUGPRINTF_1("growing cache: ")
	DEBUGPRINTF_1(cache->name)
	DEBUGPRINTF_1("\n")
	// TODO disable fiqs while handling the unlisted kernel pages, so that if a new address space gets created, they will be also copied into that
	if (cache->obj_size < SMALL_PAGE_SIZE)
	{
		
		void* page = usePage();
		if (page == NULL)
		{
			panic("no page for a growing cache found!");
		}
		// TODO addVirtualKernelPage uses the slab allocator and if the cpt or linkedlist caches need to grow,
		// it would result in an endless loop
		//addVirtualKernelPage();
		uint32_t* cpt = NULL;
		uint32_t* cpt_unaligned = NULL;
		addUnlistedKernelPage(page,kernel_heap_next_page,&cpt,&cpt_unaligned);
		void *virtpage = kernel_heap_next_page;
		k_memset(virtpage,0,SMALL_PAGE_SIZE);
		kernel_heap_next_page += SMALL_PAGE_SIZE;
		
		struct slab_desc_t *slab;
		if (cache->obj_size >= 128)
		{
			slab = requestSlabDesc();
		}
		else
		{
			slab = virtpage;
		}
		
		slab->size = 1;
		slab->start = virtpage;
		slab->next = NULL;
		slab->used = NULL;
		if (cache->obj_size >= 128)
		{
			slab->used = kmalloc(((SMALL_PAGE_SIZE/cache->obj_size)/8)+1);
			if (slab->used == NULL)
			{
				panic("cache growth: could not kmalloc the used array for the slab!\n");
			}
		}
		// add it to the cache
		slab->next = cache->free;
		cache->free = slab;
		
		
		
		// page is added to the slab allocator, now add it to the virtual memory manager
		if (cpt != NULL)
		{
			migrateKernelCPT((uint32_t) page,cpt,256);
			intoKernelSpaceSaveAddressSpace();
			ti_free(cpt_unaligned);
			restoreAddressSpace();
		}
		else
		{
			addVirtualKernelPage(page,virtpage);
		}
		
		return;
	}
	else
	{
		uint32_t size = cache->obj_size / SMALL_PAGE_SIZE;
		if (cache->obj_size % SMALL_PAGE_SIZE != 0)
		{
			size++;
		}
		void *page = useConsecutivePages(size,cache->alignment);
		if (page == NULL)
		{
			panic("not enough consecutive pages for cache growth!\n");
		}
		
		void* virtpage = kernel_heap_next_page;
		kernel_heap_next_page += SMALL_PAGE_SIZE*size;
		
		// no object should be bigger than 100 pages
		uint32_t* cpts[100];
		uint32_t* cpts_unaligned[100];
		if (size >= 100)
		{
			panic("cache growth: object bigger than 100 pages!\n");
		}
		
		for (uint32_t i = 0;i<100;i++)
		{
			cpts[i] = NULL;
		}
		
		for (uint32_t i = 0;i<size;i++)
		{
			addUnlistedKernelPage(page+SMALL_PAGE_SIZE*i,virtpage+SMALL_PAGE_SIZE*i,&(cpts[i]),&(cpts_unaligned[i]));
		}
		k_memset(virtpage,0,SMALL_PAGE_SIZE*size);
		
		struct slab_desc_t *slab = kmalloc(sizeof(struct slab_desc_t));
		if (slab == NULL)
		{
			panic("cache growth: could not kmalloc a slab descriptor!\n");
		}
		uint32_t objs = (SMALL_PAGE_SIZE*size) / cache->obj_size;
		slab->size = size;
		slab->start = virtpage;
		slab->next = NULL;
		slab->used = kmalloc((objs/8)+1);
		if (slab->used == NULL)
		{
			panic("cache growth: could not kmalloc the used array for the slab!\n");
		}
		
		// add it to the cache
		slab->next = cache->free;
		cache->free = slab;
		
		
		
		// now migrate all the pages to the virtual memory manager
		for (uint32_t i = 0;i<100;i++)
		{
			if (cpts[i] != NULL)
			{
				migrateKernelCPT(((uint32_t) page)+i*SMALL_PAGE_SIZE,cpts[i],256);
				intoKernelSpaceSaveAddressSpace();
				ti_free(cpts_unaligned[i]);
				restoreAddressSpace();
			}
			else
			{
				addVirtualKernelPage(page,virtpage);
			}
		}
	}
}

bool free_object_from_cache(struct cache_t *cache,void* obj)
{
	if (cache->partial == NULL)
	{
		if (cache->full == NULL)
		{
			return false;
		}
		else
		{
			struct slab_desc_t *cslab = cache->full;
			struct slab_desc_t *prevslab = NULL;
			while (cslab != NULL)
			{
				if (free_object_from_slab(cslab,cache->obj_size,obj))
				{
					if (prevslab == NULL)
					{
						cache->full = cslab->next;
					}
					else
					{
						prevslab->next = cslab->next;
					}
					cslab->next = cache->partial;
					cache->partial = cslab;
					return true;
				}
				prevslab = cslab;
				cslab = cslab->next;
			}
		}
	}
	else
	{
		struct slab_desc_t *cslab = cache->full;
		struct slab_desc_t *prevslab = NULL;
		while (cslab != NULL)
		{
			if (free_object_from_slab(cslab,cache->obj_size,obj))
			{
				if (prevslab == NULL)
				{
					cache->full = cslab->next;
				}
				else
				{
					prevslab->next = cslab->next;
				}
				cslab->next = cache->partial;
				cache->partial = cslab;
				return true;
			}
			prevslab = cslab;
			cslab = cslab->next;
		}
		cslab = cache->partial;
		prevslab = NULL;
		while (cslab != NULL)
		{
			if (free_object_from_slab(cslab,cache->obj_size,obj))
			{
				if (isSlabFree(cslab,cache->obj_size))
				{
					if (prevslab == NULL)
					{
						cache->partial = cslab->next;
					}
					else
					{
						prevslab->next = cslab->next;
					}
					cslab->next = cache->free;
					cache->free = cslab;
				}
				return true;
			}
			prevslab = cslab;
			cslab = cslab->next;
		}
	}
	return false;
}


void* alloc_object_from_cache(struct cache_t *cache)
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
			void *obj = alloc_object_from_slab(freeslab,cache->obj_size);
			if (obj == NULL)
			{
				panic("could not get object from empty slab!\n");
			}
			return obj;
		}
	}
	else
	{
		struct slab_desc_t *cslab = cache->partial;
		struct slab_desc_t *prevslab = NULL;
		while (cslab != NULL)
		{
			void *object = alloc_object_from_slab(cslab,cache->obj_size);
			if (object != NULL)
			{
				return object;
			}
			// add it to the full list
			if (prevslab == NULL)
			{
				cache->partial = cslab->next;
			}
			else
			{
				prevslab->next = cslab->next;
			}
			cslab->next = cache->full;
			cache->full = cslab;
			
			
			prevslab = cslab;
			cslab = cslab->next;
		}
		cslab = cache->free;
		if (cslab == NULL)
		{
			growCache(cache);
			return alloc_object_from_cache(cache);
		}
		cache->free = cslab->next;
		cslab->next = cache->partial;
		cache->partial = cslab;
		void *obj = alloc_object_from_slab(cslab,cache->obj_size);
		if (obj == NULL)
		{
			panic("could not get object from empty slab!\n");
		}
		return obj;
	}
}
// returns true if the object was on this slab and freed
bool free_object_from_slab(struct slab_desc_t* slab,uint32_t obj_size,void *obj)
{
	if (obj_size < 128)
	{
		uint32_t objs_raw = (slab->size*SMALL_PAGE_SIZE) / obj_size;
		void *objects_start = slab->start + sizeof(struct slab_desc_t) + objs_raw/8+1;
		uint32_t unalignment = (uint32_t) objects_start & 0b11;
		objects_start = align4Bytes(objects_start);
		uint32_t objs = (uint32_t) (slab->start+slab->size*SMALL_PAGE_SIZE-objects_start)/obj_size;
		
		int64_t offset = ((int32_t)obj)-((int32_t)objects_start);
		if (offset < 0 || offset/obj_size > objs)
		{
			return false;
		}
		uint8_t *used = slab->start+sizeof(struct slab_desc_t);
		uint32_t index = offset/obj_size;
		uint32_t bit = index % 8;
		used[index/8] = used[index/8] & (~ (0b1 << bit));
		return true;
	}
	else
	{
		uint32_t objs = (slab->size*SMALL_PAGE_SIZE) / obj_size;
		if ((uint32_t) obj < (uint32_t) slab->start)
		{
			return false;
		}
		uint32_t offset = ((uint32_t)obj)-((uint32_t)slab->start);
		if (offset/obj_size > objs)
		{
			return false;
		}
		uint8_t *used = slab->used;
		if (used == NULL)
		{
			DEBUGPRINTLN_1("obj size: %d",obj_size);
			panic("slab allocator free: used array in slab = NULL!\n");
		}
		uint32_t index = offset/obj_size;
		uint32_t bit = index % 8;
		used[index/8] = used[index/8] & (~ (0b1 << bit));
		return true;
	}
}

void print_cacheinfo()
{
	struct cache_t *ccache = caches;
	while (ccache != NULL)
	{
		DEBUGPRINTF_1("cachename:")
		DEBUGPRINTF_1(ccache->name)
		DEBUGPRINTF_1("\n")
		DEBUGPRINTLN_1("cache size: %d",ccache->obj_size)
		DEBUGPRINTLN_1("full slabs:")
		struct slab_desc_t *slab = ccache->full;
		while (slab != NULL)
		{
			DEBUGPRINTLN_1("  address: 0x%x",slab)
			DEBUGPRINTLN_1("  data: 0x%x",slab->start)
			DEBUGPRINTLN_1("  used: 0x%x",slab->used)
			slab = slab->next;
		}
		DEBUGPRINTLN_1("partial slabs:")
		slab = ccache->partial;
		while (slab != NULL)
		{
			DEBUGPRINTLN_1("  address: 0x%x",slab)
			DEBUGPRINTLN_1("  data: 0x%x",slab->start)
			DEBUGPRINTLN_1("  used: 0x%x",slab->used)
			slab = slab->next;
		}
		DEBUGPRINTLN_1("free slabs:")
		slab = ccache->free;
		while (slab != NULL)
		{
			DEBUGPRINTLN_1("  address: 0x%x",slab)
			DEBUGPRINTLN_1("  data: 0x%x",slab->start)
			DEBUGPRINTLN_1("  used: 0x%x",slab->used)
			slab = slab->next;
		}
		ccache = ccache->next;
	}
}


void* alloc_object_from_slab(struct slab_desc_t* slab,uint32_t obj_size)
{
	if (obj_size < 128)
	{
		// slab descriptor on-slab
		uint32_t objs_raw = (slab->size*SMALL_PAGE_SIZE) / obj_size;
		// don't subtract slab descriptor size to account for more objects if calculations are wrong
		
		//uint32_t objs = (slab->size*SMALL_PAGE_SIZE-(objs_raw/8+1))/obj_size;
		void *objects_start = slab->start + sizeof(struct slab_desc_t) + objs_raw/8+1;
		//DEBUGPRINTLN_1("objects_start: 0x%x",objects_start)
		objects_start = align4Bytes(objects_start);
		//DEBUGPRINTLN_1("objects_start: 0x%x",objects_start)
		uint32_t objs = (uint32_t) (slab->start+slab->size*SMALL_PAGE_SIZE-objects_start)/obj_size;
		
		
		
		
		
		
		// offset = sizeof(slab descriptor) + length of used array
		uint8_t *used = slab->start+sizeof(struct slab_desc_t);
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
		if (used == NULL)
		{
			panic("slab allocator alloc: used array in slab = NULL!\n");
		}
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
bool isSlabFree(struct slab_desc_t* slab,uint32_t obj_size)
{
	if (obj_size < 128)
	{
		uint32_t objs_raw = (slab->size*SMALL_PAGE_SIZE) / obj_size;
		void *objects_start = slab->start + sizeof(struct slab_desc_t) + objs_raw/8+1;
		objects_start = align4Bytes(objects_start);
		uint32_t objs = (uint32_t) (slab->start+slab->size*SMALL_PAGE_SIZE-objects_start)/obj_size;
		
		uint8_t *used = slab->start+sizeof(struct slab_desc_t);
		
		for (uint32_t i = 0;i<(objs/obj_size)+1;i++)
		{
			if (used[i] != 0)
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		uint32_t objs = (slab->size*SMALL_PAGE_SIZE) / obj_size;
		uint8_t *used = slab->used;
		if (used == NULL)
		{
			panic("slab allocator isfree: used array in slab = NULL!\n");
		}
		
		for (uint32_t i = 0;i<(objs/obj_size)+1;i++)
		{
			if (used[i] != 0)
			{
				return false;
			}
		}
		return true;
	}
}

// allocates an object from a cache, using a larger one if no perfectly fitting one is found and returning NULL if
// no cache with enough object size was found 
void* kmalloc(uint32_t size)
{
	DEBUGPRINTLN_1("kmalloc: size: %d",size)
	if (size > 1024)
	{
		DEBUGPRINTLN_1("kmalloc: size too big for size cache!")
		/*
		LinkedList* size_entry = requestLinkedListEntry();
		size_entry->next = big_objects_size;
		size_entry->data = (void*) ((size/SMALL_PAGE_SIZE)+1);
		
		LinkedList* entry = requestLinkedListEntry();
		entry->next = big_objects;
		entry->data = useConsecutivePages((uint32_t) size_entry->data,0);
		if (entry->data == NULL)
		{
			freeLinkedListEntry(entry);
			freeLinkedListEntry(size_entry);
			return NULL;
		}
		big_objects_size = size_entry;
		big_objects = entry;
		return (void*) ((entry->data-old_RAM)+remapped_RAM);
		*/
		return NULL;
	}
	struct cache_t *best_fit_cache = NULL;
	struct cache_t *ccache = caches;
	while (ccache != NULL)
	{
		DEBUGPRINTLN_1("kmalloc: current cache size: %d",ccache->obj_size)
		if ((ccache->flags & 0b1) != 0b1) // no_kmalloc flag not set
		{
			DEBUGPRINTLN_1("kmalloc: current cache can be used for malloc")
			if (ccache->obj_size == size)
			{
				DEBUGPRINTLN_1("kmalloc: current cache is best fit")
				best_fit_cache = ccache;
				break;
			}
			if (ccache->obj_size > size)
			{
				if (best_fit_cache == NULL || ccache->obj_size < best_fit_cache->obj_size)
					best_fit_cache = ccache;
			}
		}
		ccache = ccache->next;
	}
	if (best_fit_cache == NULL)
	{
		DEBUGPRINTLN_1("kmalloc: no cache found!")
		return NULL;
	}
	else
	{
		DEBUGPRINTLN_1("kmalloc: cache size: %d!",best_fit_cache->obj_size)
		return alloc_object_from_cache(best_fit_cache);
	}
}

void kfree(void* obj)
{
	DEBUGPRINTLN_1("kfree: freeing object at: 0x%x!",obj)
	/*
	uint32_t index = 0;
	LinkedList* entry = searchLinkedListEntry(&big_objects,(void*) ((obj-remapped_RAM)+old_RAM),&index);
	if (entry != NULL)
	{
		LinkedList* size_entry = getLinkedListEntry(&big_objects_size,index);
		if (size_entry == NULL)
		{
			DEBUGPRINTLN_1("no size entry for bigobject by kmalloc!")
			panic("no size entry for bigobject by kmalloc!");
		}
		else
		{
			freeConsecutivePages(entry->data,(uint32_t) size_entry->data);
			return;
		}
	}
	*/
	
	struct cache_t *ccache = caches;
	while (ccache != NULL)
	{
		DEBUGPRINTLN_1("kfree: current cache size: %d",ccache->obj_size)
		 // no_kmalloc flag not set, because objects from that can't be allocated anyways
		if ((ccache->flags & 0b1) != 0b1)
		{
			if (free_object_from_cache(ccache,obj)) // if it returns true, the object with the specified address was freed
			{
				DEBUGPRINTLN_1("kfree: object freed!")
				return;
			}
		}
		ccache = ccache->next;
	}
}


void kfree_hint(void* obj,uint32_t size)
{
	/*
	if (size > 1024)
	{
		uint32_t index = 0;
		LinkedList* entry = searchLinkedListEntry(&big_objects,(void*) ((obj-remapped_RAM)+old_RAM),&index);
		if (entry != NULL)
		{
			LinkedList* size_entry = getLinkedListEntry(&big_objects_size,index);
			if (size_entry == NULL)
			{
				DEBUGPRINTLN_1("no size entry for bigobject by kmalloc!")
				panic("no size entry for bigobject by kmalloc!");
			}
			else
			{
				freeConsecutivePages(entry->data,(uint32_t) size_entry->data);
				return;
			}
		}
	}
	*/
	struct cache_t *ccache = caches;
	while (ccache != NULL)
	{
		if (ccache->obj_size >= size)
		{
			if (free_object_from_cache(ccache,obj))
			{
				return;
			}
		}
		ccache = ccache->next;
	}
}


struct cache_t* createCache(uint32_t obj_size,uint16_t alignment,uint16_t flags, char* name)
{
	struct cache_t *cache = alloc_object_from_cache(cache_cache);
	cache->full = NULL;
	cache->partial = NULL;
	cache->free = NULL;
	cache->obj_size = obj_size;
	cache->alignment = alignment;
	cache->flags = flags;
	cache->name = name;
	
	
	// add it to the list
	cache->next = caches;
	caches = cache;
	return cache;
}

void initSlabAllocator()
{
	void* page = usePage();
	if (page == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		return;
	}
	k_memset(page,0,SMALL_PAGE_SIZE);
	
	uint32_t *tmp_pds_unaligned = ti_malloc(256*4+1024*2);
	if (tmp_pds_unaligned == NULL)
	{
		DEBUGPRINTF_1("no memory for tmp_pds!\n");
		return;
	}
	uint32_t *tmp_pds = make1KAligned(tmp_pds_unaligned);
	k_memset(tmp_pds,0,1024);
	
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	uint32_t *tt = (uint32_t*) tt_base;
	
	tt[(uint32_t)kernel_heap_start >> 20] = newCPTD(0,(uint32_t) tmp_pds);
	
	tmp_pds[0] = newSPD(1,1,0b01010101,(uint32_t) page);
	
	
	page = usePage();
	if (page == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[1] = newSPD(1,1,0b01010101,(uint32_t) page);
	k_memset(page,0,SMALL_PAGE_SIZE);
	
	page = usePage();
	if (page == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[2] = newSPD(1,1,0b01010101,(uint32_t) page);
	k_memset(page,0,SMALL_PAGE_SIZE);
	page = usePage();
	if (page == NULL)
	{
		DEBUGPRINTF_1("no page available for the slab allocator!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	tmp_pds[3] = newSPD(1,1,0b01010101,(uint32_t) page);
	k_memset(page,0,SMALL_PAGE_SIZE);
	//asm(".long 0xE1212374"); // bkpt
	invalidate_TLB();
	//asm(".long 0xE1212374"); // bkpt
	
	
	struct slab_desc_t *cache_slab = kernel_heap_start;
	kernel_heap_next_page += SMALL_PAGE_SIZE;
	
	cache_slab->size = 1;
	cache_slab->start = cache_slab;
	cache_slab->next = NULL;
	cache_slab->used = NULL;
	//asm(".long 0xE1212374"); // bkpt
	
	cache_cache = alloc_object_from_slab(cache_slab,sizeof(struct cache_t));
	if (cache_cache == NULL)
	{
		DEBUGPRINTF_1("no free entry in created cache!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	//asm(".long 0xE1212374"); // bkpt
	cache_cache->full = NULL;
	//asm(".long 0xE1212374"); // bkpt
	cache_cache->partial = NULL;
	cache_cache->free = cache_slab;
	cache_cache->obj_size = sizeof(struct cache_t);
	cache_cache->alignment = 0;
	cache_cache->flags = 0;
	cache_cache->name = "cache_cache";
	cache_cache->next = NULL;
	
	
	struct slab_desc_t *slab_slab = kernel_heap_next_page;
	kernel_heap_next_page += SMALL_PAGE_SIZE;
	
	slab_slab->size = 1;
	slab_slab->start = slab_slab;
	slab_slab->next = NULL;
	slab_slab->used = NULL;
	
	slab_cache = alloc_object_from_slab(cache_slab,sizeof(struct cache_t));
	if (slab_cache == NULL)
	{
		DEBUGPRINTF_1("no free entry in created cache!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	
	slab_cache->full = NULL;
	slab_cache->partial = NULL;
	slab_cache->free = slab_slab;
	slab_cache->obj_size = sizeof(struct slab_desc_t);
	slab_cache->alignment = 0;
	slab_cache->flags = 0;
	slab_cache->name = "slab_cache";
	slab_cache->next = NULL;
	
	
	struct slab_desc_t *linkedlist_slab = kernel_heap_next_page;
	kernel_heap_next_page += SMALL_PAGE_SIZE;
	
	linkedlist_slab->size = 1;
	linkedlist_slab->start = linkedlist_slab;
	linkedlist_slab->next = NULL;
	linkedlist_slab->used = NULL;
	
	linkedlist_cache = alloc_object_from_slab(cache_slab,sizeof(struct cache_t));
	if (linkedlist_cache == NULL)
	{
		DEBUGPRINTF_1("no free entry in created cache!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	
	linkedlist_cache->full = NULL;
	linkedlist_cache->partial = NULL;
	linkedlist_cache->free = linkedlist_slab;
	linkedlist_cache->obj_size = sizeof(LinkedList);
	linkedlist_cache->alignment = 0;
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
	
	
	
	
	
	cpt_cache = alloc_object_from_slab(cache_slab,sizeof(struct cache_t));
	if (cpt_cache == NULL)
	{
		DEBUGPRINTF_1("no free entry in created cache!\n");
		ti_free(tmp_pds_unaligned);
		return;
	}
	
	cpt_cache->full = NULL;
	cpt_cache->partial = NULL;
	cpt_cache->free = cpt_slab;
	cpt_cache->obj_size = 1024;
	cpt_cache->alignment = 0;
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
	
	
	tt_cache = createCache(1024*16,1024*16,0,"translation_table_cache");
	address_space_cache = createCache(sizeof(struct address_space),0,0,"address_space_cache");
	process_cache = createCache(sizeof(struct Process),0,0,"process_cache");
	thread_cache = createCache(sizeof(struct Thread),0,0,"thread_cache");
	framebuffer_cache = createCache(SMALL_PAGE_SIZE*38,0,0b1,"framebuffer_cache"); // 150 KiB
	
	
	createCache(1,0,0,"1_byte_cache");
	createCache(2,0,0,"2_byte_cache");
	createCache(4,0,0,"4_byte_cache");
	// linkedlist_cache is already 8 bytes
	//createCache(8,0,0,"8_byte_cache");
	createCache(16,0,0,"16_byte_cache");
	createCache(32,0,0,"32_byte_cache");
	createCache(64,0,0,"64_byte_cache");
	createCache(128,0,0,"128_byte_cache");
	createCache(256,0,0,"256_byte_cache");
	createCache(512,0,0,"512_byte_cache");
	// 1024 is already made for cpts
	// anything bigger should just allocate pages directly
	
	
	
	//asm(".long 0xE1212374"); // bkpt
}



bool slab_allocator_self_test_pre_initialization()
{
	void *page = usePage();
	if (page == NULL)
	{
		DEBUGPRINTLN_1("no page for self-test available!")
		return false;
	}
	k_memset(page,0,SMALL_PAGE_SIZE);
	
	struct slab_desc_t *slab = page;
	slab->size = 1;
	slab->start = page;
	slab->next = NULL;
	slab->used = NULL;
	
	
	
	void *obj = alloc_object_from_slab(slab,8);
	void *objs_start = make4ByteAligned((page+sizeof(struct slab_desc_t)+(SMALL_PAGE_SIZE/8)/8)+1);
	if (obj != objs_start)
	{
		DEBUGPRINTLN_1("object not allocated where it should be! page: 0x%x  obj: 0x%x  expected: 0x%x\n",page,obj,objs_start)
		return false;
	}
	uint32_t objs = (uint32_t) (page+SMALL_PAGE_SIZE-objs_start)/8-1; // -1 for the object we already allocated
	void *obj_orig = obj;
	
	for (uint32_t i = 0;i<objs;i++)
	{
		void *obj = alloc_object_from_slab(slab,8);
		if (obj == NULL)
		{
			DEBUGPRINTLN_1("could not allocate enough objects from slab!\n")
			return false;
		}
	}
	
	obj = alloc_object_from_slab(slab,8);
	if (obj != NULL)
	{
		DEBUGPRINTLN_1("can allocate too many objects from slab!\n")
		return false;
	}
	
	free_object_from_slab(slab,8,obj_orig);
	if (alloc_object_from_slab(slab,8) != obj_orig)
	{
		DEBUGPRINTLN_1("deallocating and reallocating an object doesn't result in the same address!\n")
		return false;
	}
	
	
	
	
	
	
	
	
	
	
	
	return true;
}

bool slab_allocator_self_test_post_initialization()
{
	// TODO write the self test
	
	
	
	
	
	
	
	
	
	return true;
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
	
	uint32_t text = milis;
	
	
	
	
	
}

void ensureCPTCapacity()
{
	
}
void ensureLinkedListCapacity()
{
	
}






LinkedList* requestLinkedListEntry()
{
	return alloc_object_from_cache(linkedlist_cache);
}
void freeLinkedListEntry(void* list)
{
	free_object_from_cache(linkedlist_cache,list);
}

uint32_t* requestCPT()
{
	return alloc_object_from_cache(cpt_cache);
}
void freeCPT(void* cpt)
{
	free_object_from_cache(cpt_cache,cpt);
}


struct address_space* requestAddressSpace()
{
	return alloc_object_from_cache(address_space_cache);
}
void freeAddressSpace(void *space)
{
	free_object_from_cache(address_space_cache,space);
}


uint32_t* requestTranslationTable()
{
	return alloc_object_from_cache(tt_cache);
}
void freeTranslationTable(uint32_t *tt)
{
	free_object_from_cache(tt_cache,tt);
}


void* requestLCDFramebuffer()
{
	return alloc_object_from_cache(framebuffer_cache);
}
void freeLCDFramebuffer(void* buff)
{
	free_object_from_cache(framebuffer_cache,buff);
}


struct Process* requestProcess()
{
	return alloc_object_from_cache(process_cache);
}
void freeProcess(void* proc)
{
	free_object_from_cache(process_cache,proc);
}


struct Thread* requestThread()
{
	return alloc_object_from_cache(thread_cache);
}
void freeThread(void* thread)
{
	free_object_from_cache(thread_cache,thread);
}


uint32_t* request4Bytes()
{
	return kmalloc(4);
}
void free4Bytes(uint32_t* b)
{
	kfree_hint(b,4);
}












































































