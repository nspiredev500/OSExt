.section .text
.global _start
.extern main
.extern exit
_start:
	
	// use mmap to allocate the start of the stack
	mov r0, #0b111
	lsl r0, r0, #29 // leftshift to get 0xe0000000
	mov r1, #0b1
	lsl r1, r1, #13 // leftshift to get 0x2000
	sub r0, r0, r1 // calculate the second last user-space page
	mov r1, #0b1
	lsl r1, r1, #12 // leftshift to get 0x1000, length = one page
	mov r2, #0b11 // read/write
	mov r3, #0b1101 // MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS
	mov r4, #0
	mov r5, #0
	swi #1
	mov r1, #0xffffffff // MAP_FAILED
	cmp r0, r1
	bne success
	swi #0 // exit if failed
	success:
	mov r0, sp
	add sp, sp, r1
	sub sp, sp, #8
	mov r0, #0
	mov r1, #0
	mov r2, #0
	mov r3, #0
	mov r4, #0
	mov r5, #0
	
	// TODO arguments
	bl main
	mov r0, #0
	b exit
.size _start, . - _start
