#include "kernel.h"


asm(
".global delegate_system_calls \n"
".global swi_wrapper \n"
"delegate_system_calls: .long 1 \n"
"swi_address: .long 0x8 \n"
"tmp_sp: .long 0 \n"
"swi_wrapper: \n"
//".long 0xE1212374 \n" // breakpoint
"ldr pc, swi_address \n"


"push {r0} \n"
"mrs r0, cpsr \n"
"push {r0} \n"
"ldr r0, delegate_system_calls \n"
"cmp r0, #0 \n"
"bne delegate \n"
".long 0xE1212374 \n" // breakpoint
" \n"
" \n"
" \n"
" \n"
" \n"
"delegate: \n"
"pop {r0} \n"
"msr cpsr, r0 \n"
"pop {r0} \n"
"ldr pc, swi_address \n"
" \n"
" \n"
" \n");







































