#include "io/io.h"
#include "sbi.h"

#ifdef SJF
#define SCHEDULE "SJF"
#elif PRIORITY
#define SCHEDULE "PRIORITY"
#else 
#define SCHEDULE "UNDEFINED"
#endif

int start_kernel() {


    printk("\n =============RISC-V===========\n");
    printk(" Scheduler: %s\n", SCHEDULE);

    schedule();
    while (1);

	return 0;
}
