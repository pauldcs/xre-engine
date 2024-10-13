#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define IS_OCTAL(x) ((x) >= '0' && (x) <= '7')
#define IS_DIGIT(x) ((x) >= '0' && (x) <= '9')

static bool hex_to_byte(uint8_t *val, uint8_t c)
{
	if (IS_DIGIT(c)) {
		*val = (uint8_t)(*val) * 16 + (c - '0');

	} else if (c >= 'A' && c <= 'F') {
		*val = (uint8_t)(*val) * 16 + (c - 'A' + 10);

	} else if (c >= 'a' && c <= 'f') {
		*val = (uint8_t)(*val) * 16 + (c - 'a' + 10);

	} else {
		return (true);
	}
	return (false);
}

int str_unescape(char *buf)
{
	unsigned char ch = 0, ch2 = 0;
	int	      err = 0;
	int	      i;

	for (i = 0; buf[i]; i++) {
		if (buf[i] != '\\') {
			continue;
		}
		int esc_seq_len = 2;
		switch (buf[i + 1]) {
		case 'e':
			buf[i] = 0x1b;
			break;

		case ' ':
		case 's':
			buf[i] = ' ';
			break;

		case '\\':
			buf[i] = '\\';
			break;

		case 'r':
			buf[i] = 0x0d;
			break;

		case 'n':
			buf[i] = 0x0a;
			break;

		case 'a':
			buf[i] = 0x07;
			break;

		case 'b':
			buf[i] = 0x08;
			break;

		case 't':
			buf[i] = 0x09;
			break;

		case 'v':
			buf[i] = 0x0b;
			break;

		case 'f':
			buf[i] = 0x0c;
			break;

		case '"':
			buf[i] = '"';
			break;

		case '\'':
			buf[i] = '\'';
			break;

		case '`':
			buf[i] = '`';
			break;

		case 'x':
			err = ch2 = ch = 0;
			if (!buf[i + 2] || !buf[i + 3]) {
				return (0);
			}

			err |= hex_to_byte(&ch, buf[i + 2]);
			err |= hex_to_byte(&ch2, buf[i + 3]);

			if (err) {
				return (0);
			}

			buf[i]	    = (ch << 4) + ch2;
			esc_seq_len = 4;

			break;

		case '$':
			buf[i] = '$';
			break;

		default:
			if (IS_OCTAL(buf[i + 1])) {
				int num_digits = 1;
				buf[i]	       = buf[i + 1] - '0';

				if (IS_OCTAL(buf[i + 2])) {
					num_digits++;
					buf[i] = (uint8_t)buf[i] * 8 +
						 (buf[i + 2] - '0');

					if (IS_OCTAL(buf[i + 3])) {
						num_digits++;
						buf[i] = (uint8_t
							 )buf[i] *
								 8 +
							 (buf[i + 3] -
							  '0');
					}
				}
				esc_seq_len = 1 + num_digits;

			} else {
				esc_seq_len = 1;
			}
		}
		memmove(buf + i + 1,
			buf + i + esc_seq_len,
			strlen(buf + i + esc_seq_len) + 1);
	}
	return i;
}
