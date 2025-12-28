#include "bus.h"

struct FrvBUS frvNewBus(struct FrvRAM* ram)
{
	return (struct FrvBUS) {
		.ram = ram
	};
}

bool frvBusLoad(const struct FrvBUS* const bus, const uint64_t addr, const uint64_t size, uint64_t* dest)
{
	if (addr < FRV_RAM_BASE_ADDR) {
		fprintf(stderr, "FrvBUS->FrvRAM load failed: illegal access\n");
		return false;
	}
	return frvRamLoad(bus->ram, addr, size, dest);
}

bool frvBusLoadInst(const struct FrvBUS* const bus, const uint64_t addr, uint32_t* dest)
{
	if (addr < FRV_RAM_BASE_ADDR) {
		fprintf(stderr, "FrvBUS->FrvRAM load failed: illegal access\n");
		return false;
	}
	return frvRamLoadInst(bus->ram, addr, dest);
}

bool frvBusStore(const struct FrvBUS* const bus, const uint64_t addr, const uint64_t size, const uint64_t val)
{
	if (addr < FRV_RAM_BASE_ADDR) {
		fprintf(stderr, "FrvBUS->FrvRAM store failed: illegal access\n");
		return false;
	}
	return frvRamStore(bus->ram, addr, size, val);
}
