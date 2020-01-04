#ifndef UART_H
#define UART_H






void uart_send_string(char *string);
void uart_send_string_length(char *string,int length);
void uart_send(char c);
char uart_rec();































#endif