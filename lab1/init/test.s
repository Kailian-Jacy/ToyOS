	.file	"test.c"
	.option nopic
	.text
	.align	2
	.globl	test
	.type	test, @function
test:
	addi	sp,sp,-16
	sd	s0,8(sp)
	addi	s0,sp,16
.L2:
	j	.L2
	.size	test, .-test
	.ident	"GCC: (g5964b5cd727) 11.1.0"
	.section	.note.GNU-stack,"",@progbits
