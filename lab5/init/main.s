	.file	"main.c"
	.option nopic
	.text
	.section	.rodata
	.align	3
.LC0:
	.string	" Hello RISC-V\n"
	.text
	.align	2
	.globl	start_kernel
	.type	start_kernel, @function
start_kernel:
	addi	sp,sp,-16
	sd	ra,8(sp)
	sd	s0,0(sp)
	addi	s0,sp,16
	li	a0,2022
	call	printk
	; call	puti
	lui	a5,%hi(.LC0)
	addi	a0,a5,%lo(.LC0)
	call	printk
	; call	puts
	call	test
	li	a5,0
	mv	a0,a5
	ld	ra,8(sp)
	ld	s0,0(sp)
	addi	sp,sp,16
	jr	ra
	.size	start_kernel, .-start_kernel
	.ident	"GCC: (g5964b5cd727) 11.1.0"
	.section	.note.GNU-stack,"",@progbits
