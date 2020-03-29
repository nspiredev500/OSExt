#ifndef TI_OS
#define TI_OS


void ascii2utf16(char *string16,char *string, size_t size);
void* ti_malloc(uint32_t size); // not 8-byte-aligned !
void ti_free(void *ptr);
void* ti_calloc(uint32_t size); // not 8-byte-aligned !
int TCT_Local_Control_Interrupts(int mask);
int32_t show_1_numeric_input(char* title,char* subtitle,char* msg,int32_t *val);



#endif