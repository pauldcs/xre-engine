#include <stddef.h>
#include <stdint.h>

static inline size_t
memdcmp(const void *a, const void *b, size_t size_a, size_t size_b)
{
	register size_t dist = 0;

	while (size_a-- && size_b--) {
		dist += __builtin_popcount(
			*(char *)a++ ^ *(char *)b++
		);
	}

	return (dist + ((size_a - size_b) << 3));
}
