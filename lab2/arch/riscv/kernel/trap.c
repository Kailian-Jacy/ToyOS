#include "trap.h"
#include "clock.h"

void handler_interrupt(uint64 scause, uint64 sepc, uint64 regs)
{
	switch (scause & ~TRAP_MASK)
	{
	case STI:
		printk("kernel is running! \n");
		printk("[S] Supervisor Mode Timer Interrupt\n");
		clock_set_next_event();
		break;
	default:
		break;
	}
}

void handler_exception(uint64 scause, uint64 sepc, uint64 regs)
{
}

void trap_handler(uint64 scause, uint64 sepc, uint64 regs)
{
	if (scause & TRAP_MASK) 
		handler_interrupt(scause, sepc, regs);
	else
		handler_exception(scause, sepc, regs);
}
