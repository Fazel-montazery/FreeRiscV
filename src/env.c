#include "env.h" 

bool frvEcallExec(const enum FrvEcall ecall, const uint64_t in)
{
	switch (ecall) {
		case FRV_ECALL_PRINT_D:
			printf("%ld\n", (int64_t)in);
			return true;

		case FRV_ECALL_PRINT_S:
			puts((char*)in);
			return true;

		case FRV_ECALL_PRINT_C:
			putchar((char)in);
			putchar('\n');
			return true;

		case FRV_ECALL_END:
			return false;

		default:
			printf("Invalid Ecall: 0x%X\n", ecall);
			return false;
	}
}
