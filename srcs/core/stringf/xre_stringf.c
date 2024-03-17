#include "xre_stringf.h"
#include "xre_utils.h"
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void iob_write(t_iobuf *iob, const char *src, size_t n)
{
	size_t i;

	i = 0;
	while (n && iob->len + i < iob->cap) {
		*(unsigned char *)((char *)iob->data + iob->len + i) = src[i];
		i++;
		n--;
	}

	iob->len += i;
	iob->disc += n;
}

static void __chr(t_iobuf *iob, const int c)
{
	char buf[3];

	if (c >= 32 && c <= 126) {
		iob_write(iob, (char *)&c, 1);

	} else {
		buf[2] = "0123456789abcdef"[c & 0xf];
		buf[1] = "0123456789abcdef"[(c >> 4) & 0xf];
		buf[0] = '\\';
		iob_write(iob, buf, 3);
	}
}

static void __hex(t_iobuf *iob, const uint32_t n)
{
	uint64_t tmp;
	char bytes[16];
	size_t i;

	i = 16;
	if (n == 0) {
		bytes[--i] = '0';

	} else {
		tmp = n;

		while (i && tmp) {
			bytes[--i] = "0123456789abcdef"[tmp & 0xf];
			tmp >>= 4;
		}
	}
	iob_write(iob, &bytes[i], 16 - i);
}

static void __int(t_iobuf *iob, const int32_t c)
{
	char nbr[20];
	uint32_t n;
	size_t i;

	if (c < 0) {
		n = c * -1;

	} else {
		n = c;
	}

	i = 20;

	if (n == 0)
		nbr[--i] = '0';

	while (n) {
		nbr[--i] = (n % 10) | 0x30;
		n /= 10;
	}

	if (c < 0)
		nbr[--i] = '-';

	iob_write(iob, &nbr[i], 20 - i);
}

static void __ptr(t_iobuf *iob, const uint64_t *p)
{
	uint64_t ptr;
	char hex[18];
	size_t i;

	i = 18;

	if (p == NULL)
		hex[--i] = '0';

	ptr = (uint64_t)p;

	while (ptr && i) {
		hex[--i] = "0123456789abcdef"[ptr & 0xf];
		ptr >>= 4;
	}

	hex[--i] = 'x';
	hex[--i] = '0';

	iob_write(iob, &hex[i], 18 - i);
}

static void __str(t_iobuf *iob, const char *s)
{
	if (s) {
		iob_write(iob, s, strlen(s));

	} else {
		iob_write(iob, "(null)", 6);
	}
}

static char *format_field(t_iobuf *iob, char *ptr, va_list *ap)
{
	char *tmp = ptr;
	int ok = 0;

	if (*tmp == 'd' && ++tmp && ++ok)
		__int(iob, va_arg(*ap, int32_t));
	else if (*tmp == 's' && ++tmp && ++ok)
		__str(iob, va_arg(*ap, char *));
	else if (*tmp == 'c' && ++tmp && ++ok)
		__chr(iob, va_arg(*ap, int));
	else if (*tmp == 'x' && ++tmp && ++ok)
		__hex(iob, va_arg(*ap, uint32_t));
	else if (*tmp == 'p' && ++tmp && ++ok)
		__ptr(iob, va_arg(*ap, uint64_t *));

	if (ok)
		return (tmp);
	return (iob_write(iob, "%", 1), ptr);
}

static void iob_format_str(t_iobuf *iob, const char *format, va_list *ap)
{
	char *ptr;

	ptr = (char *)format;

	while (*ptr) {
		while (*ptr && *ptr != '%')
			iob_write(iob, ptr++, 1);

		if (!*ptr++)
			break;

		iob->width = 0;

		ptr = format_field(iob, ptr, ap);
	}
}

size_t cpyf(void *dst, size_t dstsize, const char *format, ...)
{
	va_list ap;
	t_iobuf iob;

	(void)xmemset(&iob, '\0', sizeof(t_iobuf));

	iob.data = dst;
	iob.cap = dstsize;

	va_start(ap, format);
	iob_format_str(&iob, format, &ap);
	va_end(ap);

	return (iob.len + iob.disc);
}

size_t scpyf(char *str, const char *format, ...)
{
	va_list ap;
	t_iobuf iob;

	(void)xmemset(&iob, '\0', sizeof(t_iobuf));

	iob.data = str;
	iob.cap = IOBUF_MAX - 1;

	va_start(ap, format);
	iob_format_str(&iob, format, &ap);
	va_end(ap);

	*(char *)((char *)iob.data + iob.len) = '\0';
	return (iob.len + iob.disc);
}

size_t slcpyf(char *dst, size_t dstsize, const char *format, ...)
{
	va_list ap;
	t_iobuf iob;

	xmemset(&iob, '\0', sizeof(t_iobuf));

	iob.data = dst;
	iob.cap = dstsize;

	va_start(ap, format);
	iob_format_str(&iob, format, &ap);
	va_end(ap);

	if (dstsize)
		*(char *)((char *)iob.data + iob.len - (iob.len == dstsize)) =
			'\0';
	return (iob.len + iob.disc);
}

ssize_t ssavef(char **dst, const char *format, ...)
{
	static char buf[IOBUF_MAX];
	va_list ap;
	t_iobuf iob;

	(void)xmemset(&iob, '\0', sizeof(t_iobuf));

	iob.data = buf;
	iob.cap = sizeof(buf) - 1;

	va_start(ap, format);
	iob_format_str(&iob, format, &ap);
	va_end(ap);

	*dst = malloc(iob.len + 1);

	if (*dst) {
		(void)xmemcpy(*dst, iob.data, iob.len);
		(*dst)[iob.len] = '\0';
		return (iob.len + iob.disc);
	}
	return (-1);
}

size_t fputstr(int fd, const char *format, ...)
{
	static char buf[IOBUF_MAX];
	va_list ap;
	t_iobuf iob;

	xmemset(&iob, '\0', sizeof(t_iobuf));

	iob.data = buf;
	iob.cap = sizeof(buf);

	va_start(ap, format);
	iob_format_str(&iob, format, &ap);
	va_end(ap);

	return (xwrite(fd, iob.data, iob.len));
}
