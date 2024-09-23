#include "xre_compiler.h"
#include "xre_memory.h"
#include "xre_log.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static void number_repr(void *ptr)
{
#if defined(__linux__)
	(void)fprintf(stdout, "%ld", (int64_t)ptr);
#else
	(void)fprintf(stdout, "%lld", (int64_t)ptr);
#endif
}

static bool number_test(void *ptr)
{
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

void object_number_init(int64_t data, object_t *buf)
{
	__trigger_bug_if(buf == NULL);

	buf->repr    = number_repr;
	buf->drop    = number_drop;
	buf->is_true = number_test;

	__object_set_attr(buf, ATTR_NUMBER);
	__object_set_data_as_number(buf, data);
	__object_set_ref_count(buf, 0);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "created register @%p", object.data.ptr);
#endif
}

static inline bool
extract_number_object_value(object_t *object, int64_t *data)
{
	if (!__object_has_attr(object, ATTR_NUMBER)) {
		return (false);
	}

	*data = __object_get_data_as_number(object);

	return (true);
}

bool unwrap_number_object(
	struct statement *self, object_t *object, int64_t *data
)
{
	if (!extract_number_object_value(object, data)) {
		return (set_current_error(
				self, XRE_UNEXPECTED_TYPE_ERROR
			),
			false);
	}

	return (object_drop(object), true);
}
