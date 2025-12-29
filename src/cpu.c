#include "cpu.h"

// Create a unique cose for each instruction
static uint32_t frvCpuInstCode(const uint32_t inst)
{
	const uint32_t opcode = FRV_INST_OPCODE(inst);
	const uint32_t funct3 = FRV_INST_FUNCT3(inst);
	const uint32_t funct7 = FRV_INST_FUNCT7(inst);
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

void frvCpuPrintRegs(struct FrvCPU* cpu)
{
	printf("PC => 0x%lX | %ld\n", cpu->pc, cpu->pc);
	for (int i = 0; i < FRV_NUM_REGS; i++) {
		printf("x%d => 0x%lX | %ld\n", i, cpu->regs[i], cpu->regs[i]);
	}
}

struct FrvCPU frvNewCpu(struct FrvBUS* bus)
{
	struct FrvCPU cpu = { 0 };
	cpu.regs[0] = 0; // x0 hard wired to zero
	cpu.regs[2] = bus->ram->size + FRV_RAM_BASE_ADDR; // x2 stack pointer
	cpu.pc = FRV_RAM_BASE_ADDR; // Program-couter
	cpu.bus = bus;
	return cpu;
}

bool frvCpuLoadProgram(struct FrvCPU* cpu, const char* path)
{
	int64_t read_bytes;
	int64_t file_size;
	if ((file_size = frvReadFileToBuf(path, NULL, 0, true)) < 0) return false;
	if ((read_bytes = frvReadFileToBuf(path, cpu->bus->ram->bytes , cpu->bus->ram->size, true)) < 0) return false;
	if (read_bytes != file_size) {
		fprintf(stderr, "Could'nt read program to memory! (Out of mem)\n");
		return false;
	}
	return true;
}

// Loading without a bus to avoid mismatch of 32 and 64 bits
static bool frvCpuFetch(struct FrvCPU* cpu, uint32_t* inst)
{
	return frvBusLoadInst(cpu->bus, cpu->pc, inst);
}

static bool frvCpuExec(struct FrvCPU* cpu, uint32_t inst)
{
	uint32_t instcode = frvCpuInstCode(inst);
        uint64_t rd = FRV_INST_RD(inst);
        uint64_t rs1 = FRV_INST_RS1(inst);
        uint64_t rs2 = FRV_INST_RS2(inst);

	switch (instcode) {
	// Arithmetic
	case FRV_INSTCODE_ADD: {
		cpu->regs[rd] = cpu->regs[rs1] + cpu->regs[rs2];
		return true;
	}

	case FRV_INSTCODE_SUB: {
		cpu->regs[rd] = cpu->regs[rs1] - cpu->regs[rs2];
		return true;
	}

	case FRV_INSTCODE_ADDI: {
		uint64_t imm = FRV_INST_IMM_I(inst);
		cpu->regs[rd] = cpu->regs[rs1] + imm;
		return true;
	}
	
	// Loads
	case FRV_INSTCODE_LB: {
		uint64_t imm = FRV_INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		uint64_t val;
		if (!frvBusLoad(cpu->bus, addr, 1, &val)) return false;
		cpu->regs[rd] = (uint64_t)((int64_t)((int8_t) (val)));
		return true;
	}

	case FRV_INSTCODE_LH: {
		uint64_t imm = FRV_INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		uint64_t val;
		if (!frvBusLoad(cpu->bus, addr, 2, &val)) return false;
		cpu->regs[rd] = (uint64_t)((int64_t)((int16_t) (val)));
		return true;
	}

	case FRV_INSTCODE_LW: {
		uint64_t imm = FRV_INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		uint64_t val;
		if (!frvBusLoad(cpu->bus, addr, 4, &val)) return false;
		cpu->regs[rd] = (uint64_t)((int64_t)((int32_t) (val)));
		return true;
	}

	case FRV_INSTCODE_LD: {
		uint64_t imm = FRV_INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return frvBusLoad(cpu->bus, addr, 8, &cpu->regs[rd]);
	}

	case FRV_INSTCODE_LBU: {
		uint64_t imm = FRV_INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return frvBusLoad(cpu->bus, addr, 1, &cpu->regs[rd]);
	}

	case FRV_INSTCODE_LHU: {
		uint64_t imm = FRV_INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return frvBusLoad(cpu->bus, addr, 2, &cpu->regs[rd]);
	}

	case FRV_INSTCODE_LWU: {
		uint64_t imm = FRV_INST_IMM_I(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return frvBusLoad(cpu->bus, addr, 4, &cpu->regs[rd]);
	}

	case FRV_INSTCODE_SB: {
		uint64_t imm = FRV_INST_IMM_S(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return frvBusStore(cpu->bus, addr, 1, cpu->regs[rs2]);
	}

	case FRV_INSTCODE_SH: {
		uint64_t imm = FRV_INST_IMM_S(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return frvBusStore(cpu->bus, addr, 2, cpu->regs[rs2]);
	}

	case FRV_INSTCODE_SW: {
		uint64_t imm = FRV_INST_IMM_S(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return frvBusStore(cpu->bus, addr, 4, cpu->regs[rs2]);
	}

	case FRV_INSTCODE_SD: {
		uint64_t imm = FRV_INST_IMM_S(inst);
		uint64_t addr = cpu->regs[rs1] + imm;
		return frvBusStore(cpu->bus, addr, 8, cpu->regs[rs2]);
	}

	default:
		return false;
	}
}

void frvCpuRun(struct FrvCPU* cpu)
{
	while (true) {
		uint32_t inst;
		if(!frvCpuFetch(cpu, &inst)) break;
		cpu->pc += 4;
		if(!frvCpuExec(cpu, inst)) break;
		if(cpu->pc == 0) break;
	}
}
