#include "types.h"
#include "printk.h"

#define TRAP_MASK (1UL << 63)
#define STI 0x05

void handler_interrupt(uint64 scause, uint64 sepc, uint64 regs);
void handler_exception(uint64 scause, uint64 sepc, uint64 regs);
void trap_handler(uint64 scause, uint64 sepc, uint64 regs);