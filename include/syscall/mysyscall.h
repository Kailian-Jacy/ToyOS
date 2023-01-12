#ifndef __MYSYSCALL_H
#define __MYSYSCALL_H
#include "proc.h"
#include "io/io.h"
#include "defs.h"
#include <elf.h>

#define SYS_WRITE 64
#define SYS_GETC 65 // Self-defined.
#define SYS_GETPID 172

#define SYS_MUNMAP 215
#define SYS_CLONE 220 // fork
#define SYS_EXEC 221
#define SYS_MMAP 222
#define SYS_MPROTECT 226

uint64 sys_getpid();
uint64 sys_getc();
uint64 sys_write(unsigned int fd, const char *buf, size_t count);
uint64 sys_fork(struct pt_regs * regs);
// Sys_exec call loads regs->a0 as start of elf runnable file.
uint64 sys_exec(struct pt_regs * regs);

extern char __return_from_fork[];
#endif