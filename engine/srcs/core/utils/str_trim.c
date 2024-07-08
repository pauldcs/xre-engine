#include <ctype.h>

char *str_trim(char *str)
{
	char *last;

	while (isspace(*(unsigned char *)str)) {
		str++;
	}

	last = str;

	while (*last) {
		last++;
	}

	--last;

	while (isspace(*(unsigned char *)last)) {
		last--;
	}

	*++last = '\0';

	return (str);
}
