#ifndef VIRTUAL_MM_H
#define VIRTUAL_MM_H

// coarse page table
struct cpt;
struct cpt {
	uint32_t *table; // the actual table has to be in dma memory
	struct cpt *next;
};

struct address_space {
	uint32_t *tt;
	struct cpt *page_tables; // for mapped pages,except kernel pages, because these page tables can be reused
};


uint32_t newCPTD(unsigned char domain,uint32_t base_address);
uint32_t newLPD(unsigned char c,unsigned char b,unsigned char ap,uint32_t base_address);
uint32_t newSPD(unsigned char c,unsigned char b,unsigned char ap,uint32_t base_address);

void invalidate_TLB();
void clear_caches();








#endif