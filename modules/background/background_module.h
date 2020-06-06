#ifndef BACKGROUND_MODULE_H
#define BACKGROUND_MODULE_H




void (*msleep)(uint32_t);


bool (*isKeyPressed)();
void* (*get_back_framebuffer_address)();
void (*blitLCDBuffer)();

void* (*useConsecutivePages)(uint32_t,uint32_t);
void (*freeConsecutivePages)(void*,uint32_t);

uint32_t (*rtc_get_value)();


NUC_FILE* (*nuc_fopen)(const char *path,char *mode);
int32_t (*nuc_fclose)(NUC_FILE* f);
uint32_t (*nuc_fread)(void* buffer,uint32_t elements,uint32_t size,NUC_FILE* f);


void (*framebuffer_fillrect)(void*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
void (*framebuffer_drawrect)(void*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
void (*framebuffer_write10pdigit)(void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b,int charn);
void (*framebuffer_setpixel)(void *buff,uint32_t x,uint32_t y,uint32_t r, uint32_t g,uint32_t b);
void (*framebuffer_write10pstring_ascii)(char* string,void *buff,int xx,int yy,uint32_t r, uint32_t g,uint32_t b);

void (*k_memset)(void *str,int c, size_t n);
void (*sprintf_safe)(char *result,char *str,uint32_t length,...);
uint32_t (*k_strlen)(const char *string,uint32_t max);


bool (*register_draw_function)(void*);
bool (*unregister_draw_function)(void*);
bool (*register_file_function)(void*);
bool (*unregister_file_function)(void*);
void (*debug_shell_println_rgb)(const char*,uint32_t,uint32_t,uint32_t,...);
void* (*search)(char*);
void (*panic)(const char*);
void (*uart_printf)(const char*,...);

void* (*kmalloc)(uint32_t);
void (*kfree)(void*);

void (*framebuffer_setpixel565)(void *buff,uint32_t x,uint32_t y,uint16_t rgb565);
uint16_t (*rgbto565)(uint32_t,uint32_t,uint32_t);

void* (*search)(char*);


void (*k_memcpy)(void*,void*,size_t);
uint32_t (*signedtounsigned32)(int32_t i);





#endif