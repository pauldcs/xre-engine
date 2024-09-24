#include "xre_compiler.h"
#include "xre_memory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static void undefined_repr(void *ptr)
{
	__return_if_fail__(ptr);

	(void)ptr;
	(void)fprintf(stdout, "undefined");
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

void object_undefined_init(object_t *buf)
{
	buf->repr    = undefined_repr;
	buf->drop    = udnefined_drop;
	buf->is_true = undefined_test;

	__object_set_attr(buf, ATTR_MUTABLE);
	__object_set_data_as_any(buf, NULL);
	__object_set_ref_count(buf, 0);
	//__object_set_invalid_address(&object);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "created undefined @%p", object.data.ptr);
#endif
}
