all: main

main: main.s
	riscv64-unknown-elf-gcc -Wl,-Ttext=0x0 -nostdlib -o main main.s
	riscv64-unknown-elf-objcopy -O binary main main.bin

c: main.c
	riscv64-unknown-elf-gcc -S main.c
	riscv64-unknown-elf-gcc -Wl,-Ttext=0x0 -nostdlib -march=rv64i -mabi=lp64 -o main main.s
	riscv64-unknown-elf-objcopy -O binary main main.bin
