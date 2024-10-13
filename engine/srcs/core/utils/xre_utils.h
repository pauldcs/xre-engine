#ifndef __XRE_UTILS_H__
#define __XRE_UTILS_H__

#include "xre_core.h"
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <time.h>

const char *str_to_uint32(const char *str, uint32_t *dst);
char	   *str_to_int32(const char *str, int32_t *dst);
int	    stricmp(char *s1, char *s2);
size_t	    stresc2chr(char *s);
int32_t	    hash_string(const char *key);
ssize_t	    xwrite(int fd, const unsigned char *buf, size_t nbytes);

char *str_escape(const uint8_t *buf, int sz);
int   str_unescape(char *buf);

const char *shortname(const char *name);

bool timespec_to_string(
	const struct timespec st_mtimespec,
	char		     *dest,
	const char	     *format,
	size_t		      dest_size
);

bool st_mode_to_string(mode_t st_mode, char *dest, size_t dest_size);

char *format_string(const char *format, ...);

#endif /* __XRE_UTILS_H__ */
