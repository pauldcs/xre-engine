#include "dynstr.h"
#include "xre_assert.h"
#include "xre_memory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static void string_drop(void *ptr)
{
	__return_if_fail__(ptr);

	dynstr_kill((dynstr_t *)ptr);
}

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

object_t *object_create_string(unsigned char *ptr, size_t size)
{
	__return_val_if_fail__(ptr, NULL);

	static object_t object = { .attrs = ATTR_STRING,
				   .repr = string_repr,
				   .drop = string_drop,
				   .is_true = string_test };

	object.data.ptr = dynstr_from((const char *)ptr, size);
	if (!object.data.ptr) {
		return (NULL);
	}

	object.data.size = size;
	return (&object);
}
