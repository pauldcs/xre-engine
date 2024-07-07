#include "xre_assert.h"
#include "xre_memory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static void undefined_repr(void *ptr)
{
	__return_if_fail__(ptr);

	(void)ptr;
	(void)fprintf(stderr, "undefined");
}

static bool undefined_test(void *ptr)
{
	__return_val_if_fail__(ptr, false);

	(void)ptr;
	return (false);
}

static void udnefined_drop(void *ptr)
{
	__return_if_fail__(ptr);

	(void)ptr;
#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "dropping undefined @%p", ptr);
#endif
}

object_t *object_create_undefined(void)
{
	static object_t object = { .attrs = ATTR_MUTABLE,
				   .repr = undefined_repr,
				   .drop = udnefined_drop,
				   .is_true = undefined_test,
				   .data.ptr = NULL,
				   .data.size = 0 };

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "created undefined @%p", object.data.ptr);
#endif
	return (&object);
}
