#include "xre_assert.h"
#include "xre_memory.h"
#include "xre_log.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static void register_repr(void *ptr)
{
	//__return_if_fail__(ptr);

	int64_t num = (int64_t)ptr;
#if defined(__linux__)
	(void)fprintf(stderr, "%ld", num);
#else
	(void)fprintf(stderr, "%lld", num);
#endif
}

static bool register_test(void *ptr)
{
	//__return_val_if_fail__(ptr, false);

	return (!!((int64_t)ptr));
}

static void register_drop(void *ptr)
{
	//__return_if_fail__(ptr);
#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "dropping register @%p", ptr);
#endif
	(void)ptr;
}

object_t *object_create_register(int64_t data)
{
	static object_t object = { .attrs = ATTR_REGISTER,
				   .repr = register_repr,
				   .drop = register_drop,
				   .is_true = register_test };

	object.data.ptr = (void *)data;
	object.data.size = sizeof(int64_t);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "created register @%p", object.data.ptr);
#endif
	return (&object);
}

static inline bool extract_register_object_value(object_t *object,
						 int64_t *data)
{
	if (!__object_has_attr(object, ATTR_REGISTER)) {
		return (false);
	}

	*data = __as_int64_t(object);
	return (true);
}

bool unwrap_register_object(ast_stmt_t *self, object_t *object, int64_t *data)
{
	if (!extract_register_object_value(object, data)) {
		return (set_current_error(self, XRE_UNEXPECTED_TYPE_ERROR),
			false);
	}

	return (object_drop(object), true);
}
