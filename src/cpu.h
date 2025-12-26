#pragma once

#include <stdint.h>

#include "defs.h"

struct CPU {
	uint64_t	pc;
	uint64_t	regs[NUM_REGS];
	uint8_t		mem[MEM_SIZE];
};

struct CPU newCpu(void);
uint32_t cpuFetch(struct CPU* cpu);
void cpuExec(struct CPU* cpu, uint32_t instruction);
