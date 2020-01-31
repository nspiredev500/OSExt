#ifndef PHYSICAL_MM_H
#define PHYSICAL_MM_H



void* makeLargePageAligned(void *ptr);
void* makeSmallPageAligned(void *ptr);


bool allocPageblock(uint32_t size);
void* usePage();
bool isPageUsed(void *page);
void setPageUnused(void* page);
void setPageUsed(void* page);

bool isDirty(void *page);
void clearDirty(void *page);
void setDirty(void *page);

bool isPagedOut(void *page);
void clearPageout(void *page);
void setPagedOut(void *page);


void* useConsecutivePages(uint32_t size,uint32_t alignment);









void physical_mm_self_test();


#endif