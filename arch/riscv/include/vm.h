#ifndef __VM_H__
#define __VM_H__

#include "proc.h"
#include "stddef.h"
#include <defs.h>
#include <string.h>

void setup_vm(void);
void setup_vm_final(void);

void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm, int in_virtual, int rewrite, int *check);
void check_created(uint64 *pgtbl, uint64 va, int *check);
void unset_created(uint64 *pgtbl, uint64 va);

struct vm_area_struct * find_vma(struct task_struct *task, uint64_t addr);
void set_vma(struct task_struct *task, uint64_t addr, uint64_t length, uint64_t flags,
			 uint64_t vm_content_offset_in_file, uint64_t vm_content_size_in_file);

extern void _set_satp_with_swapper(uint64 addr);
unsigned long get_swapper_pg_dir();

void flush();

#endif