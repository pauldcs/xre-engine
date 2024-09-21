#include "dstr.h"
#include "xre_compiler.h"
#include "xre_memory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static bool string_test(void *ptr)
{
	__return_val_if_fail__(ptr, false);

	return (((dstr_t *)ptr)->len > 1);
}

static void string_repr(void *ptr)
{
	__return_if_fail__(ptr);

	// char *escaped = str_escape(ptr, strlen(ptr));
	(void)fprintf(stdout, "%s", ((dstr_t *)ptr)->buf);
	// free(escaped);
}

static void string_drop(void *ptr)
{
	__return_if_fail__(ptr);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "dropping string @%p", ptr);
#endif
	dstr_release((dstr_t *)ptr);
}

bool object_string_init(unsigned char *string, object_t *buf)
{
	__return_val_if_fail__(string, NULL);

	buf->repr    = string_repr;
	buf->drop    = string_drop;
	buf->is_true = string_test;

	size_t	str_len = strlen((const char *)string);
	dstr_t *dstr	= malloc(sizeof(dstr_t));

	(void)dstr_init(dstr, str_len);
	(void)dstr_addstr(dstr, (const char *)string);

	__object_set_attr(buf, ATTR_STRING);
	__object_set_data_as_string(buf, dstr);
	__object_set_ref_count(buf, 0);
	//__object_set_invalid_address(&object);

#if defined XRE_ENABLE_OBJECT_LOGGING
	__xre_logger(info, "created string @%p", object.data.ptr);
#endif
	return (true);
}
