#include <proc.h>
#include <mm.h>
#include <rand.h>
#include "defs.h"

extern void __dummy();

struct task_struct* idle;           // idle process
struct task_struct* current;        // 指向当前运行线程的 `task_struct`
struct task_struct* task[NR_TASKS]; // 线程数组，所有的线程都保存在此

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
	current = idle;
	task[0] = idle;	// 5

    // 1. 参考 idle 的设置, 为 task[1] ~ task[NR_TASKS - 1] 进行初始化
    // 2. 其中每个线程的 state 为 TASK_RUNNING, counter 为 0, priority 使用 rand() 来设置, pid 为该线程在线程数组中的下标。
    // 3. 为 task[1] ~ task[NR_TASKS - 1] 设置 `thread_struct` 中的 `ra` 和 `sp`,
    // 4. 其中 `ra` 设置为 __dummy （见 4.3.2）的地址， `sp` 设置为 该线程申请的物理页的高地址

    /* YOUR CODE HERE */
	for (int i = 1; i < NR_TASKS; i++){
		task[i] = (struct task_struct *)kalloc();
		task[i]->state  = TASK_RUNNING;   
		task[i]->counter = 0;
		task[i]->priority = rand();  
		task[i]->pid = i;  // 2

		task[i]->thread.ra = (uint64)__dummy;
		task[i]->thread.sp = (uint64)task[i] + PGSIZE;  // 3 and 4
	}

    // printk("...proc_init done!\n");
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
		for (int i = 1; i < NR_TASKS; i++){
			if (task[i]->state == TASK_RUNNING && task[i]->counter != 0){
				flag = 1;
				next = i;
				break;
			}
		}
		if (!flag) {
			for (int i = 1; i < NR_TASKS; i++){
				// task[i]->counter = 2;
				task[i]->counter = rand();
        		printk("ReIniting. [PID = %d] . SET Counter = %d, SET Priority = %d\n", i, task[i]->counter, task[i]->priority);
			}
		}
	}
	for (int i = 2; i < NR_TASKS; i++){
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
		for (int i = 1; i < NR_TASKS; i++)
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
			for (int i = 1; i < NR_TASKS; i++)
			{
				// Task Counter contains its priority.
				task[i]->counter = rand();
				printk("ReIniting. [PID = %d] . SET Counter = %d, SET Priority = %d\n", i, task[i]->counter, task[i]->priority);
			}
		}
	}
	for (int i = 2; i < NR_TASKS; i++)
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