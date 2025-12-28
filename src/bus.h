#pragma once

#include <stdio.h>
#include <stdint.h>

#include "ram.h"

struct FrvBUS {
	struct FrvRAM* ram;
};

struct FrvBUS frvNewBus(struct FrvRAM* ram);
bool frvBusLoad(const struct FrvBUS* const bus, const uint64_t addr, const uint64_t size, uint64_t* dest);
bool frvBusLoadInst(const struct FrvBUS* const bus, const uint64_t addr, uint32_t* dest);
bool frvBusStore(const struct FrvBUS* const bus, const uint64_t addr, const uint64_t size, const uint64_t val);
