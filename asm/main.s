# From: https://github.com/lftraining/LFD110x/blob/main/src/Terminal%20Interface/terminal.asm
# with modifications

# Data Section
.data
prompt:   .string   "what's your name?\n"
response: .string   "I'm happy to see you, "
.equ name_size, 64
name:     
	.space name_size

# Code Section
.text
1:
auipc gp, %pcrel_hi(__global_pointer$)
addi  gp, gp, %pcrel_lo(1b)
main:
    # print_string(prompt) - Environment call 1
    li a0, 1 # li for load imm (psudo-inst)
    la a1, prompt # load address
    ecall

    # scan_string - ECALL 5
    li a0, 5
    la a1, name # buf
    li a2, name_size # buf siz
    ecall

    # print_string(response) - Environment call 1
    li a0, 1
    la a1, response
    ecall

    # print_string(name) - Environment call 1
    li a0, 1
    la a1, name
    ecall

    # print_char(a0) - Environment call 2
    li a0, 2
    li a1, '!'
    ecall

    li a1, '\n'
    ecall

    # End - Environment call 7
    li a0, 7
    ecall
