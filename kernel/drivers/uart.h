#ifndef UART_H
#define UART_H


void remappUART(void *new_mapping);

void uart_printf(const char *str,...);
void uart_send_uint32(uint32_t a);
void uart_send_string(const char *string);
void uart_send_string_length(const char *string,int length);
void uart_send(char c);
char uart_rec();
void uart_send_uint32_base(uint32_t a,uint32_t base);
void uart_send_uint64_base(uint64_t a,uint32_t base);
void uart_println(const char *str,...);
void uart_println_va(const char *str,va_list list);
void uart_printf_va(const char *str,va_list list);



























#endif