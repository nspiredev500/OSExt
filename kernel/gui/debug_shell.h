#ifndef DEBUG_SHELL
#define DEBUG_SHELL

void debug_shell_println_rgb(char* string,uint32_t r, uint32_t g,uint32_t b,...);
void debug_shell_println(char* string,...);

void debug_shell_reset();
void setShellFramebuffer(void *framebuffer);

void shell_println_rgb(char* string,uint32_t r, uint32_t g,uint32_t b,...);
void shell_println(char* string,...);



#endif