main:
	addi x4, x0, 500
	addi x5, x0, 90
	sub x6, x5, x4
	addi x2, x2, -8
	sd x6, 0(x2)
        ld x7, 0(x2)
	addi x2, x2, 8
