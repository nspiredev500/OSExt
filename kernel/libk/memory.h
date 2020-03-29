#ifndef MEMORY_H
#define MEMORY_H


void *k_memset(void *str,int c, size_t n);
void *k_memcpy(void *str1,const void *str2, size_t n);
int k_memcmp(const void *str1, const void *str2, size_t n);


void* align4Bytes(void* address);







#endif