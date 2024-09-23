#ifndef __XRE_COMPILER_H__
#define __XRE_COMPILER_H__

#include <stdio.h>
#include <signal.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <libgen.h>

extern int __trace_depth;

#define __trace_in(func_name)                                \
	do {                                                 \
		for (size_t i = __trace_depth; i > 0; i--) { \
			(void)fprintf(stderr, "    ");       \
		}                                            \
		__trace_depth++;                             \
		(void)fprintf(stderr, " %s\n", (func_name)); \
	} while (0)
#define __trace_out() __trace_depth--;

#ifdef DISABLE_CHECKS

#define __trigger_bug()
#define __trigger_bug_if_fail(expr)
#define __return_if_fail__(expr)
#define __return_val_if_fail__(expr, val)

#else /* !DISABLE_CHECKS */

// Triggers a SIGSEGV so that sanitizers can display a stack trace.
// Useful for unveiling the code path that lead to the bug.
#define __trigger_bug()                                \
	do {                                           \
		(void)fprintf(                         \
			stderr,                        \
			"bug: %s:%d (killing self)\n", \
			__FILE__,                      \
			__LINE__                       \
		);                                     \
		fflush(stderr);                        \
		raise(SIGSEGV);                        \
		__builtin_unreachable();               \
	} while (0)

#define __trigger_bug_if(expr)           \
	do {                             \
		if (expr) {              \
			__trigger_bug(); \
		}                        \
	} while (0)

#define __trigger_bug_if_fail(expr)      \
	do {                             \
		if (!(expr)) {           \
			__trigger_bug(); \
		}                        \
	} while (0)

#define __return_if_fail__(expr)                                  \
	do {                                                      \
		if (!(expr)) {                                    \
			(void)fprintf(                            \
				stderr,                           \
				"Assertion failed: %s:%d (%s)\n", \
				__FILE__,                         \
				__LINE__,                         \
				#expr                             \
			);                                        \
			return;                                   \
		};                                                \
	} while (0)

#define __return_val_if_fail__(expr, val)                         \
	do {                                                      \
		if (!(expr)) {                                    \
			(void)fprintf(                            \
				stderr,                           \
				"Assertion failed: %s:%d (%s)\n", \
				__FILE__,                         \
				__LINE__,                         \
				#expr                             \
			);                                        \
			return val;                               \
		};                                                \
	} while (0)
#endif

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#define bitsizeof(x) (CHAR_BIT * sizeof(x))

#define maximum_signed_value_of_type(a) \
	(INTMAX_MAX >> (bitsizeof(intmax_t) - bitsizeof(a)))

#define maximum_unsigned_value_of_type(a) \
	(UINTMAX_MAX >> (bitsizeof(uintmax_t) - bitsizeof(a)))

/*
 * Signed integer overflow is undefined in C, so here's a helper macro
 * to detect if the sum of two integers will overflow.
 *
 * Requires: a >= 0, typeof(a) equals typeof(b)
 */
#define signed_add_overflows(a, b) \
	((b) > maximum_signed_value_of_type(a) - (a))

#define unsigned_add_overflows(a, b) \
	((b) > maximum_unsigned_value_of_type(a) - (a))

/*
 * Returns true if the multiplication of "a" and "b" will
 * overflow. The types of "a" and "b" must match and must be unsigned.
 * Note that this macro evaluates "a" twice!
 */
#define unsigned_mult_overflows(a, b) \
	((a) && (b) > maximum_unsigned_value_of_type(a) / (a))

/*
 * Returns true if the left shift of "a" by "shift" bits will
 * overflow. The type of "a" must be unsigned.
 */
#define unsigned_left_shift_overflows(a, shift) \
	((shift) < bitsizeof(a) &&              \
	 (a) > maximum_unsigned_value_of_type(a) >> (shift))

static inline size_t st_add(size_t a, size_t b)
{
	if (unsigned_add_overflows(a, b)) {
		assert(0 && "overflow");
	}

	return (a + b);
}
#define st_add3(a, b, c)    st_add(st_add((a), (b)), (c))
#define st_add4(a, b, c, d) st_add(st_add3((a), (b), (c)), (d))

static inline size_t st_mult(size_t a, size_t b)
{
	if (unsigned_mult_overflows(a, b)) {
		assert(0 && "overflow");
	}

	return (a * b);
}

static inline size_t st_sub(size_t a, size_t b)
{
	if (a < b) {
		assert(0 && "overflow");
	}

	return (a - b);
}

static inline size_t st_left_shift(size_t a, unsigned shift)
{
	if (unsigned_left_shift_overflows(a, shift)) {
		assert(0 && "overflow");
	}

	return (a << shift);
}

static inline unsigned long cast_size_t_to_ulong(size_t a)
{
	if (a != (unsigned long)a) {
		assert(0 && "overflow");
	}

	return ((unsigned long)a);
}

static inline uint32_t cast_size_t_to_uint32_t(size_t a)
{
	if (a != (uint32_t)a) {
		assert(0 && "overflow");
	}

	return ((uint32_t)a);
}

static inline int cast_size_t_to_int(size_t a)
{
	if (a > INT_MAX) {
		assert(0 && "overflow");
	}
	return ((int)a);
}
#pragma GCC diagnostic pop

#endif /* __XRE_COMPILER_H__ */
