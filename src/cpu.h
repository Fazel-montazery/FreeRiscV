#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "fs.h"

#include "bus.h"

#define NUM_REGS 32

// RV32I (func7 + func3 + opcode)
#define INSTCODE_ADD	((0x00 << 10) | (0x0 << 7) | 0x33) // Arithmetic
#define INSTCODE_SUB	((0x20 << 10) | (0x0 << 7) | 0x33)
#define INSTCODE_ADDI	((0x0 << 7) | 0x13)
#define INSTCODE_LB	((0x0 << 7) | 0x3) // Loads
#define INSTCODE_LH	((0x1 << 7) | 0x3)
#define INSTCODE_LW	((0x2 << 7) | 0x3)
#define INSTCODE_LBU	((0x4 << 7) | 0x3)
#define INSTCODE_LHU	((0x5 << 7) | 0x3)
#define INSTCODE_LWU	((0x6 << 7) | 0x3)
#define INSTCODE_LD	((0x3 << 7) | 0x3)
#define INSTCODE_SB	((0x0 << 7) | 0x23) // Stores
#define INSTCODE_SH	((0x1 << 7) | 0x23)
#define INSTCODE_SW	((0x2 << 7) | 0x23)
#define INSTCODE_SD	((0x3 << 7) | 0x23)

// Helpers
#define INST_OPCODE(inst) (inst & 0x7f)
#define INST_FUNCT3(inst) ((inst >> 12) & 0x7)
#define INST_FUNCT7(inst) ((inst >> 25) & 0x7f)
#define INST_RD(inst) ((size_t) ((inst >> 7) & 0x1f))
#define INST_RS1(inst) ((size_t) ((inst >> 15) & 0x1f))
#define INST_RS2(inst) ((size_t) ((inst >> 20) & 0x1f))
#define INST_IMM_I(inst) ((uint64_t) ((int64_t)((int32_t)(inst & 0xfff00000)) >> 20))
#define INST_IMM_S(inst) (((uint64_t) ((int64_t)((int32_t)(inst & 0xfe000000)) >> 20)) | ((inst >> 7) & 0x1f))

struct CPU {
	uint64_t	pc;
	uint64_t	regs[NUM_REGS];
	struct BUS*	bus;
};

struct CPU newCpu(struct BUS* bus);
void cpuPrintRegs(struct CPU* cpu); // print registers in hex and decimal
bool cpuLoadProgram(struct CPU* cpu, const char* path); // load the binary from path into memory
void cpuRun(struct CPU* cpu); // The main cpu cycle to run the program
