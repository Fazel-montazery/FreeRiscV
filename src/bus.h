#pragma once

#include <stdio.h>
#include <stdint.h>

#include "ram.h"

struct BUS {
	struct RAM* ram;
};

struct BUS newBus(struct RAM* ram);
bool busLoad(const struct BUS* const bus, const uint64_t addr, const uint64_t size, uint64_t* dest);
bool busLoadInst(const struct BUS* const bus, const uint64_t addr, uint32_t* dest);
bool busStore(const struct BUS* const bus, const uint64_t addr, const uint64_t size, const uint64_t val);
