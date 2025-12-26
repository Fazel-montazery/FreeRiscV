#include "fs.h"

int64_t readFileToBuf(const char* path, uint8_t* buf, const size_t bufsize, const bool bin)
{
	struct stat sb;
	if (stat(path, &sb) == -1) {
		fprintf(stderr, "Failed to fetch file size: %s [%s]\n", path, strerror(errno));
		return -1;
	}

	if (!buf) {
		return sb.st_size;
	}

	FILE* file = fopen(path, bin ? "rb" : "r");
	if (!file) {
		fprintf(stderr, "Failed to open file: %s [%s]\n", path, strerror(errno));
		return -1;
	}

	size_t read_len = fread(buf, 1, (bin) ? (bufsize) : (bufsize - 1), file);

	if (!bin) buf[read_len] = '\0';
	
	return read_len;
}
