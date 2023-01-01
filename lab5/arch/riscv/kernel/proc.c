#include <proc.h>
#include <mm.h>
#include <rand.h>
#include <elf.h>
#include "defs.h"

extern void __dummy();
extern char __return_from_fork[];

struct task_struct* idle;           // idle process
struct task_struct* current;        // 指向当前运行线程的 `task_struct`
struct task_struct* task[CAP_TASKS]; // 线程数组，所有的线程都保存在此
int num_task = NR_TASKS;

struct task_struct* current_task(){
	return current;
}

// Load ELF. Loads uapp only.
void load_uapp(int i){
	// Now ELF is linked into UAPP_START.
	Elf64_Ehdr * header = (Elf64_Ehdr *) uapp_start;
	unsigned char header_magic[EI_NIDENT] = { 0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	// check header.
	for (int j = 0; j< EI_NIDENT; j++){
		if (header_magic[j] != header->e_ident[j]) {
			printk("load_uapp.header.magic\n");
		}
	}
	uint64_t phdr_start = (uint64_t)header + header->e_phoff;
	int phdr_cnt = header->e_phnum;

	Elf64_Phdr *phdr;
	int load_phdr_cnt = 0;
	for (int j = 0; j < phdr_cnt; j++)
	{
		phdr = (Elf64_Phdr *)(phdr_start + sizeof(Elf64_Phdr) * j);
		if (phdr->p_type == PT_LOAD)
		{
			// Alloc vma for each phdr
			do_mmap(task[i], phdr->p_vaddr, phdr->p_memsz, phdr->p_flags, 
					phdr->p_offset, phdr->p_filesz);
		}
	}
	// allocate user stack and do mapping
	do_mmap(task[i],USER_END - PGSIZE, PGSIZE, VM_READ | VM_WRITE, 0, 0);
	uint64_t stack = alloc_page();
	create_mapping(task[i]->pgd + PA2VA_OFFSET, USER_END - PGSIZE, stack - PA2VA_OFFSET, 1, VM_WRITE | VM_READ | VM_USER, 1, 0, 0);

	// following code has been written for you.
	// TODO set user stack
	// pc for the user program
	task[i]->thread.sepc = USER_START + header->e_entry;
	// sstatus bits set
	task[i]->thread.sstatus = ~SPP | SPIE | SUM;
	// user stack for user program
	task[i]->thread.sscratch = KERNEL_MODE;
	task[i]->thread_info->user_sp = USER_END;
	task[i]->thread.sp = USER_END; // 3 and 4
}

// Set kernel 
void set_kernel(int i) {
	task[i] = (struct task_struct *)kalloc();
	task[i]->state  = TASK_RUNNING;   
	task[i]->counter = 0;
	task[i]->priority = rand();  
	task[i]->pid = i;  // 2

	task[i]->thread.ra = (uint64)__dummy;
	task[i]->thread.sp = (uint64)task[i] + PGSIZE;  // 3 and 4
	task[i]->thread_info = (struct thread_info *)kalloc();
	task[i]->thread_info->kernel_sp = (uint64)task[i] + PGSIZE;
}

void task_copy_pgtbl(){
	for (int i = 1; i < NR_TASKS; i++){
		// Alloc for user page table.
		task[i]->pgd = (pagetable_t) alloc_page();
		// Copy kernel page table to this.
		char * origin = get_swapper_pg_dir();
		for (int j = 0; j < PGSIZE; j++) {
			((char *)task[i]->pgd)[j] = origin[j];
		}
		task[i]->pgd -= PA2VA_OFFSET;
		// After copying swapper_pg_dir can we load uapp.
		load_uapp(i);
	}
}

void task_init() {
    // 1. 调用 kalloc() 为 idle 分配一个物理页
    // 2. 设置 state 为 TASK_RUNNING;
    // 3. 由于 idle 不参与调度 可以将其 counter / priority 设置为 0
    // 4. 设置 idle 的 pid 为 0
    // 5. 将 current 和 task[0] 指向 idle

    /* YOUR CODE HERE */
	idle = (struct task_struct *)kalloc(); 	// 1. Point idle to the start address of page.
	idle->state  = TASK_RUNNING;  // 2
	idle->counter = 0;
	idle->priority = 0; 		// 3
	idle->pid = 0; 	// 4
	// idle->thread_info = (struct thread_info *)kalloc();
	task[0] = idle;	// 5
	current = idle;

    // 1. 参考 idle 的设置, 为 task[1] ~ task[NR_TASKS - 1] 进行初始化
    // 2. 其中每个线程的 state 为 TASK_RUNNING, counter 为 0, priority 使用 rand() 来设置, pid 为该线程在线程数组中的下标。
    // 3. 为 task[1] ~ task[NR_TASKS - 1] 设置 `thread_struct` 中的 `ra` 和 `sp`,
    // 4. 其中 `ra` 设置为 __dummy （见 4.3.2）的地址， `sp` 设置为 该线程申请的物理页的高地址

    /* YOUR CODE HERE */
	for (int i = 1; i < NR_TASKS; i++){
		set_kernel(i);
	}
}

void dummy() {
    uint64 MOD = 1000000007;
    uint64 auto_inc_local_var = 0;
    int last_counter = -1;
    while(1) {
        if (last_counter == -1 || current->counter != last_counter) {
            last_counter = current->counter;
            auto_inc_local_var = (auto_inc_local_var + 1) % MOD;
            printk("[PID = %d] is running. auto_inc_local_var = %d\n", current->pid, auto_inc_local_var);
        }
    }
}

extern void __switch_to(struct task_struct* prev, struct task_struct* next);

void switch_to(struct task_struct* next) {
    /* YOUR CODE HERE */
	if (next->pid == current->pid){
		return;
	}
	struct task_struct * prev = current;
	current = next;
	printk("\nSwitched to Thread: %d\n", current->pid);
				// task[i]->counter = 2;
	__switch_to(prev, current);
	return;
}

void do_timer(){
	if (current->pid == 0){
		schedule();
		return;
	}
	current->counter--;
	if (current->counter > 0) {
		return;
	}
	schedule();
	return;
}

#ifdef SJF
void schedule(){
	int next = 1;
	int flag = 0;
	while (!flag){
		for (int i = 1; num_task; i++){
			if (task[i]->state == TASK_RUNNING && task[i]->counter != 0){
				flag = 1;
				next = i;
				break;
			}
		}
		if (!flag) {
			for (int i = 1; i < num_task; i++){
				// task[i]->counter = 2;
				task[i]->counter = rand();
        		printk("ReIniting. [PID = %d] . SET Counter = %d, SET Priority = %d\n", i, task[i]->counter, task[i]->priority);
			}
		}
	}
	for (int i = 2; i < num_task; i++){
		if (task[i]->state == TASK_RUNNING && task[i]->counter != 0 && task[i]->counter < task[next]->counter){
			next = i;
		}
	}
	switch_to(task[next]);
}
#elif PRIORITY
void schedule()
{
	int next = 1;
	int flag = 0;
	while (!flag)
	{
		for (int i = 1; i < num_task; i++)
		{
			if (task[i]->state == TASK_RUNNING && task[i]->counter != 0)
			{
				flag = 1;
				next = i;
				break;
			}
		}
		if (!flag)
		{
			for (int i = 1; i < num_task; i++)
			{
				// Task Counter contains its priority.
				task[i]->counter = rand();
				printk("ReIniting. [PID = %d] . SET Counter = %d, SET Priority = %d\n", i, task[i]->counter, task[i]->priority);
			}
		}
	}
	for (int i = 2; i < num_task; i++)
	{
		// Select the largest.
		if (task[i]->state == TASK_RUNNING && task[i]->counter != 0 && task[i]->priority > task[next]->priority)
		{
			next = i;
		}
	}
	switch_to(task[next]);


}
#endif

void do_page_fault(struct pt_regs *regs){
	struct vm_area_struct * vma = 0;
	// // Should it be sepc here?
	uint64_t stval;
	asm volatile("csrr %0, stval" \
		: "=r" (stval):		\
		: "memory" );
	printk("stval: %lx\n", stval);
	if ((vma = find_vma(current, stval)) == 0){
		printk("page_fault.find_vma");
		// Should halt kernel.
	}
	// Create new entry in process table.
	// Each page fault, we allocate 1 page.
	uint64_t pg_start = PGROUNDDOWN(stval);
	uint64_t flag = vma->flags << 1;

	uint64_t page_pa = alloc_page() - PA2VA_OFFSET;
	create_mapping(current->pgd + PA2VA_OFFSET, pg_start, page_pa, 1, flag, 1, 0, 0);
	flush();

	uint64_t file_start;
	if (pg_start >= vma->vm_start){
		// Start in the middle of vm_start.
		file_start = (uint64_t)uapp_start + (vma->file_offset + pg_start - vma->vm_start);
	} else {
		// vm_start 不整页，且此时在分配此vma的第一个page。从vm_start开始拷贝。
		// TODO: 如果按照整页配置，pg_start -> vm_start 之间的内存没有拷贝。如果这一段本来是有内容的，下一次不会引发缺页中断。这是一个bug。
		pg_start = vma->vm_start;
		file_start = (uint64_t)uapp_start + vma->file_offset;
	}
	uint64_t limit = vma->file_size + vma->vm_start - pg_start;
	limit = limit > PGSIZE ? PGSIZE: limit;
	for (int j = 0; j < limit; j++){
		((char *)pg_start)[j] = ((char *)file_start)[j];
	}
	// Clear the others.
	for (int j = limit; j < PGSIZE - (pg_start - PGROUNDDOWN(stval)); j++ ){
		((char *)pg_start)[j] = 0;
	}
	flag |= VM_USER;
	create_mapping(current->pgd + PA2VA_OFFSET, pg_start, page_pa, 1, flag, 1, 1, 0);
	return;
}

struct task_struct * new_task(struct pt_regs * regs, struct task_struct * parent){
	int new = -1;
	for (int i = NR_TASKS; i < CAP_TASKS; i++){
		if (task[i] == NULL){
			new = i;
			break;
		}
	}
	if (new < 0) {
		printk("[Kernel Panic] Run out of pid.\n");
		while (1);
	}
	task[new] = (struct task_struct *)kalloc();
	for (int i = 0; i < sizeof(struct task_struct); i++){
		((char *)task[new])[i] = ((char *)parent)[i];
	}
	task[new]->pid = new;  // 2

	task[new]->thread.ra = (uint64)__return_from_fork;
	task[new]->thread.sp = regs->sp;  // 3 and 4
	task[new]->thread_info = (struct thread_info *)kalloc();
	task[new]->thread_info->kernel_sp = (uint64)task[new] + (uint64)parent->thread_info->kernel_sp - (uint64)parent;

	for (int i = 0; i < parent->mm.vma_cnt; i++){
		((struct vm_area_struct *)task[new]->mm.mmap)[i] = ((struct vm_area_struct *)parent->mm.mmap)[i];
	}

	num_task++;
	return task[new];
}