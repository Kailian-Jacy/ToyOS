#include "syscall/mysyscall.h"

extern void __switch_to(struct task_struct* prev, struct task_struct* next);
extern void __dummy();
uint64_t program(){
	return (uint64_t) uapp_start;
}

uint64 sys_getc()
{
	char c;
	getck(&c);
	return c;
}

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

	// Set in new process.
	uint64_t temp = regs->a0;
	regs->a0 = 0;
	// Copy parent page table.
	copy_pgdir(parent, new);
	// Recover for parent process.
	regs->a0 = temp;

	// Jump tp BR2;
	printk("[S] Fork performed. Producing new task %d from parent %d.\n", new->pid, parent->pid);

	return new->pid; // 6
}

uint64 sys_exec(struct pt_regs * regs){
	// exec checkes loaded ELF.
	uint64_t start = regs->a0;
	Elf64_Ehdr * header = (Elf64_Ehdr *) start;
	unsigned char header_magic[EI_NIDENT] = { 0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	for (int j = 0; j< EI_NIDENT; j++){
		if (header_magic[j] != header->e_ident[j]) {
			printk("[S] Exec target is not valid ELF.\n");
			// Panic this process.
			while (1);
		}
	}

	// Unset old page table and vmas.
	for (struct vm_area_struct * vma = current_task()->mm.mmap; vma != NULL; vma = vma->vm_next){
		for (uint64_t pg_start = PGROUNDDOWN(vma->vm_start); pg_start < vma->vm_end; pg_start += PGSIZE){
			int check = 0;
			check_created(current_task()->pgd + PA2VA_OFFSET, pg_start, &check);
			// Not dealloc user stack.
			if (check != 0 && pg_start != USER_END - PGSIZE){
				unset_created(current_task()->pgd + PA2VA_OFFSET, pg_start);
			}
		}
	}
	flush();
	unset_vma(current_task()->mm.mmap);

	// Truly load elf.
	uint64_t phdr_start = (uint64_t)header + header->e_phoff;
	int phdr_cnt = header->e_phnum;

	current_task()->mm.file=start;
	Elf64_Phdr *phdr;
	int load_phdr_cnt = 0;
	for (int j = 0; j < phdr_cnt; j++)
	{
		phdr = (Elf64_Phdr *)(phdr_start + sizeof(Elf64_Phdr) * j);
		if (phdr->p_type == PT_LOAD)
		{
			// Alloc vma for each phdr
			set_vma(current_task(), phdr->p_vaddr, phdr->p_memsz, phdr->p_flags, 
					phdr->p_offset, phdr->p_filesz);
		}
	}

	// Set register. 
	current_task()->thread.sepc = USER_START + header->e_entry;
	current_task()->thread.sstatus = ~SPP | SPIE | SUM;
	current_task()->thread.sscratch = KERNEL_MODE;
	current_task()->thread_info->user_sp = USER_END;
	// Reset sp.
	current_task()->thread.sp = USER_END;
	current_task()->thread.ra = (uint64)__dummy;

	// Reset kernel sp as a new processes.
	current_task()->thread_info->kernel_sp = (uint64)current_task();

	printk("[S] Exec performed. Task %d runs new elf from %lx.\n", current_task()->pid, current_task()->thread.sepc);
	__switch_to(get_task(0), current_task());
	return current_task()->thread.sepc;
}