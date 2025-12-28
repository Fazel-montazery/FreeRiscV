#include "bus.h"

struct BUS newBus(struct RAM* ram)
{
	return (struct BUS) {
		.ram = ram
	};
}

bool busLoad(const struct BUS* const bus, const uint64_t addr, const uint64_t size, uint64_t* dest)
{
	if (addr < RAM_BASE_ADDR) {
		fprintf(stderr, "BUS->RAM load failed: illegal access\n");
		return false;
	}
	return ramLoad(bus->ram, addr, size, dest);
}

bool busLoadInst(const struct BUS* const bus, const uint64_t addr, uint32_t* dest)
{
	if (addr < RAM_BASE_ADDR) {
		fprintf(stderr, "BUS->RAM load failed: illegal access\n");
		return false;
	}
	return ramLoadInst(bus->ram, addr, dest);
}

bool busStore(const struct BUS* const bus, const uint64_t addr, const uint64_t size, const uint64_t val)
{
	if (addr < RAM_BASE_ADDR) {
		fprintf(stderr, "BUS->RAM store failed: illegal access\n");
		return false;
	}
	return ramStore(bus->ram, addr, size, val);
}
