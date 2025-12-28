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

		case 0x3: // I-type(Load)
			return (funct3 << 7) | opcode;

		case 0x23: // S-type
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

struct CPU newCpu(struct BUS* bus)
{
	struct CPU cpu = { 0 };
	cpu.regs[0] = 0; // x0 hard wired to zero
	cpu.regs[2] = bus->ram->size + RAM_BASE_ADDR; // x2 stack pointer
	cpu.pc = RAM_BASE_ADDR; // Program-couter
	cpu.bus = bus;
	return cpu;
}

bool cpuLoadProgram(struct CPU* cpu, const char* path)
{
	int64_t read_bytes;
	int64_t file_size;
	if ((file_size = readFileToBuf(path, NULL, 0, true)) < 0) return false;
	if ((read_bytes = readFileToBuf(path, cpu->bus->ram->bytes , cpu->bus->ram->size, true)) < 0) return false;
	if (read_bytes != file_size) {
		fprintf(stderr, "Could'nt read program to memory! (Out of mem)\n");
		return false;
	}
	return true;
}

// Loading without a bus to avoid mismatch of 32 and 64 bits
static bool cpuFetch(struct CPU* cpu, uint32_t* inst)
{
	return busLoadInst(cpu->bus, cpu->pc, inst);
}

static bool cpuExec(struct CPU* cpu, uint32_t inst)
{
	uint32_t instcode = cpuInstCode(inst);
        uint64_t rd = INST_RD(inst);
        uint64_t rs1 = INST_RS1(inst);
        uint64_t rs2 = INST_RS2(inst);

	switch (instcode) {
	// Arithmetic
	case INSTCODE_ADD: {
		cpu->regs[rd] = cpu->regs[rs1] + cpu->regs[rs2];
		return true;
	}

	case INSTCODE_SUB: {
		cpu->regs[rd] = cpu->regs[rs1] - cpu->regs[rs2];
		return true;
	}

	case INSTCODE_ADDI: {
		uint64_t imm = INST_IMM_I(inst);
		cpu->regs[rd] = cpu->regs[rs1] + imm;
		return true;
	}
	
	// Loads
	case INSTCODE_LB: {
		uint64_t imm = INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		uint64_t val;
		if (!busLoad(cpu->bus, addr, 1, &val)) return false;
		cpu->regs[rd] = (uint64_t)((int64_t)((int8_t) (val)));
		return true;
	}

	case INSTCODE_LH: {
		uint64_t imm = INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		uint64_t val;
		if (!busLoad(cpu->bus, addr, 2, &val)) return false;
		cpu->regs[rd] = (uint64_t)((int64_t)((int16_t) (val)));
		return true;
	}

	case INSTCODE_LW: {
		uint64_t imm = INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		uint64_t val;
		if (!busLoad(cpu->bus, addr, 4, &val)) return false;
		cpu->regs[rd] = (uint64_t)((int64_t)((int32_t) (val)));
		return true;
	}

	case INSTCODE_LD: {
		uint64_t imm = INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return busLoad(cpu->bus, addr, 8, &cpu->regs[rd]);
	}

	case INSTCODE_LBU: {
		uint64_t imm = INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return busLoad(cpu->bus, addr, 1, &cpu->regs[rd]);
	}

	case INSTCODE_LHU: {
		uint64_t imm = INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return busLoad(cpu->bus, addr, 2, &cpu->regs[rd]);
	}

	case INSTCODE_LWU: {
		uint64_t imm = INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return busLoad(cpu->bus, addr, 4, &cpu->regs[rd]);
	}

	case INSTCODE_SB: {
		uint64_t imm = INST_IMM_S(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return busStore(cpu->bus, addr, 1, cpu->regs[rs2]);
	}

	case INSTCODE_SH: {
		uint64_t imm = INST_IMM_S(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return busStore(cpu->bus, addr, 2, cpu->regs[rs2]);
	}

	case INSTCODE_SW: {
		uint64_t imm = INST_IMM_S(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return busStore(cpu->bus, addr, 4, cpu->regs[rs2]);
	}

	case INSTCODE_SD: {
		uint64_t imm = INST_IMM_S(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return busStore(cpu->bus, addr, 8, cpu->regs[rs2]);
	}

	default:
		return false;
	}
}

void cpuRun(struct CPU* cpu)
{
	while (true) {
		uint32_t inst;
		if(!cpuFetch(cpu, &inst)) break;
		cpu->pc += 4;
		if(!cpuExec(cpu, inst)) break;
		if(cpu->pc == 0) break;
	}
}
