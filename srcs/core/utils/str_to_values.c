#include <stdint.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

/*
 * Parses as hexadecimal if the number start with 0x or 0X
 */
const char	*
str_to_uint32(const char *str, uint32_t *result) {

	long tmp;

	tmp = 0;
    if (*str == '0'
		&& (   *(str + 1) == 'x'
		 	|| *(str + 1) == 'X'
		)
	) {
        char *endptr;
        tmp = strtol(str, &endptr, 16);
        str = endptr;
        if (errno == ERANGE
            || tmp > INT_MAX
            || *str)
        return NULL;
    } else {
        while (*str >= '0'
			&& *str <= '9')
        {
            tmp *= 10;
            tmp += *(str++) & 0xCF;
            if (tmp > INT_MAX)
                return (NULL);
        }
    }
	*result = tmp;
	return (str);
}
