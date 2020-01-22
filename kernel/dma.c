#include "kernel.h"

const uint32_t SMALL_PAGE_SIZE = 1024*4;

struct dmablock {
	void *base;
	uint32_t pages;
};
#define DMABLOCKSIZE 20

static uint32_t dmablockindex = 0;
static struct dmablock[DMABLOCKSIZE]; // normal array, so I don't have to use the heap for DMA memory management


struct malloc_header {
	uint32_t size;
	uint32_t alignment;
	bool end;
	bool used;
};

bool reserveDMAMemory(uint32_t pages)
{
	void *pageblock = DMAPageblock(pages);
	if (pageblock == NULL)
	{
		return false;
	}
	bool t = addDMABlock(pageblock,pages);
	panic("not enough dma blocks reserved!\n");
	return true;
}

static bool addDMABlock(void* base, uint32_t pages)
{
	if (dmablockindex <= DMABLOCKSIZE-1)
	{
		return false;
	}
	dmablock[dmablockindex].base = base;
	dmablock[dmablockindex].pages = pages;
	dmablockindex++;
	struct malloc_header *h = base;
	h->size = pages*SMALL_PAGE_SIZE;
	h->alignment = 0;
	h->used = false;
	h->end = true;
	return true;
}



void *mallocDMA(uint32_t size,uint32_t alignment)
{
	for (int i = 0;i<DMABLOCKSIZE-1;i++)
	{
		struct malloc_header *h = dmablock[i].base;
		while (true)
		{
			if (h-> used == false)
			{
				void* data_start = h;
				data_start += sizeof(malloc_header);
				if ()
			}
			if (h->end)
			{
				break;
			}
			void* next = h;
			next += sizeof(malloc_header) + h->size;
			h = next;
		}
	}
	return NULL;
}
































