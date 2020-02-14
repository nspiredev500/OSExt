#ifndef PHYSICAL_MM_H
#define PHYSICAL_MM_H



void* makeLargePageAligned(void *ptr);
void* makeSmallPageAligned(void *ptr);


void freeAllPageblocks();

bool allocPageblock(uint32_t size);
void* usePage();
bool isPageUsed(void *page);
void setPageUsedBit(void* page, bool used);

bool isPageDirty(void *page);
void setPageDirtyBit(void *page, bool dirty);

bool isPagedOut(void *page);
void setPagedOutBit(void *page,bool pagedout);


void* useConsecutivePages(uint32_t size,uint32_t alignment);









bool physical_mm_self_test();


#endif