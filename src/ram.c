#include "ram.h"

struct RAM newRam(const uint64_t size)
{
	uint8_t* bytes = malloc(sizeof(uint8_t) * size);
	if (!bytes) fprintf(stderr, "Failed to create a new RAM with size %lu: %s\n", size, strerror(errno));
	return (struct RAM) {
		.bytes = bytes,
		.size = size
	};
}

bool isRamValid(const struct RAM* const ram)
{
	return (ram->bytes != NULL);
}

void ramDestroy(struct RAM* ram)
{
	free(ram->bytes);
}

static inline uint64_t ramLoad8(const struct RAM* const ram, const uint64_t addr)
{
	return ram->bytes[addr];
}

static inline uint64_t ramLoad16(const struct RAM* const ram, const uint64_t addr)
{
	return ((uint64_t)(ram->bytes[addr])) | 
	       (((uint64_t)(ram->bytes[addr + 1])) << 8);
}

static inline uint64_t ramLoad32(const struct RAM* const ram, const uint64_t addr)
{
	return ((uint64_t)(ram->bytes[addr]))		  | 
	       (((uint64_t)(ram->bytes[addr + 1])) << 8)  |
	       (((uint64_t)(ram->bytes[addr + 2])) << 16) |
	       (((uint64_t)(ram->bytes[addr + 3])) << 24);
}

static inline uint64_t ramLoad64(const struct RAM* const ram, const uint64_t addr)
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

static inline void ramStore8(struct RAM* ram, const uint64_t addr, const uint64_t val)
{
	ram->bytes[addr] = val & 0xFF;
}

static inline void ramStore16(struct RAM* ram, const uint64_t addr, const uint64_t val)
{
	ram->bytes[addr] = (uint8_t)(val & 0xFF);
	ram->bytes[addr + 1] = (uint8_t)((val >> 8) & 0xFF);
}

static inline void ramStore32(struct RAM* ram, const uint64_t addr, const uint64_t val)
{
	ram->bytes[addr] = (uint8_t)(val & 0xFF);
	ram->bytes[addr + 1] = (uint8_t)((val >> 8) & 0xFF);
	ram->bytes[addr + 2] = (uint8_t)((val >> 16) & 0xFF);
	ram->bytes[addr + 3] = (uint8_t)((val >> 24) & 0xFF);
}

static inline void ramStore64(struct RAM* ram, const uint64_t addr, const uint64_t val)
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

bool ramLoad(const struct RAM* const ram, uint64_t addr, const uint64_t size, uint64_t* dest)
{
	addr -= RAM_BASE_ADDR;

	if (addr + size > ram->size) {
		fprintf(stderr, "Ram load failed: Out of range Address\n");
		return false;
	}

	switch (size) {
		case 1:
			*dest = ramLoad8(ram, addr);
			break;
		case 2:
			*dest = ramLoad16(ram, addr);
			break;
		case 4:
			*dest = ramLoad32(ram, addr);
			break;
		case 8:
			*dest = ramLoad64(ram, addr);
			break;

		default:
			fprintf(stderr, "Ram load failed: Invalid fetch size\n");
			return false;
	}
	return true;
}

bool ramStore(struct RAM* ram, uint64_t addr, const uint64_t size, const uint64_t val)
{
	addr -= RAM_BASE_ADDR;

	if (addr + size > ram->size) {
		fprintf(stderr, "Ram store failed: Out of range Address\n");
		return false;
	}

	switch (size) {
		case 1:
			ramStore8(ram, addr, val);
			break;
		case 2:
			ramStore16(ram, addr, val);
			break;
		case 4:
			ramStore32(ram, addr, val);
			break;
		case 8:
			ramStore64(ram, addr, val);
			break;

		default:
			fprintf(stderr, "Ram store failed: Invalid fetch size\n");
			return false;
	}
	return true;
}

bool ramLoadInst(struct RAM* ram, uint64_t addr, uint32_t* dest)
{
	addr -= RAM_BASE_ADDR;

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
