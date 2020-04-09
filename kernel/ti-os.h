#ifndef TI_OS
#define TI_OS


void ascii2utf16(char *string16,char *string, size_t size);
void* ti_malloc(uint32_t size); // not 8-byte-aligned !
void ti_free(void *ptr);
void* ti_calloc(uint32_t size); // not 8-byte-aligned !
int TCT_Local_Control_Interrupts(int mask);
//int32_t show_1_numeric_input(char* title,char* subtitle,char* msg,int32_t *val);

typedef uint32_t NUC_FILE;

NUC_FILE* nuc_fopen(char *path,char *mode);
void nuc_fclose(NUC_FILE* f);
uint32_t nuc_fread(void* buffer,uint32_t elements,uint32_t size,NUC_FILE* f);
uint32_t nuc_fwrite(void* buffer,uint32_t elements,uint32_t size,NUC_FILE* f);
int32_t nuc_fseek(NUC_FILE* f,int32_t offset, int32_t whence);
int32_t nuc_fflush(NUC_FILE* f);


#endif