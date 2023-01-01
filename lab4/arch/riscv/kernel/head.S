.extern start_kernel
    .section .text.init
    .globl _start
    .globl _set_satp_with_swapper
    .align 2
_start:
    la sp, boot_stack_top

    call setup_vm
    call relocate

    # init memory and task
    call mm_init
    call task_init
    
    # setup_vm_final requires allocating pages. So we put it here.
    call setup_vm_final
    

    # set stvec = _trap  
    la t0, _trap
    csrw stvec, t0
    # enable timer interrupt sie.STIE = 1
    csrr t0, sie
    ori t0, t0, 0x20
    csrw sie, t0 
    # set first time interrupt 
    rdtime t0
    li t1, 10000000
    add a0, t0, t1
    add a6, zero, zero
    add a7, zero, zero
    # How can you step into an ecall? 
    # With normal gdb you cannot inspect machine mode registers.
    ecall   
    # enable interrupt sstatus.SIE = 1
    csrr t0, sstatus
    ori t0, t0, 0x2
    csrw sstatus, t0


    j start_kernel

relocate:
    # set ra = ra + PA2VA_OFFSET;
    # set sp = sp + PA2VA_OFFSET;

    li t0, 0xffffffdf 
    slli t0, t0, 0x20
    li t1, 0x80000000
    or t0, t0, t1

    add ra, ra, t0
    add sp, sp, t0

    # set satp with early_pgtbl
    # satp: 60-63->8; 44-59->0 ASID ; 0->43 PPN early_pgtbl > 12 ?
    la  t0, early_pgtbl
    srli t0, t0, 0xc
    addi t1, zero, 0x8
    slli t1, t1, 0x3c
    or t0, t0, t1
    csrw satp, t0

    # flush tlb
    sfence.vma zero, zero

    ret

_set_satp_with_swapper:
    add  t0, zero, a0
    srli t0, t0, 0xc
    addi t1, zero, 0x8
    slli t1, t1, 0x3c
    or t0, t0, t1
    csrw satp, t0
    ret
    
    # this part should be in the last of file. Preserve space.
    .section .bss.stack
    .globl boot_stack

boot_stack:
    .space 4096
    .globl boot_stack_top
    # After loading. boot_stack_top is at 0x0··080205000. Stack space 4096.
boot_stack_top: