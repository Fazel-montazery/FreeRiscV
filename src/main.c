#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "fs.h"

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Usage: %s [riscv binary]\n", argv[0]);
		return -1;
	}

	return 0;
}
