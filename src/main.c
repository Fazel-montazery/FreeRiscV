#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "fs.h"

#include "cpu.h"
#include "ram.h"

#define MB(n) ((n) * 1024 * 1024)
#define DEFAULT_MEM_SIZE (MB(32)) // 32MB Default

int main(int argc, char** argv)
{
	if (argc < 2) {
		printf("Usage: %s [riscv binary] <ram size(MB)>\n", argv[0]);
		return -1;
	}

	// Creating a RAM
	struct RAM ram = newRam((argc == 3) ? MB(atoi(argv[2])) : DEFAULT_MEM_SIZE);
	if (!isRamValid(&ram)) return -1;

	// Initializing the BUS
	struct BUS bus = newBus(&ram);

	// Initializing the CPU
	struct CPU cpu = newCpu(&bus);
	if (!cpuLoadProgram(&cpu, argv[1])) return -1;
	cpuRun(&cpu);
	cpuPrintRegs(&cpu);

	ramDestroy(&ram);
	return 0;
}
