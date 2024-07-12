#include "dynstr.h"
#include "xre_assert.h"
#include "xre_memory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static bool string_test(void *ptr)
{
	__return_val_if_fail__(ptr, false);

	return (dynstr_len((dynstr_t *)ptr) > 1);
}

static void string_repr(void *ptr)
{
	__return_if_fail__(ptr);

	// char *escaped = str_escape(ptr, strlen(ptr));
	(void)fprintf(stderr, "%s", ((dynstr_t *)ptr)->_ptr);
	// free(escaped);
}

static void string_drop(void *ptr)
{
	__return_if_fail__(ptr);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "dropping string @%p", ptr);
#endif
	dynstr_kill((dynstr_t *)ptr);
}

object_t *object_string_create(unsigned char *string)
{
	__return_val_if_fail__(string, NULL);

	static object_t object = { .repr = string_repr,
				   .drop = string_drop,
				   .is_true = string_test };

	size_t str_len = strlen((const char *)string);
	dynstr_t *dynstr = dynstr_from((const char *)string, str_len);
	if (!dynstr) {
		return (NULL);
	}

	__object_set_attr(&object, ATTR_STRING);
	__object_set_data_as_string(&object, dynstr);
	__object_set_ref_count(&object, 0);
	//__object_set_invalid_address(&object);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "created string @%p", object.data.ptr);
#endif
	return (&object);
}
