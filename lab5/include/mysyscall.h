#ifndef __MYSYSCALL_H
#define __MYSYSCALL_H
#include "proc.h"
#include "printk.h"

#define SYS_WRITE 64
#define SYS_GETPID 172

#define SYS_MUNMAP 215
#define SYS_CLONE 220 // fork
#define SYS_MMAP 222
#define SYS_MPROTECT 226

uint64 sys_write(unsigned int fd, const char *buf, size_t count);
uint64 sys_getpid();
uint64 sys_fork();

#endif