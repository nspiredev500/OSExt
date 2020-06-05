#ifndef STRING_H
#define STRING_H


// maximal length to prevent endless scanning for non-terminated strings
uint32_t k_strlen(const char *string,uint32_t max);

int32_t k_strcmp(const char *s1, const char *s2, uint32_t max);
bool k_streq(const char *s1, const char *s2, uint32_t max); // k_strcmp thought systime_timestamp_to_unix and systime_unix_to_timestamp were the same, and we can't have that happen

void sprintf_safe(char *result,char *str,uint32_t length,...);
uint32_t sprint_uint64_base_safe(char* string,uint32_t length,uint64_t a,uint32_t base);
uint32_t sprint_uint32_base_safe(char* string,uint32_t length,uint32_t a,uint32_t base);
uint32_t sprint_double_safe(char* string,uint32_t length,double a,uint32_t roundto);
void sprintf_safe_va(char *result,char *str,uint32_t length,va_list va);

































#endif