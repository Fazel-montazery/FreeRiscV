main:
	addi t0, zero, 10
	addi t1, zero, 12
	mul t2, t0, t1
	addi a0, zero, 0
	add a1, zero, t2
	ecall
