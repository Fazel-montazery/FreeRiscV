#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define RAM_BASE_ADDR (0x80000000)

struct RAM {
	uint8_t*	bytes;
	uint64_t	size;
};

struct RAM newRam(const size_t size);
bool isRamValid(const struct RAM* const ram);
void ramDestroy(struct RAM* ram);

// Load/Store ops
// return false on fail otherise true
// size is in bytes [1,2,4,8]
bool ramLoad(const struct RAM* const ram, uint64_t addr, const uint64_t size, uint64_t* dest);
bool ramLoadInst(struct RAM* ram, uint64_t addr, uint32_t* dest); // Load 32-bit instruction
bool ramStore(struct RAM* ram, uint64_t addr, const uint64_t size, const uint64_t val);
