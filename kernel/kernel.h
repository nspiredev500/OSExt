#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
// from the ndless-sdk include directory
#include <syscall-list.h>


#include "mutex.h"
#include "process.h"
#include "thread.h"
#include "LinkedList.h"
#include "chars.h"
#include "digits.h"
#include "ascii.h"


#include "scheduler.h"
#include "watchdog.h"
#include "fiq.h"
#include "virtual_MM.h"
#include "physical_MM.h"
#include "syscalls.h"
#include "data_abort.h"
#include "prefetch_abort.h"
#include "undefined_instruction.h"
#include "lcd.h"
#include "rtc.h"
#include "aborts.h"
#include "panic.h"
#include "uart.h"
#include "memory.h"
#include "reloc.h"
#include "start.h"
#include "string.h"
#include "math.h"
#include "slab.h"
#include "self_test.h"
#include "keypad.h"
#include "debug_shell.h"
#include "modes.h"




extern char _EXEC_START;
// _EXEC_END doesn't take the GOT into account
//extern char _EXEC_END;
extern char _EXEC_SIZE;


extern char _TEXT_START;
extern char _DATA_START;
extern char _BSS_START;
extern char _GOT_START;

extern char _TEXT_SIZE;
extern char _DATA_SIZE;
extern char _BSS_SIZE;
extern char _GOT_SIZE;



void* const virtual_base_address;
void* const kernel_heap_start;
volatile void** LCD_UPBASE;


const uint32_t SMALL_PAGE_SIZE;
const uint32_t LARGE_PAGE_SIZE;
const uint32_t SECTION_SIZE;




#define DEBUGLEVEL 3






#if DEBUGLEVEL == 0
	#define DEBUGPRINTF_1(...) 
	#define DEBUGPRINTF_2(...) 
	#define DEBUGPRINTF_3(...) 
#endif
#if DEBUGLEVEL == 1
	#define DEBUGPRINTF_1(...) uart_printf(__VA_ARGS__);
	#define DEBUGPRINTF_2(...) 
	#define DEBUGPRINTF_3(...) 
#endif
#if DEBUGLEVEL == 2
	#define DEBUGPRINTF_1(...) uart_printf(__VA_ARGS__);
	#define DEBUGPRINTF_2(...) uart_printf(__VA_ARGS__);
	#define DEBUGPRINTF_3(...) 
#endif
#if DEBUGLEVEL == 3
	#define DEBUGPRINTF_1(...) uart_printf(__VA_ARGS__);
	#define DEBUGPRINTF_2(...) uart_printf(__VA_ARGS__);
	#define DEBUGPRINTF_3(...) uart_printf(__VA_ARGS__);
#endif



#if DEBUGLEVEL == 0
	#define DEBUGPRINTLN_1(...) 
	#define DEBUGPRINTLN_2(...) 
	#define DEBUGPRINTLN_3(...) 
#endif
#if DEBUGLEVEL == 1
	#define DEBUGPRINTLN_1(...) uart_println(__VA_ARGS__);
	#define DEBUGPRINTLN_2(...) 
	#define DEBUGPRINTLN_3(...) 
#endif
#if DEBUGLEVEL == 2
	#define DEBUGPRINTLN_1(...) uart_println(__VA_ARGS__);
	#define DEBUGPRINTLN_2(...) uart_println(__VA_ARGS__);
	#define DEBUGPRINTLN_3(...) 
#endif
#if DEBUGLEVEL == 3
	#define DEBUGPRINTLN_1(...) uart_println(__VA_ARGS__);
	#define DEBUGPRINTLN_2(...) uart_println(__VA_ARGS__);
	#define DEBUGPRINTLN_3(...) uart_println(__VA_ARGS__);
#endif













#endif