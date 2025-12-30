#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "frv.h"

#define MB(n) ((n) * 1024 * 1024)
#define DEFAULT_MEM_SIZE (MB(32)) // 32MB Default

int main(int argc, char** argv)
{
	if (argc < 2) {
		printf("Usage: %s [riscv binary] <ram size(MB)>\n", argv[0]);
		return -1;
	}

	// Creating a RAM
	struct FrvRAM ram = frvNewRam((argc == 3) ? MB(atoi(argv[2])) : DEFAULT_MEM_SIZE);
	if (!frvIsRamValid(&ram)) return -1;

	// Initializing the BUS
	struct FrvBUS bus = frvNewBus(&ram);

	// Initializing the CPU
	struct FrvCPU cpu = frvNewCpu(&bus);
	if (!frvCpuLoadProgram(&cpu, argv[1])) return -1;
	frvCpuRun(&cpu);
	frvCpuPrintRegs(&cpu);
	frvCpuPrintCsrs(&cpu);

	frvRamDestroy(&ram);
	return 0;
}
