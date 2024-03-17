#include "xre_utils.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <unistd.h>

char *str_to_int32(const char *str, int32_t *result)
{
	int64_t tmp;
	char *ptr = (char *)str;
	int negative = 0;

	tmp = 0;

	if (*ptr == '-') {
		negative++;
		ptr++;
	}

	if (*ptr < '0' || *ptr > '9')
		return ((char *)str);

	if (*ptr == '0') {
		char *endptr;
		int base;

		if (*(ptr + 1) == 'x' || *(ptr + 1) == 'X')
			base = 16;

		else if (*(ptr + 1) == 'b' || *(ptr + 1) == 'B') {
			base = 2;
			ptr += 2;
		}

		else
			base = 8;

		tmp = strtol(ptr, &endptr, base);
		ptr = endptr;

		if (errno == ERANGE || tmp > ((int64_t)INT_MAX + negative)) {
			return (NULL);
		}

	} else {
		while (*ptr >= '0' && *ptr <= '9') {
			tmp *= 10;
			tmp += *(ptr++) & 0xcf;

			if (tmp > ((int64_t)INT_MAX + negative)) {
				return (NULL);
			}
		}
	}

	if (negative)
		tmp = -tmp;

	*result = tmp;

	return ((char *)ptr);
}
