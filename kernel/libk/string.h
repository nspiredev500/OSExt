#ifndef STRING_H
#define STRING_H


// maximal length to prevent endless scanning for non-terminated strings
uint32_t k_strlen(const char *string,uint32_t max);

void sprintf_safe(char *result,char *str,uint32_t length,...);
uint32_t sprint_uint64_base_safe(char* string,uint32_t length,uint64_t a,uint32_t base);
uint32_t sprint_uint32_base_safe(char* string,uint32_t length,uint32_t a,uint32_t base);
uint32_t sprint_double_safe(char* string,uint32_t length,double a,uint32_t roundto);
void sprintf_safe_va(char *result,char *str,uint32_t length,va_list va);

































#endif