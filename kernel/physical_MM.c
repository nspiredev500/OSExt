#include "kernel.h"


const uint32_t SMALL_PAGE_SIZE = 1024*4;
const uint32_t LARGE_PAGE_SIZE = 1024*64;
const uint32_t SECTION_SIZE = 1024*1024;


struct pageblock {
	void* unaligned;
	void* start; // first page
	uint16_t size; // number of pages in block
	uint64_t used; // one bit per page, so no more than 64 pages per block
	uint64_t dirty; // one bit per page
	uint64_t pageout;
};








#define MAXPAGEBLOCKS 200
uint32_t blockindex = 0;
struct pageblock pages[MAXPAGEBLOCKS];



void* makeLargePageAligned(void *ptr)
{
	if ((((uint32_t) ptr) & (~ 0xFFFF)) == ptr)
		return ptr;
	return (void*) ((((uint32_t) ptr) & (~ 0xFFFF))+0x10000);
}

void* makeSmallPageAligned(void *ptr)
{
	if ((((uint32_t) ptr) & (~ 0xFFF)) == ptr)
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
	// TODO malloc pages, plus alignment
	
	if (size > 64)
	{
		return false;
	}
	
	/*
	// TODO first malloc some space and check the pointer, then malloc the chunk before a page boundrary, then malloc the pages and free the chunk before them
	void *tmp = ti_malloc(8);
	if (tmp == NULL)
	{
		return false;
	}
	// void *aligned = (void*) ((malloced_chunk & (~ 0xFFFF))+0x10000);
	uint32_t padding = (((uint32_t) tmp)-((((uint32_t) tmp) & (~ 0xffff))+0x10000))-0xff;
	ti_free(tmp);
	*/
	
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
	if (! addPageBlock(b))
	{
		ti_free(b.unaligned);
	}
	return true;
}

void* DMAPageblock(uint32_t size)
{
	void* unaligned = ti_malloc(SMALL_PAGE_SIZE*(size+2));
	if (unaligned == NULL)
	{
		return NULL;
	}
	return makeSmallPageAligned(unaligned);
}


// pageblock is not visible in the other files, and freeing pageblocks should only
// be done in this memory manager
static void freePageblock(struct pageblock block)
{
	removePageblock(block);
	ti_free(block.unaligned);
}





// get a pointer to a unused page and set it to used
void* usePage()
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[blockindex];
		if (b.used < k_pow_of_2(b.size))
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (((b.used >> i) & 0b1) == 0)
				{
					void* page = b.start+i*SMALL_PAGE_SIZE;
					return page;
				}
			}
			
			
		}
	}
	return NULL;
}




void setPageUnused(void* page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[blockindex];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					b.used = b.used & (~ (0b1 << i));
					return;
				}
			}
		}
	}
}

bool isDirty(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[blockindex];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					if ((b.dirty & (0b1 << i)) != 0)
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

void clearDirty(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[blockindex];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					b.dirty = b.dirty & (~ (0b1 << i));
					return;
				}
			}
		}
	}
}

void setDirty(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[blockindex];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					b.dirty = b.dirty | (0b1 << i);
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
		struct pageblock b = pages[blockindex];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					if ((b.pageout & (0b1 << i)) != 0)
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

void clearPageout(void *page)
{
	for (uint32_t i = 0;i<blockindex;i++)
	{
		struct pageblock b = pages[blockindex];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					b.pageout = b.pageout & (~ (0b1 << i));
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
		struct pageblock b = pages[blockindex];
		if (page >= b.start && page <= b.start+SMALL_PAGE_SIZE*b.size)
		{
			for (uint32_t i = 0;i<b.size;i++)
			{
				if (page == b.start+SMALL_PAGE_SIZE*i)
				{
					b.pageout = b.pageout | (0b1 << i);
					return;
				}
			}
		}
	}
}

























