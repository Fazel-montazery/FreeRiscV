.data
a: .string "a\n"
b: .string "b\n"
c: .string "c\n"

.text
main:
1:
auipc gp, %pcrel_hi(__global_pointer$)
addi  gp, gp, %pcrel_lo(1b)
	li a0, 1
	la a1, a
	ecall

	la a1, b
	ecall

	la a1, c
	ecall
