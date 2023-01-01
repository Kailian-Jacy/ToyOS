#include "printk.h"
#include "sbi.h"
#include "user_main.h"

#ifdef SJF
#define SCHEDULE "SJF"
#elif PRIORITY
#define SCHEDULE "PRIORITY"
#else 
#define SCHEDULE "UNDEFINED"
#endif

extern void test();

int start_kernel() {

    printk(" =============RISC-V=========== Booted.\n");
    printk(" -----------Scheduler---------- \n");
    printk( SCHEDULE );
    printk(" \n ");

    schedule();
    test(); // DO NOT DELETE !!!

	return 0;
}
