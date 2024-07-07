#ifndef __XRE_ASSERT_H__
#define __XRE_ASSERT_H__

// # define RANGE(start, end) ((start <= end) ? (end - start + 1) : (start - end
// + 1)) # define MIN(a, b) ((a < b) ? a : b)

#include "xre_log.h"
#include <stdio.h>

#ifdef DISABLE_ASSERTS

#define xre_assert(expr)
#define xre_assert_not_reached()

#else /* !XRE_DISABLE_ASSERT */

#ifdef __GNUC__

#define __assert__(expr)                                                \
	do {                                                            \
		if (!(expr))                                            \
			__xre_logger(warning, "Assertion failed: (%s)", \
				     #expr);                            \
	} while (0)

#define __assert_not_reached__()                                \
	do {                                                    \
		__xre_logger(warning, "Should not be reached"); \
	} while (0)

#else /* !__GNUC__ */

#define __assert__(expr)                                                 \
	do {                                                             \
		if (!(expr))                                             \
			__xre_logger(warning, "Assertion failed: (%s)"); \
	} while (0)

#define __assert_not_reached__()                                \
	do {                                                    \
		__xre_logger(warning, "Should not be reached"); \
	} while (0)

#endif /* __GNUC__ */

#endif /* !XRE_DISABLE_ASSERT */

#ifdef XRE_DISABLE_CHECKS

#define __return_if_fail__(expr)
#define __return_val_if_fail__(expr, val)

#else /* !XRE_DISABLE_CHECKS */

#ifdef __GNUC__

#define __return_if_fail__(expr)                                        \
	do {                                                            \
		if (!(expr)) {                                          \
			__xre_logger(warning, "Assertion failed: (%s)", \
				     #expr);                            \
			return;                                         \
		};                                                      \
	} while (0)

#define __return_val_if_fail__(expr, val)                               \
	do {                                                            \
		if (!(expr)) {                                          \
			__xre_logger(warning, "Assertion failed: (%s)", \
				     #expr);                            \
			return val;                                     \
		};                                                      \
	} while (0)

#else /* !__GNUC__ */

#define __return_if_fail__(expr)                                        \
	do {                                                            \
		if (!(expr)) {                                          \
			__xre_logger(warning, "Assertion failed: (%s)", \
				     #expr);                            \
			return;                                         \
		};                                                      \
	} while (0)

#define __return_val_if_fail__(expr, val)                               \
	do {                                                            \
		if (!(expr)) {                                          \
			__xre_logger(warning, "Assertion failed: (%s)", \
				     #expr);                            \
			return val;                                     \
		};                                                      \
	} while (0)

#endif
#endif

#endif /* __XRE_ASSERT_H__ */
