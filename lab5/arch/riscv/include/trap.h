#ifndef __TRAP_H
#define __TRAP_H

#include "types.h"
#include "printk.h"
#include "clock.h"
#include "proc.h"
#include "mysyscall.h"

#define TRAP_MASK (1UL << 63)
#define STI 0x05
/*
08  Ecall from User mode.
12	Instruction Page Fault.
13	Load Page Fault.
15	Store/AMO Page Fault.
All handled by page fault handler here.
*/
#define IPF 0x0c 
#define LPF 0x0d
#define SPF 0x0f
#define EFU 0x08

void handler_interrupt(uint64 scause, uint64 sepc, struct pt_regs * regs);
void handler_exception(uint64 scause, uint64 sepc, struct pt_regs * regs);
void trap_handler(uint64 scause, uint64 sepc, struct pt_regs * regs);

#endif