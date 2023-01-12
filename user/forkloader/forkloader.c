#include "syscall/mysyscall.h"
#include "io/mystdio.h"

static inline long getpid()
{
	long ret;
	asm volatile("li a7, %1\n"
				 "ecall\n"
				 "mv %0, a0\n"
				 : "+r"(ret)
				 : "i"(SYS_GETPID));
	return ret;
}

static inline long exec()
{
	long ret;
	asm volatile("li a7, %1\n"
				 "ecall\n"
				 "mv %0, a0\n"
				 : "+r"(ret)
				 : "i"(SYS_EXEC));
	return ret;
}

static inline long fork()
{
	long ret;
	asm volatile("li a7, %1\n"
				 "ecall\n"
				 "mv %0, a0\n"
				 : "+r"(ret)
				 : "i"(SYS_CLONE));
	return ret;
}

int globl_variable = 1;

int main_forkloader() {
	int pid = fork();

	if (pid == 0){
		// Child.
		exec();
		while (1){
			printf("[PID = %d] If I appears, this is a child process, and exec fails. variable: %d\n", getpid(), globl_variable++);
			for (unsigned int i = 0; i < 0x7FFFFFF; i++);
		}
	} else {
		// father.
		while(1) {
			// Printf does not work now.
			printf("[PID = %d] original forker is running, variable: %d\n", getpid(), globl_variable++);
			for (unsigned int i = 0; i < 0x7FFFFFF; i++);
		}
	}
}