all: main

main: main.s
	riscv64-unknown-elf-gcc -Wl,-Ttext=0x0 -nostdlib -o main main.s
	riscv64-unknown-elf-objcopy -O binary main main.bin
