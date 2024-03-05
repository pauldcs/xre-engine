#include <stddef.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

#define RCAST(T, b)	((T)(uintptr_t)(b))
#define CAST(T, b)	((T)(b))

char *
str2print(char *buf, size_t bufsiz, const char *str) {

	char *ptr, *eptr = buf + bufsiz - 1;
	const uint8_t *s = RCAST(const uint8_t *, str);
	const uint8_t *es = s + strlen(str);

	for (ptr = buf;  ptr < eptr && s < es && *s; s++) {
		if (isprint(*s)) {
			*ptr++ = *s;
			continue;
		}
		if (ptr >= eptr - 3)
			break;
		*ptr++ = '\\';
		*ptr++ = ((CAST(uint32_t, *s) >> 6) & 7) + '0';
		*ptr++ = ((CAST(uint32_t, *s) >> 3) & 7) + '0';
		*ptr++ = ((CAST(uint32_t, *s) >> 0) & 7) + '0';
	}
	*ptr = '\0';
	return buf;
}
