#include "types.h"

struct run {
    struct run *next;
};

void mm_init();

// kalloc memset physical memory and return starting address.
uint64 kalloc();
void kfree(uint64);