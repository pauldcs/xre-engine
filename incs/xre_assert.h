#ifndef __XRE_ASSERT_H__
# define __XRE_ASSERT_H__

// # define RANGE(start, end) ((start <= end) ? (end - start + 1) : (start - end + 1))
// # define MIN(a, b) ((a < b) ? a : b)

# include "xre_log.h"
# include <stdio.h>

# ifdef G_DISABLE_ASSERT

#define xre_assert(expr)
#define xre_assert_not_reached()

# else /* !XRE_DISABLE_ASSERT */

# ifdef __GNUC__

#  define __assert__(expr) \
	do { \
		if (!(expr)) \
			XRE_LOGGER(warning, "file '%s', line: %d\nassertion failed: (%s)\n", \
			__FILE__, __LINE__ \
			#expr); \
	} while (0)

#  define __assert_not_reached__() \
	do { \
		XRE_LOGGER(warning, "file '%s', line: %d\nshould not be reached\n", \
			__FILE__, __LINE__); \
	} while (0)

# else /* !__GNUC__ */

#  define __assert__(expr) \
	do { \
		if (!(expr)) \
			XRE_LOGGER(warning, "file '%s', line: %d\n(%s): assertion failed: (%s)\n", \
				__FILE__, __LINE__, \
				#expr); \
	} while (0)

#  define __assert_not_reached__() \
	do { \
    	XRE_LOGGER(warning, "file '%s', line: %d\n(%s): should not be reached\n",	\
	    __FILE__, __LINE__); \
	} while (0)

# endif /* __GNUC__ */

# endif /* !XRE_DISABLE_ASSERT */

# ifdef XRE_DISABLE_CHECKS

# define __return_if_fail__(expr)
# define __return_val_if_fail__(expr,val)

# else /* !XRE_DISABLE_CHECKS */

# ifdef __GNUC__

# define __return_if_fail__(expr) \
	do { \
		if (!(expr)) { \
			XRE_LOGGER(warning, "file '%s', line: %d\nassertion failed: (%s)\n", \
				__FILE__, __LINE__, \
				#expr);	\
			return;	\
		}; \
	} while (0)

# define __return_val_if_fail__(expr,val) \
	do { \
    	if (!(expr)) { \
	 		XRE_LOGGER(warning, "file '%s', line: %d\nassertion failed: (%s)\n", \
				__FILE__, __LINE__, \
				#expr);	\
	 		return val; \
       	}; \
	} while (0)

# else /* !__GNUC__ */

# define __return_if_fail__(expr) \
	do { \
    	if (!(expr)) { \
			XRE_LOGGER(warning, "file '%s', line: %d\n(%s): assertion failed: (%s)\n",	\
				__FILE__, __LINE__, \
				#expr); \
	 		return; \
       	}; \
	} while (0)

# define __return_val_if_fail__(expr, val)	\
	do { \
		if (!(expr)) {	\
			XRE_LOGGER (warning, "file '%s', line: %d\n(%s): assertion failed: (%s)\n",	\
				__FILE__, __LINE__, \
				#expr); \
	 		return val; \
       	}; \
	} while (0)

#  endif
# endif

#endif /* __XRE_ASSERT_H__ */
