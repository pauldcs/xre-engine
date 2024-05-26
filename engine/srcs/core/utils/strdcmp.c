#include <stddef.h>
#include <stdint.h>
#include <string.h>

size_t strdcmp(const char *s1, const char *s2)
{
	size_t dist = 0;

	while (*s1 && *s2)
		dist += __builtin_popcount(*s1++ ^ *s2++);

	return (dist + ((strlen(s1) - strlen(s2)) << 3));
}
