#include "kernel.h"


const uint32_t SMALL_PAGE_SIZE = 1024*4;
const uint32_t LARGE_PAGE_SIZE = 1024*64;
const uint32_t SECTION_SIZE = 1024*1024;


struct pageblock {
	void* unaligned;
	void* start; // first page
	uint32_t size; // number of pages in block
	uint64_t used; // one bit per page
	uint64_t used2;
	uint64_t dirty; // one bit per page
	uint64_t dirty2;
	uint64_t pageout;
	uint64_t pageout2;
};








#define MAXPAGEBLOCKS 200
uint32_t blockindex = 0;
struct pageblock pages[MAXPAGEBLOCKS];



void* makeLargePageAligned(void *ptr)
{
	if ((((uint32_t) ptr) & (~ 0xFFFF)) == (uint32_t) ptr)
		return ptr;
	return (void*) ((((uint32_t) ptr) & (~ 0xFFFF))+0x10000);
}

void* makeSmallPageAligned(void *ptr)
{
	if ((((uint32_t) ptr) & (~ 0xFFF)) == (uint32_t) ptr)
		return ptr;
	return (void*) ((((uint32_t) ptr) & (~ 0xFFF))+0x1000);
}


static bool addPageBlock(struct pageblock block)
{
	if (blockindex >= MAXPAGEBLOCKS-4)
	{
		return false;
	}
	pages[blockindex] = block;
	blockindex++;
	return true;
}

static void removePageblock(struct pageblock block)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		if (pages[i].unaligned == block.unaligned)
		{
			for (uint32_t a = i;a<blockindex-1;a++)
			{
				pages[a] = pages[a+1];
			}
			blockindex--;
			return;
		}
	}
}




// allocate new pages for use in the kernel
// use ti_malloc, so the ti-os doesn't use those pages
bool allocPageblock(uint32_t size)
{
	if (size > 128)
	{
		return false;
	}
	struct pageblock b;
	b.unaligned = ti_malloc(SMALL_PAGE_SIZE*(size+3));
	if (b.unaligned == NULL)
	{
		return false;
	}
	b.size = size;
	b.start = makeSmallPageAligned(b.unaligned);
	b.used = 0b0;
	b.dirty = 0b0;
	b.pageout = 0b0;
	b.used2 = 0b0;
	b.dirty2 = 0b0;
	b.pageout2 = 0b0;
	if (! addPageBlock(b))
	{
		ti_free(b.unaligned);
		return false;
	}
	DEBUGPRINTF_2("pageblock added, size: 0x%x\n",b.size)
	return true;
}


void* useConsecutivePages(uint32_t size,uint32_t alignment)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		void *page = pages[i].start;
		for (uint32_t a = 0;a<pages[i].size;a++)
		{
			if (((uint32_t) page % alignment) == 0)
			{
				//DEBUGPRINTLN_1("aligned page found: 0x%x",page)
				bool free = true;
				for (uint32_t b = 0;b<size;b++)
				{
					if (isPageUsed(page+SMALL_PAGE_SIZE*b))
					{
						free = false;
						break;
					}
				}
				if (free)
				{
					for (uint32_t b = 0;b<size;b++)
					{
						//DEBUGPRINTLN_1("setting page 0x%x used",page+SMALL_PAGE_SIZE*b)
						setPageUsedBit(page+SMALL_PAGE_SIZE*b,true);
						//DEBUGPRINTLN_1("used: %d",isPageUsed(page+SMALL_PAGE_SIZE*b))
					}
					return page;
				}
			}
			page = page + SMALL_PAGE_SIZE;
		}
	}
	return NULL;
}







// pageblock is not visible in the other files, and freeing pageblocks should only
// be done in this memory manager
static void freePageblock(struct pageblock block)
{
	removePageblock(block);
	ti_free(block.unaligned);
}



void freeAllPageblocks()
{
	while (blockindex != 0)
	{
		freePageblock(pages[0]);
	}
	
	
}

static void setBit128(uint64_t *a,uint64_t *b,uint32_t i,bool value)
{
	uint64_t *p;
	if (i < 64)
	{
		p = a;
	}
	else
	{
		p = b;
		i -= 64;
	}
	if (value)
	{
		*p = *p | ((uint64_t) 0b1 << (uint64_t) i);
	}
	else
	{
		*p = *p & (~ ((uint64_t) 0b1 << (uint64_t) i));
	}
}
static uint32_t getBit128(uint64_t *a,uint64_t *b,uint32_t i)
{
	uint64_t *p;
	if (i < 64)
	{
		p = a;
	}
	else
	{
		p = b;
		i -= 64;
	}
	return (uint32_t) (((*p) >> i) & 0b1);
}



// get a pointer to a unused page and set it to used
void* usePage()
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock *b = pages+i;
		uint32_t ones = signedtounsigned32(__builtin_popcountll(b->used) + __builtin_popcountll(b->used2));
		if (ones < b->size) // a page in the block is free
		{
			for (uint32_t a = 0;a<b->size;a++)
			{
				if (! getBit128(&b->used,&b->used2,a))
				{
					setBit128(&b->used,&b->used2,a,true);
					return b->start+SMALL_PAGE_SIZE*a;
				}
			}
		}
	}
	return NULL;
}

void setPageUsedBit(void* page,bool used)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock *b = pages+i;
		if (page >= b->start && page <= b->start+SMALL_PAGE_SIZE*b->size)
		{
			uint32_t offset = ((uint32_t) page - (uint32_t) b->start)/SMALL_PAGE_SIZE;
			DEBUGPRINTLN_1("page offset: %d",offset)
			setBit128(&b->used,&b->used2,offset,used);
		}
	}
}


// returns whether a page is used, or true if not found (so you don't use a unallocated page)
bool isPageUsed(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock *b = pages+i;
		if (page >= b->start && page <= b->start+SMALL_PAGE_SIZE*b->size)
		{
			uint32_t offset = ((uint32_t) page - (uint32_t) b->start)/SMALL_PAGE_SIZE;
			if (getBit128(&b->used,&b->used2,offset) == 1)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}


bool isPageDirty(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock *b = pages+i;
		if (page >= b->start && page <= b->start+SMALL_PAGE_SIZE*b->size)
		{
			uint32_t offset = ((uint32_t) page - (uint32_t) b->start)/SMALL_PAGE_SIZE;
			if (getBit128(&b->dirty,&b->dirty2,offset) == 1)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}



void setPageDirtyBit(void *page,bool dirty)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock *b = pages+i;
		if (page >= b->start && page <= b->start+SMALL_PAGE_SIZE*b->size)
		{
			uint32_t offset = ((uint32_t) page - (uint32_t) b->start)/SMALL_PAGE_SIZE;
			setBit128(&b->dirty,&b->dirty2,offset,dirty);
		}
	}
}

bool isPagedOut(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock *b = pages+i;
		if (page >= b->start && page <= b->start+SMALL_PAGE_SIZE*b->size)
		{
			uint32_t offset = ((uint32_t) page - (uint32_t) b->start)/SMALL_PAGE_SIZE;
			if (getBit128(&b->pageout,&b->pageout2,offset) == 1)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}


void setPagedOutBit(void *page,bool pagedout)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock *b = pages+i;
		if (page >= b->start && page <= b->start+SMALL_PAGE_SIZE*b->size)
		{
			uint32_t offset = ((uint32_t) page - (uint32_t) b->start)/SMALL_PAGE_SIZE;
			setBit128(&b->pageout,&b->pageout2,offset,pagedout);
		}
	}
}




bool physical_mm_self_test()
{
	DEBUGPRINTLN_1("starting physical memory manager self test\n")
	
	// test pageblock allocation and deallocation
	allocPageblock(10);
	if (pages[0].size != 10)
	{
		DEBUGPRINTLN_1("size doesn't match up with pageblock allocation!")
		return false;
	}
	if (pages[0].used != 0)
	{
		DEBUGPRINTLN_1("used is not 0!")
		return false;
	}
	if (pages[0].pageout != 0)
	{
		DEBUGPRINTLN_1("pageout is not 0!")
		return false;
	}
	if (pages[0].dirty != 0)
	{
		DEBUGPRINTLN_1("dirty is not 0!")
		return false;
	}
	uint64_t a1 = 0, a2 = 0;
	setBit128(&a1,&a2,0,true);
	if (a1 != 1)
	{
		DEBUGPRINTLN_1("setBit128 0 not working!")
		return false;
	}
	if (getBit128(&a1,&a2,0) != 1)
	{
		DEBUGPRINTLN_1("getBit128 0 not working!")
		return false;
	}
	a1 = 0;
	a2 = 0;
	setBit128(&a1,&a2,64,true);
	if (a2 != 1)
	{
		DEBUGPRINTLN_1("setBit128 64 not working!")
		return false;
	}
	if (getBit128(&a1,&a2,64) != 1)
	{
		DEBUGPRINTLN_1("getBit128 64 not working!")
		return false;
	}
	a1 = 0;
	a2 = 0;
	setBit128(&a1,&a2,3,true);
	if (a1 != (1 << 3))
	{
		DEBUGPRINTLN_1("setBit128 3 not working! expected: 0x%llx, got: 0x%llx",(uint64_t) 1 << 3,a1)
		return false;
	}
	if (getBit128(&a1,&a2,3) != 1)
	{
		DEBUGPRINTLN_1("getBit128 3 not working!")
		return false;
	}
	void *page = usePage();
	if (! isPageUsed(page))
	{
		DEBUGPRINTLN_1("isPageUsed not working!")
		return false;
	}
	setPageDirtyBit(page,true);
	if (! isPageDirty(page))
	{
		DEBUGPRINTLN_1("isPageDirty or setDirty not working! expected: 1, got: 0x%llx, 0x%llx",pages[0].dirty,pages[0].dirty2)
		return false;
	}
	setPagedOutBit(page,true);
	if (! isPagedOut(page))
	{
		DEBUGPRINTLN_1("isPagedOut or setPagedOut not working!")
		return false;
	}
	for (uint32_t i = 0;i<15;i++)
	{
		void *p = usePage();
		if (i >= 9 && p != NULL) // one page already used for the tests
		{
			DEBUGPRINTLN_1("returning too many pages from a pageblock!")
			return false;
		}
	}
	
	
	
	removePageblock(pages[0]);
	if (blockindex != 0)
	{
		DEBUGPRINTLN_1("blockindex is not 0 after removing! blockindex: %d",blockindex)
		return false;
	}
	
	allocPageblock(10);
	
	void * cons_pages = useConsecutivePages(10,0);
	if (cons_pages == NULL)
	{
		DEBUGPRINTLN_1("useConsecutivePages not working!")
		return false;
	}
	
	for (uint32_t i = 0;i<10;i++)
	{
		if (! isPageUsed(cons_pages+SMALL_PAGE_SIZE*i))
		{
			DEBUGPRINTLN_1("useConsecutivePages not setting all pages to used! i: %d",i)
			return false;
		}
	}
	removePageblock(pages[0]);
	
	DEBUGPRINTLN_1("\nfinished physical memory manager self test\n\n\n")
	
	
	
	return true;
}



















