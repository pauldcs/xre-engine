#ifndef __XRE_CORE_H__
# define __XRE_CORE_H__

# include <stdbool.h>
# include "xre_args.h"

# if defined(__GNUC__) && __GNUC__ >= 4
#  define xre_memcpy(dst, src, size) __builtin_memcpy((void*)dst, (void*)src, size)
#  define xre_memset(b, c, len)      __builtin_memset((void*)b, c, len)
#  define xre_memmove(b, c, len)     __builtin_memmove((void*)b, c, len)
#  define xre_strlen(str)            __builtin_strlen(str)

# ifndef likely
#  define likely(x)	(__builtin_expect(!!(x), 1))
#  define unlikely(x)	(__builtin_expect(!!(x), 0))
# endif 
# else
#  define xre_memcpy(dst, src, size)  memcpy((void*)dst, (void*)src, size)
#  define xre_memmove(dst, src, size) memmove((void*)dst, (void*)src, size)
#  define xre_memset(b, c, len)       memset((void*)b, c, len)
#  define xre_strlen(str)             strlen(str)

#  define likely(x)	
#  define unlikely(x)	
# endif

typedef struct {
	const char *title;
	const char *version;
	bool exit_called;
	bool interrupted;
}	t_xre_state;

extern t_xre_state  __xre_state__;

bool __entry__(t_xre_args *args, const char *path, const char *command);

#endif /* __XRE_CORE_H__ */
