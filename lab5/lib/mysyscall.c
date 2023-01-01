#include "mysyscall.h"

uint64 sys_write(unsigned int fd, const char *buf, size_t count)
{
	if(fd == 1){
		((char*)buf)[count] = '\0';
		return printk(buf);
	}
	else{
		printk("[S] Unknown sys_write fd: %d\n", fd);
	}
	return 0;
}

uint64 sys_getpid(){
	return (uint64)current_task()->pid;
}

uint64 sys_fork(struct pt_regs * regs){
	/*
	1. 参考 task_init 创建一个新的 task, 将的 parent task 的整个页复制到新创建的
	   task_struct 页上(这一步复制了哪些东西?）。将 thread.ra 设置为
	   __ret_from_fork, 并正确设置 thread.sp
	   (仔细想想，这个应该设置成什么值?可以根据 child task 的返回路径来倒推)

	2. 利用参数 regs 来计算出 child task 的对应的 pt_regs 的地址，
	   并将其中的 a0, sp, sepc 设置成正确的值(为什么还要设置 sp?)

	3. 为 child task 申请 user stack, 并将 parent task 的 user stack
	   数据复制到其中。

	3.1. 同时将子 task 的 user stack 的地址保存在 thread_info->
	   user_sp 中，如果你已经去掉了 thread_info，那么无需执行这一步

	4. 为 child task 分配一个根页表，并仿照 setup_vm_final 来创建内核空间的映射

	5. 根据 parent task 的页表和 vma 来分配并拷贝 child task 在用户态会用到的内存

	6. 返回子 task 的 pid
   */
	struct task_struct * parent = current_task();

	// Create new task.
	struct task_struct * new = new_task(regs, parent);
	// new->thread_info->user_sp = parent->thread_info->user_sp;

	// Copy parent page table.
	copy_pgdir(parent, new);

	// Jump tp BR2;
	printk("[S] Fork performed. Producing new task %d from parent %d.\n", new->pid, parent->pid);

	return new->pid; // 6
}