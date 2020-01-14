#ifndef VIRTUAL_MM_H
#define VIRTUAL_MM_H





uint32_t newCPTD(unsigned char domain,uint32_t base_address);

uint32_t newLPD(unsigned char c,unsigned char b,unsigned char ap,uint32_t base_address);



void invalidate_TLB();
void clear_caches();










#endif