#include "cpu.h"

// Create a unique cose for each instruction
static uint32_t frvCpuInstCode(const uint32_t inst)
{
	const uint32_t opcode = FRV_INST_OPCODE(inst);
	const uint32_t funct3 = FRV_INST_FUNCT3(inst);
	const uint32_t funct6 = FRV_INST_FUNCT6(inst);
	const uint32_t funct7 = FRV_INST_FUNCT7(inst);
	const uint32_t funct12 = FRV_INST_FUNCT12(inst);
	switch (opcode) {
		case 0x33: // R-type
		case 0x3b: // RV64I
			return (funct7 << 10) | (funct3 << 7) | opcode;

		case 0x13: // I-type
			switch (funct3) {
			case 0x1:
			case 0x5:
				return (funct6 << 10)| (funct3 << 7) | opcode;
			default:
				return (funct3 << 7) | opcode;
			}

		case 0x37: // U-type, J-type
		case 0x17:
		case 0x6f:
			return opcode;

		case 0x73: // CSR, ECALL
			switch (funct3) {
			case 0:
				return (funct12 << 10) | (funct3 << 7) | opcode;
			default:
				return (funct3 << 7) | opcode;
			}

		case 0x23: // S-type, B-type, I-type(Load), J-type, CSR
		case 0x63:
		case 0x67:
		case 0x3:
		case 0x0f: // Fence
			return (funct3 << 7) | opcode;

		case 0x1b: // RV64I
			switch (funct3) {
			case 0x0:
				return (funct3 << 7) | opcode;
			default:
				return (funct7 << 10) | (funct3 << 7) | opcode;
			}

		default:
			return 0xFFFFFFFF;
	}
}

static inline uint64_t frvMulhu(const uint64_t a, const uint64_t b)
{
	unsigned __int128 r = (unsigned __int128)a * (unsigned __int128)b;
	return (uint64_t)(r >> 64);
}

static inline uint64_t frvMulh(const int64_t a, const int64_t b)
{
	__int128 r = (__int128)a * (__int128)b;
	return (uint64_t)(r >> 64);
}

static inline uint64_t frvMulhsu(const int64_t a, const uint64_t b)
{
	__int128 r = (__int128)a * (unsigned __int128)b;
	return (uint64_t)(r >> 64);
}

static inline uint64_t frvMulw(const uint64_t a, const uint64_t b)
{
	return (uint64_t)((int64_t)(int32_t)(((int32_t) a) * ((int32_t) b)));
}

static inline uint64_t frvDivw(const uint64_t a, const uint64_t b)
{
	return (b) ? ((uint64_t)((int64_t)(int32_t)(((int32_t) a) / ((int32_t) b)))) : -1;
}

static inline uint64_t frvDivuw(const uint64_t a, const uint64_t b)
{
	return (b) ? ((uint64_t)((int64_t)((int32_t)(((uint32_t) a) / ((uint32_t) b))))) : -1;
}

static inline uint64_t frvRemw(const uint64_t a, const uint64_t b)
{
	return (b) ? ((uint64_t)((int64_t)((int32_t)(((int32_t) a) % ((int32_t) b))))) : a;
}

static inline uint64_t frvRemuw(const uint64_t a, const uint64_t b)
{
	return (b) ? ((uint64_t)((int64_t)((int32_t)(((uint32_t) a) % ((uint32_t) b))))) : a;
}

static uint64_t frvLoadCsr(const struct FrvCPU* const cpu, const uint32_t addr) 
{
	switch (addr) {
	case FRV_CSR_SIE:
		return cpu->csrs[FRV_CSR_MIE] & cpu->csrs[FRV_CSR_MIDELEG];

	default:
		return cpu->csrs[addr];
	}
}

static void frvStoreCsr(struct FrvCPU* cpu, const uint32_t addr, const uint64_t val) 
{
	switch (addr) {
	case FRV_CSR_SIE:
		cpu->csrs[FRV_CSR_MIE] = (cpu->csrs[FRV_CSR_MIE] &
				!cpu->csrs[FRV_CSR_MIDELEG]) | (val & cpu->csrs[FRV_CSR_MIDELEG]);

	default:
		cpu->csrs[addr] = val;
	}
}

void frvCpuPrintRegs(const struct FrvCPU* const cpu)
{
	printf("PC => 0x%lX | %ld\n", cpu->pc, cpu->pc);
	for (int i = 0; i < FRV_NUM_REGS; i++) {
		printf("x%d => 0x%lX | %ld\n", i, cpu->regs[i], cpu->regs[i]);
	}
}

void frvCpuPrintCsrs(const struct FrvCPU* const cpu)
{
	printf("mstatus=0x%lX\tmtvec=0x%lX\tmepc=0x%lX\tmcause=0x%lX\n",
                frvLoadCsr(cpu, FRV_CSR_MSTATUS),
                frvLoadCsr(cpu, FRV_CSR_MTVEC),
                frvLoadCsr(cpu, FRV_CSR_MEPC),
                frvLoadCsr(cpu, FRV_CSR_MCAUSE)
        );

	printf("sstatus=0x%lX\tstvec=0x%lX\tsepc=0x%lX\tscause=0x%lX\n",
                frvLoadCsr(cpu, FRV_CSR_SSTATUS),
                frvLoadCsr(cpu, FRV_CSR_STVEC),
                frvLoadCsr(cpu, FRV_CSR_SEPC),
                frvLoadCsr(cpu, FRV_CSR_SCAUSE)
        );
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
	uint32_t csr = FRV_INST_CSR_CODE(inst);

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

	case FRV_INSTCODE_ADDIW: {
		uint64_t imm = FRV_INST_IMM_I(inst);
		cpu->regs[rd] = (uint64_t)((int64_t)((int32_t)(cpu->regs[rs1] + imm)));
		return true;
	}

	case FRV_INSTCODE_ADDW: {
		cpu->regs[rd] = (uint64_t)((int64_t)((int32_t)(cpu->regs[rs1] + cpu->regs[rs2])));
		return true;
	}

	case FRV_INSTCODE_SUBW: {
		cpu->regs[rd] = (uint64_t)((int32_t)(cpu->regs[rs1] - cpu->regs[rs2]));
		return true;
	}

	// Logic
	case FRV_INSTCODE_ANDI: {
		uint64_t imm = FRV_INST_IMM_I(inst);
		cpu->regs[rd] = cpu->regs[rs1] & imm;
		return true;
	}

	case FRV_INSTCODE_ORI: {
		uint64_t imm = FRV_INST_IMM_I(inst);
		cpu->regs[rd] = cpu->regs[rs1] | imm;
		return true;
	}

	case FRV_INSTCODE_XORI: {
		uint64_t imm = FRV_INST_IMM_I(inst);
		cpu->regs[rd] = cpu->regs[rs1] ^ imm;
		return true;
	}

	case FRV_INSTCODE_SLLI: {
		cpu->regs[rd] = cpu->regs[rs1] << FRV_INST_SHAMT64(inst);
		return true;
	}

	case FRV_INSTCODE_SRLI: {
		cpu->regs[rd] = cpu->regs[rs1] >> FRV_INST_SHAMT64(inst);
		return true;
	}

	case FRV_INSTCODE_SRAI: {
		cpu->regs[rd] = ((int64_t)cpu->regs[rs1]) >> FRV_INST_SHAMT64(inst);
		return true;
	}

	case FRV_INSTCODE_AND: {
		cpu->regs[rd] = cpu->regs[rs1] & cpu->regs[rs2];
		return true;
	}

	case FRV_INSTCODE_OR: {
		cpu->regs[rd] = cpu->regs[rs1] | cpu->regs[rs2];
		return true;
	}

	case FRV_INSTCODE_XOR: {
		cpu->regs[rd] = cpu->regs[rs1] ^ cpu->regs[rs2];
		return true;
	}

	case FRV_INSTCODE_SLL: {
		cpu->regs[rd] = cpu->regs[rs1] << (cpu->regs[rs2] & 0x3f);
		return true;
	}

	case FRV_INSTCODE_SRL: {
		cpu->regs[rd] = cpu->regs[rs1] >> (cpu->regs[rs2] & 0x3f);
		return true;
	}

	case FRV_INSTCODE_SRA: {
		cpu->regs[rd] = ((int64_t)cpu->regs[rs1]) >> (cpu->regs[rs2] & 0x3f);
		return true;
	}

	case FRV_INSTCODE_SLLIW: {
		cpu->regs[rd] = (uint64_t)((int64_t)((int32_t)(cpu->regs[rs1] << FRV_INST_SHAMT32(inst))));
		return true;
	}

	case FRV_INSTCODE_SRLIW: {
		cpu->regs[rd] = (uint64_t)((int64_t)((int32_t)(((uint32_t)cpu->regs[rs1]) >> FRV_INST_SHAMT32(inst))));
		return true;
	}

	case FRV_INSTCODE_SRAIW: {
		cpu->regs[rd] = (uint64_t)((int64_t)(((int32_t)cpu->regs[rs1]) >> FRV_INST_SHAMT32(inst)));
		return true;
	}
	
	case FRV_INSTCODE_SLLW: {
		uint32_t shamt = ((uint32_t)(cpu->regs[rs2] & 0x1f));
		cpu->regs[rd] = (uint64_t)((int32_t)(((uint32_t)cpu->regs[rs1]) << shamt));
		return true;
	}

	case FRV_INSTCODE_SRLW: {
		uint32_t shamt = ((uint32_t)(cpu->regs[rs2] & 0x1f));
		cpu->regs[rd] = (uint64_t)((int32_t)(((uint32_t)cpu->regs[rs1]) >> shamt));
		return true;
	}

	case FRV_INSTCODE_SRAW: {
		int32_t shamt = ((int32_t)(cpu->regs[rs2] & 0x1f));
		cpu->regs[rd] = (uint64_t)(((int32_t)cpu->regs[rs1]) >> shamt);
		return true;
	}

	// Compares
	case FRV_INSTCODE_SLTI: {
		int64_t imm = FRV_INST_IMM_I(inst);
		cpu->regs[rd] = (((int64_t)(cpu->regs[rs1])) < imm) ? 1 : 0;
		return true;
	}

	case FRV_INSTCODE_SLTIU: {
		uint64_t imm = FRV_INST_IMM_I(inst);
		cpu->regs[rd] = ((cpu->regs[rs1]) < imm) ? 1 : 0;
		return true;
	}

	case FRV_INSTCODE_SLT: {
		cpu->regs[rd] = (((int64_t)cpu->regs[rs1]) < ((int64_t)(cpu->regs[rs2]))) ? 1 : 0;
		return true;
	}

	case FRV_INSTCODE_SLTU: {
		cpu->regs[rd] = (cpu->regs[rs1] < cpu->regs[rs2]) ? 1 : 0;
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

	// Stores
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

	// Upper immidiate
	case FRV_INSTCODE_LUI: {
		cpu->regs[rd] = FRV_INST_IMM_U(inst);
		return true;
	}

	case FRV_INSTCODE_AUIPC: {
		cpu->regs[rd] = cpu->pc + FRV_INST_IMM_U(inst) - 4;
		return true;
	}

	// Jumps
	case FRV_INSTCODE_JAL: {
		cpu->regs[rd] = cpu->pc;
		uint64_t imm = FRV_INST_IMM_J(inst);
		cpu->pc += imm - 4;
		return true;
        }

	case FRV_INSTCODE_JALR: {
		uint64_t tmp = cpu->pc;
		uint64_t imm = FRV_INST_IMM_I(inst);
                cpu->pc = (cpu->regs[rs1] + imm) & (~1);
                cpu->regs[rd] = tmp;
		return true;
        }

	// Branch
	case FRV_INSTCODE_BEQ: {
		uint64_t imm = FRV_INST_IMM_B(inst);
		if (cpu->regs[rs1] == cpu->regs[rs2]) {
                            cpu->pc += imm; cpu->pc -= 4; 
		}
		return true;
        }
	
	case FRV_INSTCODE_BNE: {
		uint64_t imm = FRV_INST_IMM_B(inst);
		if (cpu->regs[rs1] != cpu->regs[rs2]) {
                            cpu->pc += imm; cpu->pc -= 4;
		}
		return true;
        }

	case FRV_INSTCODE_BLT: {
		uint64_t imm = FRV_INST_IMM_B(inst);
		if (((int64_t)cpu->regs[rs1]) < ((int64_t)cpu->regs[rs2])) {
                            cpu->pc += imm; cpu->pc -= 4;
		}
		return true;
        }

	case FRV_INSTCODE_BLTU: {
		uint64_t imm = FRV_INST_IMM_B(inst);
		if (cpu->regs[rs1] < cpu->regs[rs2]) {
                            cpu->pc += imm; cpu->pc -= 4;
		}
		return true;
        }

	case FRV_INSTCODE_BGE: {
		uint64_t imm = FRV_INST_IMM_B(inst);
		if (((int64_t)cpu->regs[rs1]) >= ((int64_t)cpu->regs[rs2])) {
                            cpu->pc += imm; cpu->pc -= 4;
		}
		return true;
        }

	case FRV_INSTCODE_BGEU: {
		uint64_t imm = FRV_INST_IMM_B(inst);
		if (cpu->regs[rs1] >= cpu->regs[rs2]) {
                            cpu->pc += imm; cpu->pc -= 4;
		}
		return true;
        }

	// ECALL
	case FRV_INSTCODE_ECALL: {
		return frvEcallExec(cpu->regs[FRV_ABI_REG_A0], cpu->regs[FRV_ABI_REG_A1]);
	}

	// CSRs
	case FRV_INSTCODE_CSRRW: {
		cpu->regs[rd] = frvLoadCsr(cpu, csr);
		frvStoreCsr(cpu, csr, cpu->regs[rs1]);
		return true;
	}

	case FRV_INSTCODE_CSRRS: {
		cpu->regs[rd] = frvLoadCsr(cpu, csr);
		frvStoreCsr(cpu, csr, cpu->regs[rs1] | cpu->regs[rd]);
		return true;
	}

	case FRV_INSTCODE_CSRRC: {
		cpu->regs[rd] = frvLoadCsr(cpu, csr);
		frvStoreCsr(cpu, csr, (~cpu->regs[rs1]) & cpu->regs[rd]);
		return true;
	}

	case FRV_INSTCODE_CSRRWI: {
		cpu->regs[rd] = frvLoadCsr(cpu, csr);
		frvStoreCsr(cpu, csr, rs1); // Rs1 is the same as imm here
		return true;
	}

	case FRV_INSTCODE_CSRRSI: {
		cpu->regs[rd] = frvLoadCsr(cpu, csr);
		frvStoreCsr(cpu, csr, rs1 | cpu->regs[rd]); // Rs1 is the same as imm here
		return true;
	}

	case FRV_INSTCODE_CSRRCI: {
		cpu->regs[rd] = frvLoadCsr(cpu, csr);
		frvStoreCsr(cpu, csr, (~rs1) & cpu->regs[rd]); // Rs1 is the same as imm here
		return true;
	}

	// M-extension
	case FRV_INSTCODE_MUL: {
		cpu->regs[rd] = (int64_t)cpu->regs[rs1] * (int64_t)cpu->regs[rs2];
		return true;
        }

	case FRV_INSTCODE_MULH: {
		cpu->regs[rd] = frvMulh((int64_t)cpu->regs[rs1], (int64_t)cpu->regs[rs2]);
		return true;
        }

	case FRV_INSTCODE_MULHU: {
		cpu->regs[rd] = frvMulhu(cpu->regs[rs1], cpu->regs[rs2]);
		return true;
        }

	case FRV_INSTCODE_MULHSU: {
		cpu->regs[rd] = frvMulhsu((int64_t)cpu->regs[rs1], cpu->regs[rs2]);
		return true;
        }

	case FRV_INSTCODE_MULW: {
		cpu->regs[rd] = frvMulw(cpu->regs[rs1], cpu->regs[rs2]);
		return true;
	}

	case FRV_INSTCODE_DIV: {
		cpu->regs[rd] = (cpu->regs[rs2]) ? (((int64_t)cpu->regs[rs1]) / ((int64_t)cpu->regs[rs2])) :
				-1;
		return true;
	}

	case FRV_INSTCODE_DIVU: {
		cpu->regs[rd] = (cpu->regs[rs2]) ? (cpu->regs[rs1] / cpu->regs[rs2]) : UINT64_MAX;
		return true;
	}

	case FRV_INSTCODE_DIVW: {
		cpu->regs[rd] = frvDivw(cpu->regs[rs1], cpu->regs[rs2]);
		return true;
	}

	case FRV_INSTCODE_DIVUW: {
		cpu->regs[rd] = frvDivuw(cpu->regs[rs1], cpu->regs[rs2]);
		return true;
	}

	case FRV_INSTCODE_REM: {
		cpu->regs[rd] = (cpu->regs[rs2]) ? (((int64_t)cpu->regs[rs1]) % ((int64_t)cpu->regs[rs2])) :
				cpu->regs[rs1];
		return true;
	}

	case FRV_INSTCODE_REMU: {
		cpu->regs[rd] = (cpu->regs[rs2]) ? (cpu->regs[rs1] % cpu->regs[rs2]) : cpu->regs[rs1];
		return true;
	}

	case FRV_INSTCODE_REMW: {
		cpu->regs[rd] = frvRemw(cpu->regs[rs1], cpu->regs[rs2]);
		return true;
	}

	case FRV_INSTCODE_REMUW: {
		cpu->regs[rd] = frvRemuw(cpu->regs[rs1], cpu->regs[rs2]);
		return true;
	}

	case FRV_INSTCODE_FENCE: // Not usefull on a single-threaded simulator
	case FRV_INSTCODE_FENCEI:
		return true;

	default:
		return false;
	}
}

void frvCpuRun(struct FrvCPU* cpu)
{
	while (true) {
		uint32_t inst;
		if(!frvCpuFetch(cpu, &inst)) break;
		cpu->regs[0] = 0; // always Hardwire x0 to 0
		cpu->pc += 4;
		if(!frvCpuExec(cpu, inst)) break;
		if(cpu->pc == 0) break;
	}
}
