#include "../kernel.h"




asm(
".global irq_wrapper \n"
"irq_wrapper: \n"
//".long 0xE1212374 \n" // breakpoint
"ldr pc, orig_irq_address \n"
" \n"
" \n"
" \n"
" \n"
"orig_irq_address: .long 0x18 \n"
" \n");


























