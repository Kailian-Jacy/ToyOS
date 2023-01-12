#ifndef _DEFS_H
#define _DEFS_H

/*
    Memory related macros.
*/

#define PHY_END   (PHY_START + PHY_SIZE)
#define PGSIZE 0x1000 // 4KB
#define PGROUNDUP(addr) ((addr + PGSIZE - 1) & (~(PGSIZE - 1)))
#define PGROUNDDOWN(addr) (addr & (~(PGSIZE - 1)))  // Swipe less 12 bits to 0.

#define USER_START (0x0000000000000000)             // user space start virtual address
#define USER_END (0x0000004000000000)               // user space end virtual address

#define SPP (0x01UL << 0x8) // SPP = 0, sret to u-mode.
#define SPIE (0x01UL << 0x05) // SPIE enabled and sret, then interrupt enabled.
#define SUM (0x01UL << 18) // SUM 

/*
    File system related macros.
*/
#define DISK_START ( USER_END )
#define DISK_END ( DISC_SRART + DISC_SIZE )
#define DISK_SIZE ( 0x100000 ) // 1MB
#define BLOCK_SIZE ( PGSIZE )  // 4KB

/*
    Symbols defined for UAPP loading and executing.
*/
#define LOAD_START (0x00000000000000e8)
#define LOAD_END

#include "types.h"
#define csr_read(csr)                       \
({                                          \
    register uint64 __v;                    \
    asm volatile ("csrr %0,"#csr            \
        :"=r" (__v):                        \
        :"memory");                         \
    __v;                                    \
}) 

#define csr_write(csr, val)                         \
({                                                  \
    uint64 __v = (uint64)(val);                     \
    asm volatile ("csrw " #csr ", %0"               \
                    : : "r" (__v)                   \
                    : "memory");                    \
})

extern char uapp_start[];
extern char uapp_end[];
extern char fork_start[];
extern char fork_end[];


#include <ndef.h>

#endif
