#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "cpu.h"

enum FrvEcall {
	FRV_ECALL_PRINT_D = 0,	// Print the number at a1 as a decimal
	FRV_ECALL_PRINT_S = 1,	// Print the string pointed by a1
	FRV_ECALL_PRINT_C = 2,	// Print the number at a1 as an ASCII Char
	FRV_ECALL_PRINT_X = 3,	// Print the number at a1 as a hex 
	FRV_ECALL_SCAN_D =  4,	// Scan the number from stdin and write to a1
	FRV_ECALL_SCAN_S =  5,	// Scan the string from stdin and write the memory pointed by a1
	FRV_ECALL_SCAN_C =  6,	// Scan the char from stdin and write to a1
	FRV_ECALL_END =     7	// Terminate the program
};

// Do the ecalls
bool frvEcallExec(struct FrvCPU* cpu);
