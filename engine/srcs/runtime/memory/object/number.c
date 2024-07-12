#include "xre_assert.h"
#include "xre_memory.h"
#include "xre_log.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static void number_repr(void *ptr)
{
	//__return_if_fail__(ptr);

	int64_t num = (int64_t)ptr;
#if defined(__linux__)
	(void)fprintf(stderr, "%ld", num);
#else
	(void)fprintf(stderr, "%lld", num);
#endif
}

static bool number_test(void *ptr)
{
	//__return_val_if_fail__(ptr, false);

	return (!!((int64_t)ptr));
}

static void number_drop(void *ptr)
{
	//__return_if_fail__(ptr);
#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "dropping register @%p", ptr);
#endif
	(void)ptr;
}

object_t *object_number_create(int64_t data)
{
	static object_t object = { .repr = number_repr,
				   .drop = number_drop,
				   .is_true = number_test };

	__object_set_attr(&object, ATTR_NUMBER);
	__object_set_data_as_number(&object, data);
	__object_set_ref_count(&object, 0);
	//__object_set_invalid_address(&object);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "created register @%p", object.data.ptr);
#endif
	return (&object);
}

static inline bool extract_number_object_value(object_t *object, int64_t *data)
{
	if (!__object_has_attr(object, ATTR_NUMBER)) {
		return (false);
	}

	*data = __object_get_data_as_number(object);

	return (true);
}

bool unwrap_number_object(ast_stmt_t *self, object_t *object, int64_t *data)
{
	if (!extract_number_object_value(object, data)) {
		return (set_current_error(self, XRE_UNEXPECTED_TYPE_ERROR),
			false);
	}

	return (object_drop(object), true);
}
