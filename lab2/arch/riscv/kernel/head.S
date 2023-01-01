.extern start_kernel
    .section .text.init
    .globl _start
    .align 2
_start:
    la sp, boot_stack_top
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
    ecall
    # enable interrupt sstatus.SIE = 1
    csrr t0, sstatus
    ori t0, t0, 0x2
    csrw sstatus, t0

    j start_kernel

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096
    .globl boot_stack_top
boot_stack_top:
