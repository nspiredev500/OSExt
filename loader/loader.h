#ifndef LOADER_H
#define LOADER_H


const uint32_t SMALL_PAGE_SIZE = 1024*4;
uint32_t newCPTD(unsigned char domain,uint32_t base_address);
uint32_t newLPD(unsigned char c,unsigned char b,unsigned char ap,uint32_t base_address);
uint32_t newSPD(unsigned char c,unsigned char b,unsigned char ap,uint32_t base_address);
uint32_t newSD(unsigned char c,unsigned char b,unsigned char domain,unsigned char ap,uint32_t base_address);
void* makeSmallPageAligned(void *ptr);


#endif