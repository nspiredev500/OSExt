#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H 1


#include <sys/types.h>



#define PROT_READ 0b1
#define PROT_WRITE 0b10
#define PROT_EXEC 0b100
#define PROT_NONE 0b0

#define MAP_SHARED 0b10
#define MAP_PRIVATE 0b1
#define MAP_FIXED 0b100
#define MAP_ANONYMOUS 0b1000
#define MAP_ANON MAP_ANONYMOUS

#define MS_ASYNC 0b1
#define MS_SYNC 0b10
#define MS_INVALIDATE 0b100


#define MCL_CURRENT 0b1
#define MCL_FUTURE 0b10


#define MAP_FAILED  0xffffffff





#endif