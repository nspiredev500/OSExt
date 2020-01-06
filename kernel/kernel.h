#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <syscall-list.h>

#include "process.h"
#include "thread.h"

#include "scheduler.h"
#include "watchdog.h"
#include "fiq.h"
#include "virtual_MM.h"
#include "physical_MM.h"
#include "syscalls.h"
#include "lcd.h"
#include "rtc.h"
#include "aborts.h"
#include "panic.h"
#include "uart.h"
#include "memory.h"
#include "reloc.h"
#include "start.h"
#include "string.h"



extern uint32_t *_EXEC_START;
extern uint32_t *_EXEC_END;


#endif