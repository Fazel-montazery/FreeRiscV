#include "cpu.h"

// Create a unique cose for each instruction
static uint32_t cpuInstCode(const uint32_t inst)
{
	const uint32_t opcode = INST_OPCODE(inst);
	const uint32_t funct3 = INST_FUNCT3(inst);
	const uint32_t funct7 = INST_FUNCT7(inst);
	switch (opcode) {
		case 0x33: // R-type
			return (funct7 << 10) | (funct3 << 7) | opcode;

		case 0x13: // I-type
			return (funct3 << 7) | opcode;

		default:
			return 0xFFFFFFFF;
	}
}

void cpuPrintRegs(struct CPU* cpu)
{
	printf("PC => 0x%lX | %ld\n", cpu->pc, cpu->pc);
	for (int i = 0; i < NUM_REGS; i++) {
		printf("x%d => 0x%lX | %ld\n", i, cpu->regs[i], cpu->regs[i]);
	}
}

struct CPU newCpu(void)
{
	struct CPU cpu = { 0 };
	cpu.regs[0] = 0; // x0 hard wired to zero
	cpu.regs[2] = MEM_SIZE; // x2 stack pointer
	cpu.pc = 0; // Program-couter
	return cpu;
}

bool cpuLoadProgram(struct CPU* cpu, const char* path)
{
	int64_t read_bytes;
	int64_t file_size;
	if ((file_size = readFileToBuf(path, NULL, 0, true)) < 0) return false;
	if ((read_bytes = readFileToBuf(path, cpu->mem, MEM_SIZE, true)) < 0) return false;
	if (read_bytes != file_size) {
		fprintf(stderr, "Could'nt read program to memory! (Out of mem)\n");
		return false;
	}
	return true;
}

static uint32_t cpuFetch(struct CPU* cpu)
{
	size_t i = (size_t) cpu->pc;
	return (uint32_t)cpu->mem[i]
		| ((uint32_t)cpu->mem[i+1] << 8)
		| ((uint32_t)cpu->mem[i+2] << 16)
		| ((uint32_t)cpu->mem[i+3] << 24);
}

static bool cpuExec(struct CPU* cpu, uint32_t inst)
{
	uint32_t instcode = cpuInstCode(inst);
        size_t rd = INST_RD(inst);
        size_t rs1 = INST_RS1(inst);
        size_t rs2 = INST_RS2(inst);

	switch (instcode) {
	case INSTCODE_ADDI: {
		uint64_t imm = INST_IMM_I(inst);
		cpu->regs[rd] = cpu->regs[rs1] + imm;
		break;
	}

	case INSTCODE_ADD: {
		cpu->regs[rd] = cpu->regs[rs1] + cpu->regs[rs2];
		break;
	}

	case INSTCODE_SUB: {
		cpu->regs[rd] = cpu->regs[rs1] - cpu->regs[rs2];
		break;
	}
	
	default:
		   return false;
	}

	return true;
}

bool cpuRun(struct CPU* cpu)
{
	while (true) {
		uint32_t inst = cpuFetch(cpu);
		cpu->pc += 4;
		if(!cpuExec(cpu, inst)) return false;
	}

	return true;
}
