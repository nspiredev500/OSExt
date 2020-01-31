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
		if (pages[blockindex].unaligned == block.unaligned)
		{
			for (uint32_t a = i;a<blockindex-1;a++)
			{
				pages[blockindex] = pages[blockindex+1];
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
	b.unaligned = ti_malloc(SMALL_PAGE_SIZE*(size+2));
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
	uint32_t alignmentbits = 0;
	for (uint32_t i = alignment;(i & 0b1) != 1;i = i>>1)
	{
		alignmentbits++;
	}
	alignmentbits = set_bits32(alignmentbits);
	for (uint32_t i = 0;i<blockindex;i++)
	{
		void *page = pages[blockindex].start;
		for (uint32_t a = 0;a<pages[blockindex].size;a++)
		{
			if (((uint32_t) page & alignmentbits) == 0)
			{
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
						setPageUsed(page+SMALL_PAGE_SIZE*b);
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
		*p = *p | (0b1 << i);
		DEBUGPRINTF_3("newp1: 0x%llx, newp2: 0x%llx\n",*a, *b)
	}
	else
	{
		*p = *p & (~ (0b1 << i));
		DEBUGPRINTF_3("newp1: 0x%llx, newp2: 0x%llx\n",*a, *b)
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
	return ((*p) >> i) & 0b1;
}



// get a pointer to a unused page and set it to used
void* usePage()
{
	//DEBUGPRINTF_3("blockindex: 0x%x\n",blockindex)
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock *b = &pages[i];
		//DEBUGPRINTF_3("used: 0x%x, size: 0x%x\n",(uint32_t) b->used,(uint32_t) b->size)
		for (uint32_t a = 0;a<b->size;a++)
		{
			//DEBUGPRINTF_1("a: 0x%x, used: 0x%x, used2: 0x%x\n",a,(uint32_t) b->used,(uint32_t) b->used2)
			if (getBit128(&b->used,&b->used2,a) == 0)
			{
				setBit128(&b->used,&b->used2,a,true);
				void* page = b->start+a*SMALL_PAGE_SIZE;
				//DEBUGPRINTF_3("page: 0x%x\n",page)
				return page;
			}
		}
	}
	return NULL;
}

void setPageUsed(void* page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[i];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					setBit128(&b.used,&b.used2,i,true);
					return;
				}
			}
		}
	}
}


void setPageUnused(void* page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[i];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					setBit128(&b.used,&b.used2,i,false);
					return;
				}
			}
		}
	}
}
// returns whether a page is used, or true if not found (so you don't use a unallocated page)
bool isPageUsed(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[i];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					if (getBit128(&b.used,&b.used2,i) != 0)
					{
						return true;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}


bool isPageDirty(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[i];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					if (getBit128(&b.dirty,&b.dirty2,i) != 0)
					{
						return true;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	return false;
}

void clearPageDirty(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[i];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					setBit128(&b.dirty,&b.dirty2,i,false);
					return;
				}
			}
		}
	}
}

void setPageDirty(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[i];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					setBit128(&b.dirty,&b.dirty2,i,true);
					return;
				}
			}
		}
	}
}

bool isPagedOut(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[i];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					if (getBit128(&b.pageout,&b.pageout2,i) != 0)
					{
						return true;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	return false;
}

void clearPagedOut(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[i];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					setBit128(&b.pageout,&b.pageout2,i,false);
					return;
				}
			}
		}
	}
}

void setPagedOut(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[i];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					setBit128(&b.pageout,&b.pageout2,i,true);
					return;
				}
			}
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
	void *page = usePage();
	if (! isPageUsed(page))
	{
		DEBUGPRINTLN_1("isPageUsed not working!")
		return false;
	}
	setPageDirty(page);
	if (! isPageDirty(page))
	{
		DEBUGPRINTLN_1("isPageDirty or setDirty not working!")
		return false;
	}
	setPagedOut(page);
	if (! isPagedOut(page))
	{
		DEBUGPRINTLN_1("isPagedOut or setPagedOut not working!")
		return false;
	}
	for (uint32_t i = 0;i<30;i++)
	{
		void *p = usePage();
		if (i == 10 && p != NULL)
		{
			DEBUGPRINTLN_1("returning too many pages from a pageblock!")
			return false;
		}
	}
	
	
	
	removePageblock(pages[0]);
	if (blockindex != 0)
	{
		DEBUGPRINTLN_1("blockindex is not 0 after removing!")
		return false;
	}
	
	
	
	DEBUGPRINTLN_1("\nfinished physical memory manager sel test\n\n\n")
	
	
	
	return true;
}



















