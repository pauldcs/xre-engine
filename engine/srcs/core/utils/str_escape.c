#include "xre_assert.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

static void str_byte_escape(const char *p, char **dst)
{
	__return_if_fail__(p && dst);
	char *q = *dst;
	switch (*p) {
	case '\n':
		*q++ = '\\';
		*q++ = 'n';
		break;
	case '\r':
		*q++ = '\\';
		*q++ = 'r';
		break;
	case '\\':
		*q++ = '\\';
		break;
	case '\t':
		*q++ = '\\';
		*q++ = 't';
		break;
	case '"':
		*q++ = '\\';
		*q++ = '"';
		break;
	case '\f':
		*q++ = '\\';
		*q++ = 'f';
		break;
	case '\b':
		*q++ = '\\';
		*q++ = 'b';
		break;
	case '\v':
		*q++ = '\\';
		*q++ = 'v';
		break;
	case '\a':
		*q++ = '\\';
		*q++ = 'a';
		break;
	default:

		if (!isprint(*p)) {
			*q++ = '\\';
			*q++ = 'x';
			*q++ = "0123456789abcdef"[*p >> 4 & 0xf];
			*q++ = "0123456789abcdef"[*p & 0xf];
		} else {
			*q++ = *p;
		}
	}
	*dst = q;
}

char *str_escape(const uint8_t *buf, int sz)
{
	__return_val_if_fail__(buf, NULL);

	char *new_buf = malloc(1 + sz * 4);
	if (!new_buf) {
		return NULL;
	}

	char *q = new_buf;
	int i;

	for (i = 0; i < sz; i++) {
		str_byte_escape((char *)&buf[i], &q);
	}
	*q = '\0';
	return new_buf;
}
