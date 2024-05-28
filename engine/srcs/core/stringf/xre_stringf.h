#ifndef __XRE_STRINGF_H__
#define __XRE_STRINGF_H__

#if defined(__GNUC__) && __GNUC__ >= 4
#define xmemcpy(dst, src, size) __builtin_memcpy((void *)dst, (void *)src, size)
#define xmemset(b, c, len) __builtin_memset((void *)b, c, len)
#define xmemmove(b, c, len) __builtin_memmove((void *)b, c, len)
#define xstrlen(str) __builtin_strlen(str)
#else
#define xmemcpy(dst, src, size) memcpy((void *)dst, (void *)src, size)
#define xmemmove(dst, src, size) memmove((void *)dst, (void *)src, size)
#define xmemset(b, c, len) memset((void *)b, c, len)
#define xstrlen(str) strlen(str)
#endif

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#define IOBUF_MAX 4096

typedef struct s_iobuf {
  void *data;
  size_t cap;
  size_t len;
  size_t disc;
  size_t width;
} t_iobuf;

/**
 * @brief copies a maximum of 'dstsize' bytes of 'format' into 'dst'.
 * The result will be truncated if the format string is larger than
 * IOBUF_MAX or 'dstsize'. the result is not null terminated.
 *
 * cpyf() is used to write formatted output in the middle of a buffer.
 *
 * @param dst The destination buffer
 * @param dstsize size of 'dst'
 * @param format The format string
 * @param ...
 * @return number of bytes written
 */
size_t cpyf(void *dst, size_t dstsize, const char *format, ...);

/**
 * @brief copies 'format' into 'str'.
 * The result will be truncated if the format string is larger than
 * IOBUF_MAX. The behavior is undefined if str is null.
 *
 * @param str The destination string
 * @param format The format string
 * @param ...
 * @return number of bytes written
 */
size_t scpyf(char *str, const char *format, ...);

/**
 * @brief Behaves as strlcpy, the src being the format
 * string. Null termination is guaranteed unless dstsize is 0.
 *
 * @param dst the destination buffer
 * @param dstsize the size of 'dst'
 * @param format the format string
 * @param ...
 * @return identical to strlcpy
 */
size_t slcpyf(char *dst, size_t dstsize, const char *format, ...);

/**
 * @brief copies the format string into a new allocated space placed into 'dst'
 * If the allocation fails, 'buf' is null and the return value is -1.
 * In any other case 'buf' is guaranteed to be null terminated.
 * The result will be truncated if the format string is larger than
 * IOBUF_MAX.
 * @param dst pointer to the destination buffer
 * @param format the format string
 * @param ...
 * @return the number of bytes copied + the number of bytes truncated
 */
ssize_t ssavef(char **dst, const char *format, ...);

size_t fputstr(int fd, const char *format, ...);

#endif /* __XRE_STRINGF_H__ */
