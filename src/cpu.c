#include "cpu.h"

struct CPU newCpu(void)
{
	struct CPU cpu;
	cpu.regs[0] = 0; // x0 hard wired to zero
	cpu.regs[2] = MEM_SIZE; // x2 stack pointer
	return cpu;
}
