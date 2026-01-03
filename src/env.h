#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

enum FrvEcall {
	FRV_ECALL_PRINT_D = 0,	// Print the number at a1 as a decimal
	FRV_ECALL_PRINT_S,	// Print the string pointed by a1
	FRV_ECALL_PRINT_C,	// Print the number at a1 as an ASCII Char
	FRV_ECALL_END		// Terminate the program
};

// Do the ecalls
bool frvEcallExec(const enum FrvEcall ecall, const uint64_t in);
