#include "printk.h"
#include "sbi.h"

extern void test();

int start_kernel() {

    printk(" ==RISC-V== Booted.\n");
    test(); // DO NOT DELETE !!!

	return 0;
}
