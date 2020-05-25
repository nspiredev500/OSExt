#ifndef KERNEL_H
#define KERNEL_H

#include "../config.h"



#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
// from the ndless-sdk include directory
#include <syscall-list.h>




#include "libk/memory.h"
#include "libk/mutex.h"
#include "libk/modes.h"


#include "scheduling/deferred_action.h"
#include "scheduling/process.h"
#include "scheduling/thread.h"
#include "scheduling/scheduler.h"

#include "ti-os.h"
#include "LinkedList.h"
#include "charset/chars.h"
#include "charset/digits.h"
#include "charset/ascii.h"
#include "osversion.h"
#include "libk/math.h"
#include "libk/string.h"
#include "libk/wait.h"


#include "elf/elf.h"
#include "modules/modules.h"

#include "gui/img565.h"
#include "gui/nspire_menus.h"
#include "gui/time-dialog.h"
#include "gui/bmp.h"
#include "gui/background.h"
#include "gui/debug_shell.h"



#include "drivers/power.h"
#include "drivers/irq_controller.h"
#include "drivers/i2c.h"
#include "drivers/touchpad.h"
#include "drivers/uart.h"
#include "drivers/lcd.h"
#include "drivers/rtc.h"
#include "drivers/watchdog.h"
#include "drivers/keypad.h"
#include "drivers/timer.h"
#include "drivers/usb.h"
#include "drivers/adc.h"
#include "drivers/nand.h"
#include "drivers/systime.h"
#include "drivers/vfs.h"
#include "drivers/init_drivers.h"


#include "exceptions/syscalls.h"
#include "exceptions/data_abort.h"
#include "exceptions/prefetch_abort.h"
#include "exceptions/undefined_instruction.h"
#include "exceptions/fiq.h"
#include "exceptions/irq.h"
#include "exceptions/aborts.h"


#include "syscalls/svc/uninstall.h"

#include "memory/physical_MM.h"
#include "memory/virtual_MM.h"
#include "memory/slab.h"



#include "panic.h"
#include "reloc.h"
#include "start.h"
#ifndef RELEASE
	#include "self_test.h"
#endif
#include "os-hooks.h"
#include "init-kernel.h"





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
#define DEBUG_BREAKPOINTS 0

#ifdef RELEASE
	#undef DEBUGLEVEL
	#undef DEBUG_BREAKPOINTS
	#define DEBUGLEVEL 0
	#define DEBUG_BREAKPOINTS 0
#endif



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




#if DEBUG_BREAKPOINTS == 0
	#define BREAKPOINT_ASM 
	#define BREAKPOINT
#else
	#define BREAKPOINT_ASM ".long 0xE1212374\n"
	#define BREAKPOINT asm volatile(".long 0xE1212374\n");
#endif









#endif