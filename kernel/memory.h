#ifndef MEMORY_H
#define MEMORY_H


void *ti_malloc(uint32_t size); // not 8-byte-aligned !
void ti_free(void *ptr);

void *k_memset(void *str,int c, size_t n);
void *k_memcpy(void *str1,const void *str2, size_t n);
int k_memcmp(const void *str1, const void *str2, size_t n);










#endif