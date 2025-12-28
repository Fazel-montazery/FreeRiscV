#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define FRV_RAM_BASE_ADDR (0x80000000)

struct FrvRAM {
	uint8_t*	bytes;
	uint64_t	size;
};

struct FrvRAM frvNewRam(const size_t size);
bool frvIsRamValid(const struct FrvRAM* const ram);
void frvRamDestroy(struct FrvRAM* ram);

// Load/Store ops
// return false on fail otherise true
// size is in bytes [1,2,4,8]
bool frvRamLoad(const struct FrvRAM* const ram, uint64_t addr, const uint64_t size, uint64_t* dest);
bool frvRamLoadInst(struct FrvRAM* ram, uint64_t addr, uint32_t* dest); // Load 32-bit instruction
bool frvRamStore(struct FrvRAM* ram, uint64_t addr, const uint64_t size, const uint64_t val);
