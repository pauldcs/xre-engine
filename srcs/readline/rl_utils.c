#include "xre_readline.h"
#include "xre_assert.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <termcap.h>

int
clamp(int v, int min, int max) {

	return (v < min)
		? min
		: ((v > max)
			? max 
			: v
		);
}

ssize_t
rl_xwrite(int fildes, const char *buf, size_t nbytes)
{
	int i;
	int ntry = 0;

	size_t limit = 0x7FFFFFFF;
  size_t n = nbytes < limit ? nbytes : limit;

	for (;;)
	{
		i = write(fildes, buf, n);
		if (i > 0)
		{
			if ((n -= i) <= 0)
				return (nbytes);
			buf += i;
			ntry = 0;
	
		} else if (i == 0)
		{
			if (++ntry > 10)
				return (nbytes - n);
	
		} else if (errno != EINTR)
			return (-1);
	}
}

ssize_t
rl_xgetchr(int fildes) {

  ssize_t nread;
  unsigned char ch;

try_read:
  nread = read(fildes, &ch, sizeof(unsigned char));
  if (nread == -1) {
    switch (errno) {
      case EINTR:  return (3 /* Ctrl-C as ascii */);
      case EAGAIN: goto try_read;
    }
    fprintf(stderr,
      "read: %s\r\n",
      strerror(errno)
    );
    return (-1);
  }
  if (nread == 0)
    return (0);
  return ((int)ch);
}

int
rl_xputchar(int c) {

	return ((int)write(STDOUT_FILENO, &c, sizeof(char)));
}


void	rl_putnbr(const int32_t c)
{
	char		nbr[20];
	uint32_t	n;
	size_t		i;

	if (c < 0)
		n = c * -1;
	else
		n = c;
	i = 20;
	if (n == 0)
		nbr[--i] = '0';
	while (n)
	{
		nbr[--i] = (n % 10) | 0x30;
		n /= 10;
	}
	if (c < 0)
		nbr[--i] = '-';
	write(1, &nbr[i], 20 - i);
}

// void
// rl_set_cxy_for_idx(int index) {
// 	__assert__(config.screencols);

// 	if (!__state__._ei) {
// 		__state__._cc.x = 0;
// 		__state__._cc.y = 0;
// 	}

// 	size_t x = index % config.screencols;
// 	size_t y = index / config.screencols;
	
// 	if (y > (size_t)__state__._cc.y) {
// 		rl_scroll_forward(1);
// 	}

// 	__state__._cc.x = x;
// 	__state__._cc.y = y;
// }

// void
// rl_set_cxy_for_step(int step) {

// 	size_t x = step
// 		+ __state__._cc.x
// 		+ __state__._cc.y
// 		* config.screencols;

// 	rl_set_cxy_for_idx(
// 		_clamp(x, 0, __state__._ei)
// 	);
// }

size_t
rl_esc_to_str(char *s)
{
	char *p, *q, c;
	int esc = 0;
	int x;

	for (p = q = s; (c = *p) != '\0'; p++) {
		if (esc) {
			switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				c -= '0';
				p++;

				if (*p >= '0' && *p <= '7') {
					c = c * 8 + *p++ - '0';

					if (*p >= '0' && *p <= '7')
						c = c * 8 + *p - '0';
					else
						p--;
				} else
					p--;

				*q++ = c;
				break;

			case 'a':
				*q++ = '\a';
				break;
			case 'b':
				*q++ = '\b';
				break;
			case 'f':
				*q++ = '\f';
				break;
			case 'n':
				*q++ = '\n';
				break;
			case 'r':
				*q++ = '\r';
				break;
			case 't':
				*q++ = '\t';
				break;
			case 'v':
				*q++ = '\v';
				break;

			case 'x':
				for (x = 0; (c = *++p) != '\0'; ) {
					if (c >= '0' && c <= '9')
						x = x * 16 + c - '0';
					else if (c >= 'a' && c <= 'f')
						x = x * 16 + c - 'a' + 10;
					else if (c >= 'A' && c <= 'F')
						x = x * 16 + c - 'A' + 10;
					else
						break;
				}
				*q++ = (char)x;
				p--;
				break;

			case '"':
			case '\\':
				*q++ = c;
				break;
			default:
				*q++ = '\\';
				*q++ = c;
			}

			esc = 0;

		} else {
			if ((esc = c == '\\') == 0)
				*q++ = c;
		}
	}

	*q = '\0';
	return ((size_t)(q - s));
}
