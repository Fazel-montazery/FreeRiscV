#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "fs.h"

#include "cpu.h"

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Usage: %s [riscv binary]\n", argv[0]);
		return -1;
	}

	struct CPU cpu = newCpu();
	if (!cpuLoadProgram(&cpu, argv[1])) return -1;
	cpuRun(&cpu);
	cpuPrintRegs(&cpu);

	return 0;
}
