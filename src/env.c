#include "env.h" 

bool frvEcallExec(struct FrvCPU* cpu)
{
	const enum FrvEcall ecall = cpu->regs[FRV_ABI_REG_A0];
	const uint64_t in = cpu->regs[FRV_ABI_REG_A1];
	switch (ecall) {
		case FRV_ECALL_PRINT_D: {
			printf("%ld", (int64_t)in);
			return true;
		}

		case FRV_ECALL_PRINT_S: {
			size_t i = 0;
			uint64_t c;
			while (true) {
				if (!frvBusLoad(cpu->bus, in + i, 1, &c)) return false;
				if (c != '\0') putchar((char)c);
				else break;
				i++;
			}
			return true;
		}

		case FRV_ECALL_PRINT_C: {
			putchar((char)in);
			return true;
		}

		case FRV_ECALL_PRINT_X: {
			printf("%lX", in);
			return true;
		}

		case FRV_ECALL_SCAN_S: {
			const uint64_t bufsiz = cpu->regs[FRV_ABI_REG_A2];
			int c;
			size_t i = 0;
			while ((c = getchar()) != EOF && c != '\n' && i + 1 < bufsiz) {
				if (!frvBusStore(cpu->bus, in + i, 1, (uint64_t) c)) return false;
				i++;
			}
			if (!frvBusStore(cpu->bus, in + i, 1, 0)) return false;
			if (c != '\n' && c != EOF)
				while ((c = getchar()) != EOF && c != '\n') {}
			return true;
	       }

		case FRV_ECALL_END: {
			return false;
		}

		default: {
			printf("Invalid Ecall: 0x%X\n", ecall);
			return false;
		}
	}
}
