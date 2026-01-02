	.file	"main.c"
	.option nopic
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_zicsr2p0_zifencei2p0_zmmul1p0_zaamo1p0_zalrsc1p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-16
	sd	ra,8(sp)
	sd	s0,0(sp)
	addi	s0,sp,16
	li	a0,10
	call	fact
	mv	a5,a0
	mv	a0,a5
	ld	ra,8(sp)
	ld	s0,0(sp)
	addi	sp,sp,16
	jr	ra
	.size	main, .-main
	.align	2
	.globl	fact
	.type	fact, @function
fact:
	addi	sp,sp,-32
	sd	ra,24(sp)
	sd	s0,16(sp)
	addi	s0,sp,32
	mv	a5,a0
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	sext.w	a4,a5
	li	a5,1
	bgt	a4,a5,.L4
	li	a5,1
	j	.L5
.L4:
	lw	a5,-20(s0)
	addiw	a5,a5,-1
	sext.w	a5,a5
	mv	a0,a5
	call	fact
	mv	a5,a0
	lw	a4,-20(s0)
	mulw	a5,a4,a5
	sext.w	a5,a5
.L5:
	mv	a0,a5
	ld	ra,24(sp)
	ld	s0,16(sp)
	addi	sp,sp,32
	jr	ra
	.size	fact, .-fact
	.ident	"GCC: (g1b306039ac) 15.1.0"
	.section	.note.GNU-stack,"",@progbits
