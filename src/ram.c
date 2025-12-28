#include "ram.h"

struct FrvRAM frvNewRam(const uint64_t size)
{
	uint8_t* bytes = malloc(sizeof(uint8_t) * size);
	if (!bytes) fprintf(stderr, "Failed to create a new FrvRAM with size %lu: %s\n", size, strerror(errno));
	return (struct FrvRAM) {
		.bytes = bytes,
		.size = size
	};
}

bool frvIsRamValid(const struct FrvRAM* const ram)
{
	return (ram->bytes != NULL);
}

void frvRamDestroy(struct FrvRAM* ram)
{
	free(ram->bytes);
}

static inline uint64_t frvRamLoad8(const struct FrvRAM* const ram, const uint64_t addr)
{
	return ram->bytes[addr];
}

static inline uint64_t frvRamLoad16(const struct FrvRAM* const ram, const uint64_t addr)
{
	return ((uint64_t)(ram->bytes[addr])) | 
	       (((uint64_t)(ram->bytes[addr + 1])) << 8);
}

static inline uint64_t frvRamLoad32(const struct FrvRAM* const ram, const uint64_t addr)
{
	return ((uint64_t)(ram->bytes[addr]))		  | 
	       (((uint64_t)(ram->bytes[addr + 1])) << 8)  |
	       (((uint64_t)(ram->bytes[addr + 2])) << 16) |
	       (((uint64_t)(ram->bytes[addr + 3])) << 24);
}

static inline uint64_t frvRamLoad64(const struct FrvRAM* const ram, const uint64_t addr)
{
	return ((uint64_t)(ram->bytes[addr]))		  | 
	       (((uint64_t)(ram->bytes[addr + 1])) << 8)  |
	       (((uint64_t)(ram->bytes[addr + 2])) << 16) |
	       (((uint64_t)(ram->bytes[addr + 3])) << 24) |
	       (((uint64_t)(ram->bytes[addr + 4])) << 32) |
	       (((uint64_t)(ram->bytes[addr + 5])) << 40) |
	       (((uint64_t)(ram->bytes[addr + 6])) << 48) |
	       (((uint64_t)(ram->bytes[addr + 7])) << 56);
}

static inline void frvRamStore8(struct FrvRAM* ram, const uint64_t addr, const uint64_t val)
{
	ram->bytes[addr] = val & 0xFF;
}

static inline void frvRamStore16(struct FrvRAM* ram, const uint64_t addr, const uint64_t val)
{
	ram->bytes[addr] = (uint8_t)(val & 0xFF);
	ram->bytes[addr + 1] = (uint8_t)((val >> 8) & 0xFF);
}

static inline void frvRamStore32(struct FrvRAM* ram, const uint64_t addr, const uint64_t val)
{
	ram->bytes[addr] = (uint8_t)(val & 0xFF);
	ram->bytes[addr + 1] = (uint8_t)((val >> 8) & 0xFF);
	ram->bytes[addr + 2] = (uint8_t)((val >> 16) & 0xFF);
	ram->bytes[addr + 3] = (uint8_t)((val >> 24) & 0xFF);
}

static inline void frvRamStore64(struct FrvRAM* ram, const uint64_t addr, const uint64_t val)
{
	ram->bytes[addr] = (uint8_t)(val & 0xFF);
	ram->bytes[addr + 1] = (uint8_t)((val >> 8) & 0xFF);
	ram->bytes[addr + 2] = (uint8_t)((val >> 16) & 0xFF);
	ram->bytes[addr + 3] = (uint8_t)((val >> 24) & 0xFF);
	ram->bytes[addr + 4] = (uint8_t)((val >> 32) & 0xFF);
	ram->bytes[addr + 5] = (uint8_t)((val >> 40) & 0xFF);
	ram->bytes[addr + 6] = (uint8_t)((val >> 48) & 0xFF);
	ram->bytes[addr + 7] = (uint8_t)((val >> 56) & 0xFF);
}

bool frvRamLoad(const struct FrvRAM* const ram, uint64_t addr, const uint64_t size, uint64_t* dest)
{
	addr -= FRV_RAM_BASE_ADDR;

	if (addr + size > ram->size) {
		fprintf(stderr, "Ram load failed: Out of range Address\n");
		return false;
	}

	switch (size) {
		case 1:
			*dest = frvRamLoad8(ram, addr);
			break;
		case 2:
			*dest = frvRamLoad16(ram, addr);
			break;
		case 4:
			*dest = frvRamLoad32(ram, addr);
			break;
		case 8:
			*dest = frvRamLoad64(ram, addr);
			break;

		default:
			fprintf(stderr, "Ram load failed: Invalid fetch size\n");
			return false;
	}
	return true;
}

bool frvRamStore(struct FrvRAM* ram, uint64_t addr, const uint64_t size, const uint64_t val)
{
	addr -= FRV_RAM_BASE_ADDR;

	if (addr + size > ram->size) {
		fprintf(stderr, "Ram store failed: Out of range Address\n");
		return false;
	}

	switch (size) {
		case 1:
			frvRamStore8(ram, addr, val);
			break;
		case 2:
			frvRamStore16(ram, addr, val);
			break;
		case 4:
			frvRamStore32(ram, addr, val);
			break;
		case 8:
			frvRamStore64(ram, addr, val);
			break;

		default:
			fprintf(stderr, "Ram store failed: Invalid fetch size\n");
			return false;
	}
	return true;
}

bool frvRamLoadInst(struct FrvRAM* ram, uint64_t addr, uint32_t* dest)
{
	addr -= FRV_RAM_BASE_ADDR;

	if (addr + 4 > ram->size) {
		fprintf(stderr, "Ram Load instruction failed: Out of range Address\n");
		return false;
	}

	*dest = ((uint32_t)(ram->bytes[addr]))		   | 
	        (((uint32_t)(ram->bytes[addr + 1])) << 8)  |
	        (((uint32_t)(ram->bytes[addr + 2])) << 16) |
	        (((uint32_t)(ram->bytes[addr + 3])) << 24);

	return true;
}
