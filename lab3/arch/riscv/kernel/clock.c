#include "clock.h"
#include "sbi.h"

// QEMU中时钟的频率是10MHz, 也就是1秒钟相当于10000000个时钟周期。
unsigned long TIMECLOCK = 10000000;
// unsigned long TIMEINTERVAL = TIMECLOCK/10;

unsigned long get_cycles()
{
	// 使用 rdtime 编写内联汇编，获取 time 寄存器中 (也就是mtime 寄存器 )的值并返回
	unsigned long n;
	asm volatile("rdtime %0"
				 : "=r"(n));
	return n;
}

void clock_set_next_event()
{
	// 下一次 时钟中断 的时间点
	unsigned long next = get_cycles() + TIMECLOCK;

	// 使用 sbi_ecall 来完成对下一次时钟中断的设置
	sbi_ecall(SBI_SETTIMER, 0, next, 0, 0, 0, 0, 0);
}
