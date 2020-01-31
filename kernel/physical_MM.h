#ifndef PHYSICAL_MM_H
#define PHYSICAL_MM_H



void* makeLargePageAligned(void *ptr);
void* makeSmallPageAligned(void *ptr);


bool allocPageblock(uint32_t size);
void* usePage();
bool isPageUsed(void *page);
void setPageUnused(void* page);
void setPageUsed(void* page);

bool isPageDirty(void *page);
void clearPageDirty(void *page);
void setPageDirty(void *page);

bool isPagedOut(void *page);
void clearPagedOut(void *page);
void setPagedOut(void *page);


void* useConsecutivePages(uint32_t size,uint32_t alignment);









bool physical_mm_self_test();


#endif