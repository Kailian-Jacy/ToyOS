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
				//  "li a6, %2\n"
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

// int global_variable = 0;
int global_variable = 1;

int main() {
	// int pid = fork();

	// if (pid == 0){
	// 	// Child.
	// 	exec();
	// 	while (1){
	// 		printf("[PID = %d] If I appears, this is a child process. variable: %d\n", getpid(), global_variable++);
	// 		for (unsigned int i = 0; i < 0x7FFFFFF; i++);
	// 	}
	// } else {
		// father.
	while(1) {
		// Printf does not work now.
		printf("[PID = %d] newly execed uapp is running, variable: %d\n", getpid(), global_variable++);
		for (unsigned int i = 0; i < 0x7FFFFFF; i++);
	}
	// }
}

// int main() {
//     int pid;

//     pid = fork();

//     if (pid == 0) {
//         while (1) {
//             printf("[U-CHILD] pid: %ld is running!, global_variable: %d\n", getpid(), global_variable++);
//             for (unsigned int i = 0; i < 0x7FFFFFF; i++);
//         }
//     } else {
//         while (1) {
//             printf("[U-PARENT] pid: %ld is running!, global_variable: %d\n", getpid(), global_variable++);
//             for (unsigned int i = 0; i < 0x7FFFFFF; i++);
//         }
//     }
//     return 0;
// }

// int main()
// {
// 	int pid;

// 	for (int i = 0; i < 3; i++)
// 		printf("[U] pid: %ld is running!, global_variable: %d\n", getpid(), global_variable++);

// 	pid = fork();

// 	if (pid == 0)
// 	{
// 		while (1)
// 		{
// 			printf("[U-CHILD] pid: %ld is running!, global_variable: %d\n", getpid(), global_variable++);
// 			for (unsigned int i = 0; i < 0x7FFFFFF; i++)
// 				;
// 		}
// 	}
// 	else
// 	{
// 		while (1)
// 		{
// 			printf("[U-PARENT] pid: %ld is running!, global_variable: %d\n", getpid(), global_variable++);
// 			for (unsigned int i = 0; i < 0x7FFFFFF; i++)
// 				;
// 		}
// 	}
// 	return 0;
// }

// int main() {

//     printf("[U] pid: %ld is running!, global_variable: %d\n", getpid(), global_variable++);
//     fork();

//     printf("[U] pid: %ld is running!, global_variable: %d\n", getpid(), global_variable++);
//     fork();

//     while(1) {
//         printf("[U] pid: %ld is running!, global_variable: %d\n", getpid(), global_variable++);
//         for (unsigned int i = 0; i < 0x7FFFFFF; i++);
//     }
// }