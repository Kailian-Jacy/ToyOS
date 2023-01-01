#ifndef __PROC_H
#define __PROC_H
#include "types.h"
#include "vm.h"

#define ELF_READ 0x01
#define ELF_WRITE 0x02
#define ELF_EXEC 0x04

// Self defined.
#define VM_VALID (0x01)
#define VM_READ (0x01 << 1)
#define VM_WRITE (0x01 << 2)
#define VM_EXEC (0x01 << 3)
#define VM_USER (0x01 << 4)

typedef uint64_t pagetable_t;

#define CAP_TASKS  (1 + 9) // 用于控制最大线程数量（idle线程 + 31内核线程）
#define NR_TASKS  (1 + 3) // 用于控制最大线程数量（idle线程 + 31内核线程）
// #define NR_TASKS  (1 + 31) // 用于控制最大线程数量（idle线程 + 31内核线程）
#define TASK_RUNNING 0 // 为了简化实验，所有的线程都只有一种状态
#define PRIORITY_MIN 1
#define PRIORITY_MAX 10

#define KERNEL_MODE 0
#define USER_MODE 1

struct pt_regs {
    // All the registers are 64 bits.
    // x1, 0*reg_size(sp)
    uint64_t ra;    
    // x2, 1*reg_size(sp)
    uint64_t sp;  
    // x3, 2*reg_size(sp)
    uint64_t gp;    
    // x4, 3*reg_size(sp)
    uint64_t tp;  
    // x5, 4*reg_size(sp)
    uint64_t t0;  
    // x6, 5*reg_size(sp)
    uint64_t t1;  
    // x7, 6*reg_size(sp)
    uint64_t t2;  

    // x8, 7*reg_size(sp)
    uint64_t fp;  
    // x9, 8*reg_size(sp)
    uint64_t s1;  
    // x10, 9*reg_size(sp)
    uint64_t a0;  
    // x11, 10*reg_size(sp)
    uint64_t a1;  
    // x12, 11*reg_size(sp)
    uint64_t a2;  
    // x13, 12*reg_size(sp)
    uint64_t a3;  
    // x14, 13*reg_size(sp)
    uint64_t a4;  
    // x15, 14*reg_size(sp)
    uint64_t a5;  
    // x16, 15*reg_size(sp)
    uint64_t a6;  
    // x17, 16*reg_size(sp)
    uint64_t a7;  
    // x18, 17*reg_size(sp)
    uint64_t s2;  
    // x19, 18*reg_size(sp)
    uint64_t s3;  
    // x20, 19*reg_size(sp)
    uint64_t s4;  
    // x21, 20*reg_size(sp)
    uint64_t s5;  
    // x22, 21*reg_size(sp)
    uint64_t s6;  
    // x23, 22*reg_size(sp)
    uint64_t s7;  
    // x24, 23*reg_size(sp)
    uint64_t s8;  
    // x25, 24*reg_size(sp)
    uint64_t s9;  
    // x26, 25*reg_size(sp)
    uint64_t s10;  
    // x27, 26*reg_size(sp)
    uint64_t s11;  
    // x28, 27*reg_size(sp)
    uint64_t t3;  
    // x29, 28*reg_size(sp)
    uint64_t t4;  
    // x30, 29*reg_size(sp)
    uint64_t t5;  
    // x31, 30*reg_size(sp)
    uint64_t t6;  
    // t0, 31*reg_size(sp)
    uint64_t sepc;  
};

struct vm_area_struct
{
    // struct mm_struct * vm_mm;
    uint64_t vm_start;
    uint64_t vm_end;
    uint64_t file_offset;
    uint64_t file_size;
    uint64_t flags;
    struct vm_area_struct *vm_next, *vm_prev;
};

struct mm_struct {
    int vma_cnt;
    struct vm_area_struct mmap[0];
};

/* 用于记录线程的内核栈指针、用户栈指针 */
/* (Lab3中无需考虑，在这里引入是为了之后实验的使用) */
struct thread_info {
    uint64 kernel_sp;
    uint64 user_sp;
};

/* 线程状态段数据结构 */
struct thread_struct {
    uint64 ra;
    uint64 sp;
    uint64 s[12];
    uint64_t sepc, sstatus, sscratch; 
};

/* 进程数据结构 */
struct task_struct {
    struct thread_info* thread_info;
    uint64 state;    // 进程状态
    uint64 counter;  // 运行剩余时间
    uint64 priority; // 运行优先级，1最低，10最高
    uint64 pid;      // 进程id

    struct thread_struct thread;

    pagetable_t pgd;
    // Why we need mm linked list? 
    // struct mm_struct *mm;
    struct mm_struct mm;
};

/* Find available task pid and return */
struct task_struct * new_task(struct pt_regs * regs, struct task_struct * parent);

/* 线程初始化，创建NR_TASKS个线程 */
void task_init();

/* 在时钟中断处理中被调用，用于判断是否需要进行调度 */
void do_timer();

/* 调度程序，选出下一个运行的线程 */
void schedule();

/* 线程切换入口函数*/
void switch_to(struct task_struct* next);

/* dummy function: 一个循环程序，循环输出自己的pid以及一个自增的局部变量*/
void dummy();

/* page fault handler. */
void do_page_fault(struct pt_regs *regs);

// Expose current task pointer.
struct task_struct* current_task();

void load_uapp(int i);

struct task_struct* current_task();

#endif