#include "defs.h"
#include "string.h"
#include "mm.h"
#include "buddy.h"

#include "printk.h"

extern char _ekernel[];
#ifndef BUDDY

uint64 kalloc() {
    struct run *r;

    r = kmem.freelist;
    kmem.freelist = r->next;
    
    memset((void *)r, 0x0, PGSIZE);
    return (uint64) r;
}

void kfree(uint64 addr) {
    struct run *r;

    // PGSIZE align 
    addr = addr & ~(PGSIZE - 1);

    memset((void *)addr, 0x0, (uint64)PGSIZE);

    r = (struct run *)addr;
    r->next = kmem.freelist;
    kmem.freelist = r;

    return ;
}

void kfreerange(char *start, char *end) {
    char *addr = (char *)PGROUNDUP((uint64)start);
    for (; (uint64)(addr) + PGSIZE <= (uint64)end; addr += PGSIZE) {
        kfree((uint64)addr);
    }
}

void mm_init(void) {
    // Modified: Absolute address modified to virtual address.
    // From System.map we know that _ekernel has been set to virtual memory.
    kfreerange(_ekernel, (char *)(PHY_END + PA2VA_OFFSET));
    printk("...mm_init done!\n");
}

#else 

#include "buddy.h"

void mm_init(void)
{
	// kfreerange(_ekernel, (char *)0xffffffe008000000);
	// printk("_ekernel: %lx\n", (uint64_t)_ekernel);
	// printk("...mm_init done!\n");
	buddy_init();
}

uint64_t kalloc()
{
	// struct run *r;
	// r = kmem.freelist;
	// // printk("r is %lx\n", (uint64_t)r);
	// kmem.freelist = r->next;

	// memset((void *)r, 0x0, PGSIZE);
	// return (uint64_t) r;
	return alloc_page();
}

void kfree(uint64_t addr)
{
	// struct run *r;

	// // PGSIZE align
	// addr = addr & ~(PGSIZE - 1);

	// memset((void *)addr, 0x0, (uint64_t)PGSIZE);

	// r = (struct run *)addr;
	// r->next = kmem.freelist;
	// kmem.freelist = r;

	// return ;
	free_pages(addr);
}

void kfreerange(char *start, char *end)
{
	// char *addr = (char *)PGROUNDUP((uint64_t)start);
	// for (; (uint64_t)(addr) + PGSIZE <= (uint64_t)end; addr += PGSIZE) {
	//     kfree((uint64_t)addr);
	// }
	return;
}


#endif