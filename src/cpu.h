#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "fs.h"

#include "bus.h"

#define FRV_NUM_REGS 32
#define FRV_NUM_CSRS 4096

// RV32I (func7 + func3 + opcode)
#define FRV_INSTCODE_ADD	((0x00 << 10) | (0x0 << 7) | 0x33) // Arithmetic
#define FRV_INSTCODE_SUB	((0x20 << 10) | (0x0 << 7) | 0x33)
#define FRV_INSTCODE_ADDI	((0x0 << 7) | 0x13)
#define FRV_INSTCODE_ADDIW	((0x0 << 7) | 0x1b)
#define FRV_INSTCODE_ADDW	((0x0 << 10) | (0x0 << 7) | 0x3b)
#define FRV_INSTCODE_SUBW	((0x20 << 10) | (0x0 << 7) | 0x3b)
#define FRV_INSTCODE_ANDI	((0x7 << 7) | 0x13) // Logic
#define FRV_INSTCODE_ORI	((0x6 << 7) | 0x13)
#define FRV_INSTCODE_XORI	((0x4 << 7) | 0x13)
#define FRV_INSTCODE_SLLI	((0x0 << 10) | (0x1 << 7) | 0x13)
#define FRV_INSTCODE_SRLI	((0x0 << 10) | (0x5 << 7) | 0x13)
#define FRV_INSTCODE_SRAI	((0x10 << 10) | (0x5 << 7) | 0x13)
#define FRV_INSTCODE_AND	((0x0 << 10) | (0x7 << 7) | 0x33)
#define FRV_INSTCODE_OR		((0x0 << 10) | (0x6 << 7) | 0x33)
#define FRV_INSTCODE_XOR	((0x0 << 10) | (0x4 << 7) | 0x33)
#define FRV_INSTCODE_SLL	((0x0 << 10) | (0x1 << 7) | 0x33)
#define FRV_INSTCODE_SRL	((0x0 << 10) | (0x5 << 7) | 0x33)
#define FRV_INSTCODE_SRA	((0x20 << 10) | (0x5 << 7) | 0x33)
#define FRV_INSTCODE_SLLIW	((0x00 << 10) | (0x1 << 7) | 0x1b)
#define FRV_INSTCODE_SRLIW	((0x00 << 10) | (0x5 << 7) | 0x1b)
#define FRV_INSTCODE_SRAIW	((0x20 << 10) | (0x5 << 7) | 0x1b)
#define FRV_INSTCODE_SLLW	((0x00 << 10) | (0x1 << 7) | 0x3b)
#define FRV_INSTCODE_SRLW	((0x00 << 10) | (0x5 << 7) | 0x3b)
#define FRV_INSTCODE_SRAW	((0x20 << 10) | (0x5 << 7) | 0x3b)
#define FRV_INSTCODE_SLTI	((0x2 << 7) | 0x13) // Compares
#define FRV_INSTCODE_SLTIU	((0x3 << 7) | 0x13)
#define FRV_INSTCODE_SLT	((0x0 << 10) | (0x2 << 7) | 0x33)
#define FRV_INSTCODE_SLTU	((0x0 << 10) | (0x3 << 7) | 0x33)
#define FRV_INSTCODE_LB		((0x0 << 7) | 0x3) // Loads
#define FRV_INSTCODE_LH		((0x1 << 7) | 0x3)
#define FRV_INSTCODE_LW		((0x2 << 7) | 0x3)
#define FRV_INSTCODE_LBU	((0x4 << 7) | 0x3)
#define FRV_INSTCODE_LHU	((0x5 << 7) | 0x3)
#define FRV_INSTCODE_LWU	((0x6 << 7) | 0x3)
#define FRV_INSTCODE_LD		((0x3 << 7) | 0x3)
#define FRV_INSTCODE_SB		((0x0 << 7) | 0x23) // Stores
#define FRV_INSTCODE_SH		((0x1 << 7) | 0x23)
#define FRV_INSTCODE_SW		((0x2 << 7) | 0x23)
#define FRV_INSTCODE_SD		((0x3 << 7) | 0x23)
#define FRV_INSTCODE_LUI	(0x37) // Upper immidiate
#define FRV_INSTCODE_AUIPC	(0x17)
#define FRV_INSTCODE_JAL	(0x6f) // Jumps
#define FRV_INSTCODE_JALR	((0x0 << 7) | 0x67)
#define FRV_INSTCODE_BEQ	((0x0 << 7) | 0x63) // Branch
#define FRV_INSTCODE_BNE	((0x1 << 7) | 0x63)
#define FRV_INSTCODE_BLT	((0x4 << 7) | 0x63)
#define FRV_INSTCODE_BLTU	((0x6 << 7) | 0x63)
#define FRV_INSTCODE_BGE	((0x5 << 7) | 0x63)
#define FRV_INSTCODE_BGEU	((0x7 << 7) | 0x63)
#define FRV_INSTCODE_CSRRW	((0x1 << 7) | 0x73) // Csrs
#define FRV_INSTCODE_CSRRS	((0x2 << 7) | 0x73)
#define FRV_INSTCODE_CSRRC	((0x3 << 7) | 0x73)
#define FRV_INSTCODE_CSRRWI	((0x5 << 7) | 0x73)
#define FRV_INSTCODE_CSRRSI	((0x6 << 7) | 0x73)
#define FRV_INSTCODE_CSRRCI	((0x7 << 7) | 0x73)

// Machine-level CSRs
/// Hardware thread ID
#define FRV_CSR_MHARTID (0xf14)
/// Machine status register
#define FRV_CSR_MSTATUS (0x300)
/// Machine exception delefation register
#define FRV_CSR_MEDELEG (0x302)
/// Machine interrupt delefation register
#define FRV_CSR_MIDELEG (0x303)
/// Machine interrupt-enable register
#define FRV_CSR_MIE (0x304)
/// Machine trap-handler base address
#define FRV_CSR_MTVEC (0x305)
/// Machine counter enable
#define FRV_CSR_MCOUNTEREN (0x306)
/// Scratch register for machine trap handlers
#define FRV_CSR_MSCRATCH (0x340)
/// Machine exception program counter
#define FRV_CSR_MEPC (0x341)
/// Machine trap cause
#define FRV_CSR_MCAUSE (0x342)
/// Machine bad address or instruction
#define FRV_CSR_MTVAL (0x343)
/// Machine interrupt pending
#define FRV_CSR_MIP (0x344)

// Supervisor-level CSRs
/// Supervisor status register
#define FRV_CSR_SSTATUS (0x100)
/// Supervisor interrupt-enable register
#define FRV_CSR_SIE (0x104)
/// Supervisor trap handler base address
#define FRV_CSR_STVEC (0x105)
/// Scratch register for supervisor trap handlers
#define FRV_CSR_SSCRATCH (0x140)
/// Supervisor exception program counter
#define FRV_CSR_SEPC (0x141)
/// Supervisor trap cause
#define FRV_CSR_SCAUSE (0x142)
/// Supervisor bad address or instruction
#define FRV_CSR_STVAL (0x143)
/// Supervisor interrupt pending
#define FRV_CSR_SIP (0x144)
/// Supervisor address translation and protection
#define FRV_CSR_SATP (0x180)

// Helpers
#define FRV_INST_OPCODE(inst) (inst & 0x7f)
#define FRV_INST_CSR_CODE(inst) ((inst >> 20) & 0xfff)
#define FRV_INST_FUNCT3(inst) ((inst >> 12) & 0x7)
#define FRV_INST_FUNCT7(inst) ((inst >> 25) & 0x7f)
#define FRV_INST_FUNCT6(inst) ((inst >> 26) & 0x3f)
#define FRV_INST_RD(inst) ((size_t) ((inst >> 7) & 0x1f))
#define FRV_INST_RS1(inst) ((size_t) ((inst >> 15) & 0x1f))
#define FRV_INST_RS2(inst) ((size_t) ((inst >> 20) & 0x1f))
#define FRV_INST_IMM_I(inst) ((uint64_t) ((int64_t)((int32_t)(inst & 0xfff00000)) >> 20))
#define FRV_INST_IMM_S(inst) (((uint64_t) ((int64_t)((int32_t)(inst & 0xfe000000)) >> 20)) | ((inst >> 7) & 0x1f))
#define FRV_INST_IMM_U(inst) ((uint64_t) ((int64_t)((int32_t)(inst & 0xfffff000))))
#define FRV_INST_IMM_J(inst) ((uint64_t)(((int64_t)((int32_t)(inst & 0x80000000))) >> 11)) \
							   | (inst & 0xff000)		   \
							   | ((inst >> 9) & 0x800)	   \
							   | ((inst >> 20) & 0x7fe)
#define FRV_INST_IMM_B(inst) ((uint64_t)(((int64_t)((int32_t)(inst & 0x80000000))) >> 19)) \
							   | ((inst & 0x80) << 4)	   \
							   | ((inst >> 20) & 0x7e0)	   \
							   | ((inst >> 7) & 0x1e)
#define FRV_INST_IMM_CSR(inst) ((uint64_t)((inst >> 15) & 0x1f))
#define FRV_INST_SHAMT64(inst) ((inst >> 20) & 0x3f)
#define FRV_INST_SHAMT32(inst) ((inst >> 20) & 0x1f)

struct FrvCPU {
	uint64_t	pc;
	uint64_t	regs[FRV_NUM_REGS];
	uint64_t	csrs[FRV_NUM_CSRS];
	struct FrvBUS*	bus;
};

struct FrvCPU frvNewCpu(struct FrvBUS* bus);
void frvCpuPrintRegs(const struct FrvCPU* const cpu); // print regs
void frvCpuPrintCsrs(const struct FrvCPU* const cpu); // print some of the csrs
bool frvCpuLoadProgram(struct FrvCPU* cpu, const char* path); // load the binary from path into memory
void frvCpuRun(struct FrvCPU* cpu); // The main frvCpu cycle to run the program
