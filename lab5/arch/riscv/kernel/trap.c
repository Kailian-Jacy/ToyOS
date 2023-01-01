#include "trap.h"
void handler_interrupt(uint64 scause, uint64 sepc, struct pt_regs* regs)
{
	switch (scause & ~TRAP_MASK)
	{
	case STI:
		clock_set_next_event();
		do_timer();
		break;
	default:
		break;
	}
}

void handler_exception(uint64 scause, uint64 sepc, struct pt_regs *regs)
{
	switch (scause)
	{
	case IPF:
	case SPF:
	case LPF:
		printk("[S] Page fault exception caught. \n scause: %lx, sepc: %lx, ", scause, sepc);
		do_page_fault(regs);
		break;
	case EFU:
		// Ecall from user mode.
		((uint64_t *)regs)[31] += 4;
		switch (regs->a7)
		{
		case SYS_WRITE:
			regs->a0 = sys_write(regs->a0, regs->a1, regs->a2);
			break;
		case SYS_GETPID:
			regs->a0 = sys_getpid();
			break;
		case SYS_CLONE:
			regs->a0 = sys_fork(regs);
			break;
		default:
			printk("[S] Unrecognized syscall: %d\n", regs->a0);
		}
		break;
	default:
		printk("[trap] Unhandled exception: %d\n", scause);
		break;
	}
}

void trap_handler(uint64 scause, uint64 sepc, struct pt_regs *regs)
{
	if (scause & TRAP_MASK) 
		handler_interrupt(scause, sepc, regs);
	else{
		handler_exception(scause, sepc, regs);
	}		
}
