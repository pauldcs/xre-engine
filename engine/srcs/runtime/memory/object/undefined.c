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

object_t *object_undefined_create(void)
{
	static object_t object = { .repr = undefined_repr,
				   .drop = udnefined_drop,
				   .is_true = undefined_test };

	__object_set_attr(&object, ATTR_MUTABLE);
	__object_set_data_ptr(&object, NULL);
	__object_set_data_size(&object, 0);
	__object_set_ref_count(&object, 0);
	//__object_set_invalid_address(&object);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "created undefined @%p", object.data.ptr);
#endif
	return (&object);
}
