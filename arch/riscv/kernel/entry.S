.equ reg_size, 0x8
    
    .section .text.entry
    .align 2
    .globl _trap    
_trap:
    csrw sscratch, sp

    # save 31 register (x0 hardwired zero) and sepc
    addi sp, sp, -32*reg_size

    sd x1, 0*reg_size(sp)
    sd x3, 2*reg_size(sp)
    sd x4, 3*reg_size(sp)
    sd x5, 4*reg_size(sp)
    sd x6, 5*reg_size(sp)
    sd x7, 6*reg_size(sp)
    sd x8, 7*reg_size(sp)
    sd x9, 8*reg_size(sp)
    sd x10, 9*reg_size(sp)
    sd x11, 10*reg_size(sp)
    sd x12, 11*reg_size(sp)
    sd x13, 12*reg_size(sp)
    sd x14, 13*reg_size(sp)
    sd x15, 14*reg_size(sp)
    sd x16, 15*reg_size(sp)
    sd x17, 16*reg_size(sp)
    sd x18, 17*reg_size(sp)
    sd x19, 18*reg_size(sp)
    sd x20, 19*reg_size(sp)
    sd x21, 20*reg_size(sp)
    sd x22, 21*reg_size(sp)
    sd x23, 22*reg_size(sp)
    sd x24, 23*reg_size(sp)
    sd x25, 24*reg_size(sp)
    sd x26, 25*reg_size(sp)
    sd x27, 26*reg_size(sp)
    sd x28, 27*reg_size(sp)
    sd x29, 28*reg_size(sp)
    sd x30, 29*reg_size(sp)
    sd x31, 30*reg_size(sp)
    
    csrr t0, sepc
    sd t0, 31*reg_size(sp)

# CSRRW reads the old value of the CSR, zero-extends the value to XLEN bits, then writes it to integer register rd.
# The initial value in rs1 is written to the CSR. If rd=x0, then the instruction shall not read the CSR and
# shall not cause any of the side effects that might occur on a CSR read. 
    csrrw t0, sscratch, x0
    sd t0, 1*reg_size(sp)

    call current_task  
    addi a0, a0, 0x28
    ld t0, 16*reg_size(a0)

    bne t0, x0, BR1
    csrr a0, scause
    csrr a1, sepc
    mv a2, sp
    call trap_handler
    beq x0, x0, _recover
BR1:
    call current_task  
    sd sp, 1*reg_size(a0)
    ld sp, 0*reg_size(a0)
    sd x0, 21*reg_size(a0)

    csrr a0, scause
    csrr a1, sepc
    mv a2, sp
    call trap_handler

    call current_task 
    sd sp, 0*reg_size(a0)
    ld sp, 1*reg_size(a0)
    addi t0, x0, 1
    sd t0, 21*reg_size(a0)
    j _recover

    .global __return_from_fork
__return_from_fork:
    addi sp, sp, -32*reg_size
_recover:
    # load sepc and 31 register (x2(sp) should be loaded last)
    ld t0, 31*reg_size(sp)
    csrw sepc, t0

    ld x1, 0*reg_size(sp)
    ld x3, 2*reg_size(sp)
    ld x4, 3*reg_size(sp)
    ld x5, 4*reg_size(sp)
    ld x6, 5*reg_size(sp)
    ld x7, 6*reg_size(sp)
    ld x8, 7*reg_size(sp)
    ld x9, 8*reg_size(sp)
    ld x10, 9*reg_size(sp)
    ld x11, 10*reg_size(sp)
    ld x12, 11*reg_size(sp)
    ld x13, 12*reg_size(sp)
    ld x14, 13*reg_size(sp)
    ld x15, 14*reg_size(sp)
    ld x16, 15*reg_size(sp)
    ld x17, 16*reg_size(sp)
    ld x18, 17*reg_size(sp)
    ld x19, 18*reg_size(sp)
    ld x20, 19*reg_size(sp)
    ld x21, 20*reg_size(sp)
    ld x22, 21*reg_size(sp)
    ld x23, 22*reg_size(sp)
    ld x24, 23*reg_size(sp)
    ld x25, 24*reg_size(sp)
    ld x26, 25*reg_size(sp)
    ld x27, 26*reg_size(sp)
    ld x28, 27*reg_size(sp)
    ld x29, 28*reg_size(sp)
    ld x30, 29*reg_size(sp)
    ld x31, 30*reg_size(sp)
    ld x2, 1*reg_size(sp)

    sret

    .global __dummy
__dummy:
	# dummy is always called after __switch_to, where a1 = current;
    # addi t0, a1, 0x28
    # ld sp, 16*reg_size(t0)
    # TODO: swap?
    # now we are heading back to uapp but not dummy.
    # ld t0, 14*reg_size(t0)
    # la x5, dummy
    # csrw sepc, t0
    sret

    .globl __switch_to
__switch_to:
    # save state to prev process
    # YOUR CODE HERE

    # now a0 is the current task_struct pointer.
    # thread_struct = 5*sizeof(uint64) + a0 

    addi t0, a0, 0x28

    # save each register.
    sd ra, 0*reg_size(t0)
    # now we are going back to user mode. We are recovering thread.sscratch (user mode suser mode sp)
    sd sp, 1*reg_size(t0)
    sd s0, 2*reg_size(t0)
    sd s1, 3*reg_size(t0)
    sd s2, 4*reg_size(t0)
    sd s3, 5*reg_size(t0)
    sd s4, 6*reg_size(t0)
    sd s5, 7*reg_size(t0)
    sd s6, 8*reg_size(t0)
    sd s7, 9*reg_size(t0)
    sd s8, 10*reg_size(t0)
    sd s9, 11*reg_size(t0)
    sd s10, 12*reg_size(t0)
    sd s11, 13*reg_size(t0)

    csrr t1, sepc
    sd t1, 14*reg_size(t0)
    csrr t1, sstatus
    sd t1, 15*reg_size(t0)
    csrr t1, sscratch
    sd t1, 16*reg_size(t0)

__new_switch:
    # restore state from next process
    # YOUR CODE HERE
    addi t0, a1, 0x28

    ld ra, 0*reg_size(t0)
    ld sp, 1*reg_size(t0)
    ld s0, 2*reg_size(t0)
    ld s1, 3*reg_size(t0)
    ld s2, 4*reg_size(t0)
    ld s3, 5*reg_size(t0)
    ld s4, 6*reg_size(t0)
    ld s5, 7*reg_size(t0)
    ld s6, 8*reg_size(t0)
    ld s7, 9*reg_size(t0)
    ld s8, 10*reg_size(t0)
    ld s9, 11*reg_size(t0)
    ld s10, 12*reg_size(t0)
    ld s11, 13*reg_size(t0)

    ld t1, 14*reg_size(t0)
    csrw sepc, t1
    ld t1, 15*reg_size(t0)
    csrw sstatus, t1
    ld t1, 16*reg_size(t0)
    csrw sscratch, t1

    # Change page table.
    and t1, t1, x0
    ld t1, 17*reg_size(t0)
    srli t1, t1, 0xc
    addi t2, zero, 0x8
    slli t2, t2, 0x3c
    or t1, t1, t2
    csrw satp, t1

	# flush tlb.
    sfence.vma zero, zero

    ret