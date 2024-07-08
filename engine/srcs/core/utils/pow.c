#include <stdint.h>

uint64_t int_pow(uint64_t base, unsigned int exp)
{
	uint64_t result = 1;

	while (exp) {
		if (exp & 1) {
			result *= base;
		}
		exp >>= 1;
		base *= base;
	}

	return (result);
}
