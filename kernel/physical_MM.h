#ifndef PHYSICAL_MM_H
#define PHYSICAL_MM_H



void* makeLargePageAligned(void *ptr);
void* makeSmallPageAligned(void *ptr);


bool allocPageblock(uint32_t size);
void* usePage();
void setPageUnused(void* page);

bool isDirty(void *page);
void clearDirty(void *page);
void setDirty(void *page);

bool isPagedOut(void *page);
void clearPageout(void *page);
void setPagedOut(void *page);







































#endif